//-*- coding: utf-8-unix -*-
/**
 * (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-05-20
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 *
 */
// import scala.collection.immutable.List
import scala.collection.mutable.Buffer
import scala.collection.immutable.List

object ListExample {
  def main(args : Array[String]) : Unit = {
    val ml : Buffer[Int] = Buffer(1, 2, 3)
    println("ml=" + ml)
    ml+=4
    println("ml=" + ml)
    ml+=5
    println("ml=" + ml)
    val l : List[Int] = List(1, 2, 3)
    println("l=" + l)
    // not possible:
    // l+=4
    // but this works:
    val l4 = 4 :: l
    println("l=" + l)
    println("l4=" + l4)
    val l5 = l ++ List(5)
    println("l=" + l)
    println("l5=" + l5)
    val ll = l ++ l
    println("l=" + l)
    println("ll=" + ll)
  }
}
