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
