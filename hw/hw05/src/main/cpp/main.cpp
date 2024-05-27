#include <iostream>

#include <json/json.h>

#include "dataset.h"
#include "result.h"

#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>

#include "measurements.av.hh"
#include "measurements.pb.h"

#include <google/protobuf/util/delimited_message_util.h>

#include <chrono>
#include <thread>

using namespace std;
using boost::asio::ip::tcp;

void processJSON(tcp::iostream &stream)
{
   Json::Value val;
   Json::Reader reader;

   std::vector<Dataset> datasets;
   std::vector<Result> results;

   /* Read json string from the stream */
   string s;
   getline(stream, s, '\0');

   /* Parse string */
   reader.parse(s, val);

   datasets.clear();
   results.clear();
   for (int i = 0; i < val.size(); i++) {
      datasets.emplace_back();
      datasets[i].Deserialize(val[i]);
      /* Calculate averages */
      results.emplace_back(datasets[i].getInfo(), datasets[i].getRecords());
   }

   /* Create output JSON structure */
   Json::Value out;
   //    Json::FastWriter writer;
   Json::StyledWriter writer;
   for (int i = 0; i < results.size(); i++) {
      Json::Value result;
      results[i].Serialize(result);
      out[i] = result;
   }

   /* Send the result back */
   std::string output = writer.write(out);
   stream << output;
   if (!getenv("ESW_JSON_QUIET"))
      cout << output;
   else
      cout << "Quiet operation enabled" << endl;
}

int readAndDecodeMessageSize(tcp::iostream &stream)
{
   char sizeBuffer[4]; // Assuming 4 bytes for message size
   stream.read(sizeBuffer, 4);
   int messageSize = 0;
   // Assuming network byte order, big endian
   messageSize |= (unsigned char)sizeBuffer[0] << 24;
   messageSize |= (unsigned char)sizeBuffer[1] << 16;
   messageSize |= (unsigned char)sizeBuffer[2] << 8;
   messageSize |= (unsigned char)sizeBuffer[3];
   return messageSize;
}

void processAvro(tcp::iostream &stream)
{
   // 1. Read incoming message
   int messageSize = readAndDecodeMessageSize(stream);
   char *buffer = new char[messageSize];
   memset(buffer, 0, messageSize); // Initialize buffer to zero
   stream.read(buffer, messageSize);
   std::cout << "Recieved bytes: " << messageSize << std::endl;

   // 2. Deserialize the message
   eswavro::AVMeasurementArray receivedData;
   std::unique_ptr<avro::InputStream> stream_in_p =
       avro::memoryInputStream((uint8_t *)buffer, messageSize);
   avro::DecoderPtr decoder = avro::binaryDecoder();
   decoder->init(*stream_in_p);

   avro::decode(*decoder, receivedData);

   // 3. Process data
   eswavro::AVAvgDataArray responseData;
   for (const auto &measurement : receivedData.datasets) {
      eswavro::AVAvgData avgData;
      avgData.info = measurement.info;

      // Calculate averages
      double totalDownload = 0, totalUpload = 0, totalPing = 0;
      int count = measurement.download
                      .size(); // Assuming equal sizes for upload and ping

      for (int i = 0; i < count; ++i) {
         totalDownload += measurement.download[i];
         totalUpload += measurement.upload[i];
         totalPing += measurement.ping[i];
      }

      if (count > 0) {
         avgData.download = (totalDownload / count);
         avgData.upload = (totalUpload / count);
         avgData.ping = (totalPing / count);
      }

      responseData.dataArray.push_back(avgData);
   }

   // Step 4: Serialize the response
   // std::unique_ptr<avro::OutputStream> outStream =
   // avro::ostreamOutputStream(stream);

   std::unique_ptr<avro::OutputStream> outputStream =
       avro::ostreamOutputStream(stream);
   avro::EncoderPtr encoder = avro::binaryEncoder();
   encoder->init(*outputStream);
   avro::encode(*encoder, responseData);
   encoder->flush();

   // avro::OutputStreamPtr out = avro::memoryOutputStream();
   // avro::EncoderPtr encoder = avro::binaryEncoder();
   // encoder->init(*out);

   // avro::encode(*encoder, responseData);
   // encoder->flush();

   // // const std::vector<uint8_t>& data = avro::snapshot(*out);
   // const std::vector<uint8_t>& data = *avro::snapshot(*out);

   // std::cout << "size: " << data.size() << std::endl;

   // stream.write(reinterpret_cast<const char*>(data.data()), data.size());

   delete[] buffer;
}

void processProtobuf(tcp::iostream &stream)
{
   // 1. Read incoming message
   int messageSize = readAndDecodeMessageSize(stream);
   char *buffer = new char[messageSize];
   memset(buffer, 0, messageSize); // Initialize buffer to zero
   stream.read(buffer, messageSize);

   std::cout << "Recieved bytes: " << messageSize << std::endl;

   // 2. Deserialize the message
   esw::PBMeasurementArray receivedData;
   if (!receivedData.ParseFromArray(buffer, messageSize)) {
      delete[] buffer;
      return;
   }

   // 3. Process data
   esw::PBAvgDataArray responseData;
   for (const auto &measurement : receivedData.datasets()) {
      esw::PBAvgData avgData;
      *avgData.mutable_info() = measurement.info();

      // Calculate averages
      double totalDownload = 0, totalUpload = 0, totalPing = 0;
      int count =
          measurement
              .download_size(); // Assuming equal sizes for upload and ping

      for (int i = 0; i < count; ++i) {
         totalDownload += measurement.download(i);
         totalUpload += measurement.upload(i);
         totalPing += measurement.ping(i);
      }

      if (count > 0) {
         avgData.set_download(totalDownload / count);
         avgData.set_upload(totalUpload / count);
         avgData.set_ping(totalPing / count);
      }

      *responseData.add_dataarray() = avgData;
   }
   auto test = responseData.mutable_dataarray();
   std::vector<esw::PBAvgData> test2(test->begin(), test->end());

   // Step 4: Serialize the response
   std::string output;
   if (!responseData.SerializeToString(&output)) {
      std::cerr << "Failed to serialize response data." << std::endl;
      return;
   }

   std::cout << "Sending bytes: " << output.size() << std::endl;

   // Step 5: Write the response back

   int size = output.size();
   stream.write(reinterpret_cast<const char *>(&size), 4);

   stream.write(output.c_str(),
                output.size() +
                    1); // Include the null character to maintain consistency
   stream.flush();
}

int main(int argc, char *argv[])
{

   if (argc != 3) {
      cout << "Error: two arguments required - ./server  <port> <protocol>"
           << endl;
      return 1;
   }

   // unsigned short int port = 12345;
   unsigned short int port = atoi(argv[1]);

   // std::string protocol = "json";
   std::string protocol(argv[2]);
   boost::to_upper(protocol);
   try {
      boost::asio::io_service io_service;

      tcp::endpoint endpoint(tcp::v4(), port);
      tcp::acceptor acceptor(io_service, endpoint);

      while (true) {
         cout << "Waiting for message in " + protocol + " format..." << endl;
         tcp::iostream stream;
         boost::system::error_code ec;
         acceptor.accept(*stream.rdbuf(), ec);

         if (protocol == "JSON") {
            processJSON(stream);
         } else if (protocol == "AVRO") {
            processAvro(stream);
         } else if (protocol == "PROTO") {
            processProtobuf(stream);
         } else {
            throw std::logic_error("Protocol not yet implemented");
         }
      }
   } catch (std::exception &e) {
      std::cerr << "Exception: " << e.what() << std::endl;
      return 1;
   }

   return 0;
}
