import scala.annotation.tailrec

object FactorialTailRecInner {
  def main(args : Array[String]) : Unit = {
    val n : BigInt = BigInt(args(0))
    val f : FactorialTailRec = new FactorialTailRec()
    val fOfN : BigInt = f.factorial(n)
    println("" + n + "!=" + fOfN);
  }
}

class FactorialTailRecInner {
  def factorial(n : BigInt) : BigInt = {
    
    @tailrec
    def factorial2(n : BigInt, partial : BigInt) : BigInt = {
      if (n <= 1) {
        partial
      } else {
        factorial2(n - 1, partial * n)
      }
    }

    factorial2(n, 1)
  }
}
