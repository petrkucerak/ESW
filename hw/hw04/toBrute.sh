cp src/test/java/cz/cvut/fel/esw/matrixmultiplication/MatrixMultiplicationBenchmark.java MatrixMultiplicationBenchmark.java
cp assets/report/benchmark_template.pdf report.pdf
zip -r kucerp28.zip results.json measurements.json report.pdf MatrixMultiplicationBenchmark.java
rm MatrixMultiplicationBenchmark.java