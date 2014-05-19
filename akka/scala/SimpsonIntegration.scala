object SimpsonIntegration {
  def main(args : Array[String]) : Unit = {
    val a = args(0).toDouble
    val b = args(1).toDouble
    val c = args(2).toDouble
    val n = args(3).toInt
    val A = args(4).toDouble
    val B = args(5).toDouble
    val s : SimpsonIntegration = new SimpsonIntegration()
    val f : (Double => Double) = ( (x : Double) => (a*x*x + b*x + c) : Double)
    val i : Double = s.integrate(f, A, B, n)  
    println("\\int_{A=" + A + "}^{B=" + B + "} (" + a + "x^2 + " + b + "x + " + c + ") dx = " + i)
  }

}

class SimpsonIntegration {
  def integrate(f : ((Double) => Double), A : Double, B : Double, n : Int) = {
    val len : Double = B - A
    val step : Double = len / (2*n)
    val step3 : Double = step/3
    var sum : Double = 0
    (0 to (n-1)).foreach( i => { 
      val x0 = A+2*i*step;
      val x1 = x0+step;
      val x2 = x1+step;
      // println("x0="+ x0+ " x1=" + x1+ " x2=" + x2);
      val y0 = f(x0);
      val y1 = f(x1);
      val y2 = f(x2);
      // println("y0="+ y0+ " y1=" + y1+ " y2=" + y2);
      sum += step3*(y0 + 4*y1 + y2) })
    sum
  }
}
