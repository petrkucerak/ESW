package cz.cvut.fel.esw.matrixmultiplication;

import org.openjdk.jmh.annotations.Benchmark;
import org.openjdk.jmh.annotations.Mode;
import org.openjdk.jmh.results.format.ResultFormatType;
import org.openjdk.jmh.runner.Runner;
import org.openjdk.jmh.runner.options.ChainedOptionsBuilder;
import org.openjdk.jmh.runner.options.OptionsBuilder;
import org.openjdk.jmh.runner.options.TimeValue;

import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.util.concurrent.TimeUnit;

public class MatrixMultiplicationBenchmark {

    public static void main(String... args) throws Exception {
        ChainedOptionsBuilder opts = new OptionsBuilder()
                // regex which benchmarks to be included
                // this includes all @Benchmark annotated methods in this class
                // e.g. "*" will include all @Benchmark annotated methods found in the classpath
                .include(MatrixMultiplicationBenchmark.class.getSimpleName())
                // number of warm-up iterations
                // set to 0 for the initial run where we want to select the warm-up for each implementation
                // if we want to set different warm-up for each implementation (and we most likely do),
                // the annotations (@Warmup(iterations = ...)) has to be used on each benchmark method and this function cannot be used (it overrides the annotations)
                .warmupIterations(0)
                // min duration of warm-up iteration
                // 1ms because we want to work with just one invocation per iteration in this benchmark
                .warmupTime(TimeValue.milliseconds(1))
                // number of measured iterations
                .measurementIterations(100)
                // min duration of measured iteration
                // 1ms because we want to work with just one invocation per iteration in this benchmark
                .measurementTime(TimeValue.milliseconds(1))
                //number of executions
                .forks(3)
                // what we want to measure
                .mode(Mode.AverageTime)
                // set JVM args used for the measurements
                // if not set (at least to emtpy args), all JVM args used to run the parent will be used also for the forks
                .jvmArgs()
                // time units at which the results are stored
                .timeUnit(TimeUnit.MILLISECONDS)
                // where to store the results
                .result("matrix_multiplication_benchmark_" + getCurrentTimeString() + ".json")
                // format used to store the results
                .resultFormat(ResultFormatType.JSON);
        new Runner(opts.build()).run();
    }

    @Benchmark
    public void measureMultiply() {
    }

    @Benchmark
    public void measureMultiply1D() {
    }

    @Benchmark
    public void measureMultiplyTrans() {
    }

    public static String getCurrentTimeString() {
        return LocalDateTime.now().format(DateTimeFormatter.ofPattern("YYYYMMdd_HHmmss"));
    }

}