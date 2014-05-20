object ClosureWithoutClass {
  def main(args : Array[String]) : Unit = {
    val x : Int = args(0).toInt
    val y : Int = args(1).toInt
    val f : ((Int) => Int) = adder(x);
    val arr = (1 to y).map(f)
    println(arr.toString)
  }

  def adder(x : Int) : ((Int) => Int) = {
    (y => x+y)
  }
}
