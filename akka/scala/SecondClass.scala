//-*- coding: utf-8-unix -*-
/**
 * (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-05-20
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 *
 */

object SecondClass {
  def main(args : Array[String]) : Unit = {
    val n : Int = args(0).toInt
    val sc : SecondClass = new SecondClass(n);
    sc.printSquares()
  }
}

class SecondClass(val n:Int) {
  def printSquares() = {
    (0 to n-1).map(x=>{val xx = BigInt(x);println("x=" + xx + " x²=" + xx*xx); Unit})
  }
}

