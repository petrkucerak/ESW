/**
 * Autogenerated by Avro
 *
 * DO NOT EDIT DIRECTLY
 */
package cz.esw.serialization.avro;

/** Protocol in Avro IDL

WARNING: avrogencpp accepts only Avro Schema (.avsc) files. Therefore, you need to define directly the schema,
or you can use avro-tools to convert IDL to the schema (or IDEA Avro plugin supports the conversion). */
@org.apache.avro.specific.AvroGenerated
public interface Protocol {
  public static final org.apache.avro.Protocol PROTOCOL = org.apache.avro.Protocol.parse("{\"protocol\":\"Protocol\",\"namespace\":\"cz.esw.serialization.avro\",\"doc\":\"Protocol in Avro IDL\\n\\nWARNING: avrogencpp accepts only Avro Schema (.avsc) files. Therefore, you need to define directly the schema,\\nor you can use avro-tools to convert IDL to the schema (or IDEA Avro plugin supports the conversion).\",\"types\":[],\"messages\":{}}");

  /** Protocol in Avro IDL

WARNING: avrogencpp accepts only Avro Schema (.avsc) files. Therefore, you need to define directly the schema,
or you can use avro-tools to convert IDL to the schema (or IDEA Avro plugin supports the conversion). */
  @org.apache.avro.specific.AvroGenerated
  public interface Callback extends Protocol {
    public static final org.apache.avro.Protocol PROTOCOL = cz.esw.serialization.avro.Protocol.PROTOCOL;
  }
}