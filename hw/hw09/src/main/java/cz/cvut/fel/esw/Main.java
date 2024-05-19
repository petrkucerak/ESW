package cz.cvut.fel.esw;

import java.util.Random;
import java.util.random.RandomGenerator;

/**
 * @author Marek Cuchý (CVUT)
 */
public class Main {

    public static void main(String[] args) {
        RandomGenerator rnd = new Random(0);

        int n = 5;
        int m = 10;
        int p = 7;
        int ub = 10;

        Matrix left = Matrix.generateMatrix(rnd, n, m, ub);
        Matrix right = Matrix.generateMatrix(rnd, m, p, ub);

        Matrix result = Utils.multiply(left, right);
        System.out.println(result);

        /*
        int arraySize = 1000000;
        int [] array = new int[arraySize];
        int arraySum = Utils.sum(array);
        System.out.println(arraySum);
        */
    }
}