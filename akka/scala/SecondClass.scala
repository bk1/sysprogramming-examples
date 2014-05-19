//  -*- coding: utf-8-unix -*-

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

