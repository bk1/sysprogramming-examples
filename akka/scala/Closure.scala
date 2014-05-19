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
