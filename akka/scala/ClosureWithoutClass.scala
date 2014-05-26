//-*- coding: utf-8-unix -*-
/**
 * (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-05-20
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 *
 */
object ClosureWithoutClass {
  def main(args : Array[String]) : Unit = {
    val x : Int = args(0).toInt
    val y : Int = args(1).toInt
    val f : ((Int) => Int) = adder(x);
    val arr = (1 to y).map(f)
    println(arr.toString)
  }

  def adder(x : Int) : ((Int) => Int) = {
    (y => x+y)
  }
}
