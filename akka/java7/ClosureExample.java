//-*- coding: utf-8-unix -*-
/**
 * (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 *
 * see also http://brodowsky.it-sky.net/2013/08/09/closures-ii-java/ for explanation (in German)
 *
 * using inner classes to do Closures in Java 7 or older
 */

public class ClosureExample {


    public Function<Integer, Integer> createAdder(final int x) {

        // This is the closure:
        return new Function<Integer, Integer>() {
            public Integer f(Integer xx) {
                return x+xx;
            }
        };
    }


    public static void main(String args[]) {
        ClosureExample c = new ClosureExample();
        Function<Integer, Integer> ff = c.createAdder(3);
        for (int i = 0; i < 3; i++) {
            System.out.println("i=" + i + " s=" + ff.f(i));
        }

        System.out.println();
        ff = c.createAdder(90);
        for (int i = 0; i < 3; i++) {
            System.out.println("i=" + i + " s=" + ff.f(i));
        }
    }
}
