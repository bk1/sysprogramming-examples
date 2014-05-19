//  -*- coding: utf-8-unix -*-
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

