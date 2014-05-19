object Factorial {
  def main(args : Array[String]) : Unit = {
    val n : BigInt = BigInt(args(0))
    val f : Factorial = new Factorial()
    val fOfN : BigInt = f.factorial(n)
    println("" + n + "!=" + fOfN);
  }
}

class Factorial {
  def factorial(n : BigInt) : BigInt = {
    if (n == 0) {
      BigInt(1)
    } else {
      n * factorial(n-1)
    }
  }

}
