package cz.esw.serialization.handler;

import cz.esw.serialization.ResultConsumer;
import cz.esw.serialization.json.DataType;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.*;

import cz.esw.serialization.avro.AVInfo;
import cz.esw.serialization.avro.AVMeasurement;
import cz.esw.serialization.avro.AVMeasurementArray;
import cz.esw.serialization.avro.AVAvgData;
import cz.esw.serialization.avro.AVAvgDataArray;
import cz.esw.serialization.json.Dataset;
import cz.esw.serialization.json.MeasurementInfo;
import cz.esw.serialization.proto.*;
import org.apache.avro.file.DataFileStream;
import org.apache.avro.io.BinaryEncoder;
import org.apache.avro.io.DatumReader;
import org.apache.avro.io.DatumWriter;
import org.apache.avro.io.EncoderFactory;
import org.apache.avro.specific.SpecificDatumReader;
import org.apache.avro.specific.SpecificDatumWriter;

/**
 * @author Marek Cuchý (CVUT)
 */
public class AvroDataHandler implements DataHandler {

    private final InputStream is;
    private final OutputStream os;

    protected Map<Integer, Dataset> datasets;

    public AvroDataHandler(InputStream is, OutputStream os) {
        this.is = is;
        this.os = os;
    }

    @Override
    public void initialize() {
        datasets = new HashMap<>();
    }

    @Override
    public void handleNewDataset(int datasetId, long timestamp, String measurerName) {
        MeasurementInfo info = new MeasurementInfo(datasetId, timestamp, measurerName);
        Map<DataType, List<Double>> recordMap = new EnumMap<>(DataType.class);

        datasets.put(datasetId, new Dataset(info, recordMap));
    }

    @Override
    public void handleValue(int datasetId, DataType type, double value) {
        Dataset dataset = datasets.get(datasetId);
        if (dataset == null) {
            throw new IllegalArgumentException("Dataset with id " + datasetId + " not initialized.");
        }
        dataset.getRecords().computeIfAbsent(type, t -> new ArrayList<>()).add(value);
    }

    @Override
    public void getResults(ResultConsumer consumer) throws IOException {
        ArrayList<AVMeasurement> measurements = new ArrayList<>();

        datasets.forEach((datasetid, dataset) -> {
            MeasurementInfo measurementInfo = dataset.getInfo();
            AVInfo info = AVInfo.newBuilder()
                    .setId(measurementInfo.id())
                    .setMeasurerName(measurementInfo.measurerName())
                    .setTimestamp(measurementInfo.timestamp())
                    .build();

            List<Double> downloadList = dataset.getRecords().get(DataType.DOWNLOAD);
            List<Double> uploadList = dataset.getRecords().get(DataType.UPLOAD);
            List<Double> pingList = dataset.getRecords().get(DataType.PING);

            AVMeasurement.Builder measurementBuilder = AVMeasurement.newBuilder()
                    .setInfo(info);

            measurementBuilder.setDownload(downloadList);
            measurementBuilder.setUpload(uploadList);
            measurementBuilder.setPing(pingList);

            AVMeasurement measurement = measurementBuilder.build();

            measurements.add(measurement);
        });

        AVMeasurementArray measurementArray = new AVMeasurementArray(measurements);

        DatumWriter<AVMeasurementArray> datumWriter = new SpecificDatumWriter<AVMeasurementArray>(AVMeasurementArray.class);
        ByteArrayOutputStream byteArrayOutputStream = new ByteArrayOutputStream();
        BinaryEncoder encoder = EncoderFactory.get().binaryEncoder(byteArrayOutputStream, null);
        datumWriter.write(measurementArray, encoder);
        encoder.flush();

        int messageSize = byteArrayOutputStream.size();
		os.write((messageSize >>> 24) & 0xFF);
		os.write((messageSize >>> 16) & 0xFF);
		os.write((messageSize >>> 8) & 0xFF);
		os.write(messageSize & 0xFF);
		byteArrayOutputStream.writeTo(os);

		System.out.println("Sending bytes: " + messageSize);
        try {
            Thread.sleep(1000);
        } catch (InterruptedException e) {
            throw new RuntimeException(e);
        }
        DatumReader<AVAvgDataArray> resultsReader = new SpecificDatumReader<>(AVAvgDataArray.class);
        DataFileStream<AVAvgDataArray> dataFileStream = new DataFileStream<>(is, resultsReader);

        while (dataFileStream.hasNext()) {
			AVAvgDataArray avgDataArray = dataFileStream.next();
			for (AVAvgData avgData : avgDataArray.getDataArray()) {
				consumer.acceptMeasurementInfo((int) avgData.getInfo().getId(), (long) avgData.getInfo().getTimestamp(), (String) avgData.getInfo().getMeasurerName());
				consumer.acceptResult(DataType.UPLOAD, avgData.getUpload());
				consumer.acceptResult(DataType.DOWNLOAD, avgData.getDownload());
				consumer.acceptResult(DataType.PING, avgData.getPing());
			}
		}
		dataFileStream.close();
    }
}
