//-*- coding: utf-8-unix -*-
/**
 * (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-05-20
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 *
 */

object Factorial {
  def main(args : Array[String]) : Unit = {
    val n : BigInt = BigInt(args(0))
    val f : Factorial = new Factorial()
    val fOfN : BigInt = f.factorial(n)
    println("" + n + "!=" + fOfN);
  }
}

class Factorial {
  def factorial(n : BigInt) : BigInt = {
    if (n == 0) {
      BigInt(1)
    } else {
      n * factorial(n-1)
    }
  }

}
