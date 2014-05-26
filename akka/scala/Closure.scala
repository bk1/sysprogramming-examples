//-*- coding: utf-8-unix -*-
/**
 * (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-05-20
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 *
 */
object Closure {
  def main(args : Array[String]) : Unit = {
    val x : Int = args(0).toInt
    val y : Int = args(1).toInt
    val c : Closure = new Closure()
    val f : ((Int) => Int) = c.adder(x);
    val arr = (1 to y).map(f)
    println(arr.toString)
  }
}

class Closure {
  def adder(x : Int) : ((Int) => Int) = {
    (y => x+y)
  }
}
