import scala.annotation.tailrec

object FactorialTailRecWeakPrivate {
  def main(args : Array[String]) : Unit = {
    val n : BigInt = BigInt(args(0))
    val f : FactorialTailRecWeakPrivate = new FactorialTailRecWeakPrivate()
    val fOfN : BigInt = f.factorial(n)
    println("" + n + "!=" + fOfN);
  }
}

class FactorialTailRecWeakPrivate {
  def factorial(n : BigInt) : BigInt = {
    factorial2(n, 1)
  }
  
  @tailrec
  private def factorial2(n : BigInt, partial : BigInt) : BigInt = {
    if (n <= 1) {
      partial
    } else {
      factorial2(n - 1, partial * n)
    }
  }
}
