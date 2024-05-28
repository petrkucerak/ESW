package cz.cvut.fel.esw;

import jdk.incubator.vector.IntVector;
import jdk.incubator.vector.VectorOperators;
import jdk.incubator.vector.VectorSpecies;

/**
 * @author Marek Cuchý (CVUT)
 */
public class Utils {

    static final VectorSpecies<Integer> SPECIES = IntVector.SPECIES_PREFERRED;

    public static int sum(int[] array) {

        int sum = 0;
        for (int i = 0; i < array.length; i += SPECIES.length()) {
            var mask = SPECIES.indexInRange(i, array.length);
            var V = IntVector.fromArray(SPECIES, array, i, mask);
            sum += V.reduceLanes(VectorOperators.ADD, mask);
        }
        return sum;
    }


    public static int dot(int[] v1, int[] v2) {
        if (v1.length != v2.length) {
            throw new IllegalArgumentException();
        }
        int dotProduct = 0;
        int i = 0;
        for(i = 0; i <= v1.length - SPECIES.length(); i += SPECIES.length()){
            var V1 = IntVector.fromArray(SPECIES, v1, i);
            var V2 = IntVector.fromArray(SPECIES, v2, i);
            var product = V1.mul(V2);
            dotProduct += product.reduceLanes(VectorOperators.ADD);
        }

        for(; i < v1.length; ++i){
            dotProduct += v1[i] * v2[i];
        }
        return dotProduct;
    }

    public static Matrix multiply(Matrix left, Matrix right) {
        int[][] a = left.rowBased();
        int[][] bt = right.columnBased();
        int n = left.rows();
        int m = left.columns();
        int p = right.columns();

        int[][] res = new int[n][p];

        for (int i = 0; i < n; i++) {
            for (int j = 0; j < p; j++) {
                for (int k = 0; k < m; k++) {
                    res[i][j] = dot(a[i], bt[j]);
                }
            }
        }
        return new Matrix(res);
    }
}
