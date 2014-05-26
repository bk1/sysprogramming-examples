//-*- coding: utf-8-unix -*-
/**
 * (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-05-20
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 *
 */
object FirstClass {
  def main(args: Array[String]) = {
    val fc: FirstClass = new FirstClass("Hello, World\n");
    fc.hello();
  }
}

class FirstClass(val msg: String) {
  def hello() = {
    println(msg)
  }
}

