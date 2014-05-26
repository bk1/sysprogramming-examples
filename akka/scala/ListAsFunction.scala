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
import scala.collection.immutable.List

object ListAsFunction {
  def main(args : Array[String]) : Unit = {
    val f : List[Int] = List(0, 1, 4, 9, 16, 25, 36);
    println("f=" + f);
    val l : List[Int] = List(0, 2, 4, 6);
    println("l=" + l);
    val ll = l.map(f);
    println("l=" + l);
    println("ll=" + ll);
  }
}
