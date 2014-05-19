// import scala.collection.immutable.List
import scala.collection.mutable.Buffer
import scala.collection.immutable.List

object ListExample {
  def main(args : Array[String]) : Unit = {
    val ml : Buffer[Int] = Buffer(1, 2, 3)
    println("ml=" + ml)
    ml+=4
    println("ml=" + ml)
    ml+=5
    println("ml=" + ml)
    val l : List[Int] = List(1, 2, 3)
    println("l=" + l)
    // not possible:
    // l+=4
    // but this works:
    val l4 = l + 4
    println("l=" + l)
    println("l4=" + l4)

  }
}
