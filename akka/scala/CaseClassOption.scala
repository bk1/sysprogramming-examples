object CaseClassOption {
  def main(args : Array[String]) : Unit = {
    val cce : CaseClassOption = CaseClassOption(3, 4, 5)
    println("cce=" + cce)

    val x = f(CaseClassOption(3, 5, 7))
    val y = f(CaseClassOption(99, 55, 77))
    val z = f(CaseClassOption(3, 99, 1))
    println("x=" + x + " y=" + y + " z=" + z)
    val map = Map("x"->x, "y"->y, "z" ->z)
        
    map.foreach(itfun)
  }

    def itfun(t:(String, Option[CaseClassOption])) : Unit = {
      val u = t._1
      val v = t._2
      if (v.isEmpty) { 
        println(u + " is empty")
      } else { 
        println(u + " contains " + v.get)
      }
    }


  def f(cce : CaseClassOption) : Option[CaseClassOption] = {
    cce match {
      case CaseClassOption(3, _, _) => Some(cce)
      case CaseClassOption(_, _, _) => None
    }
  }
}

case class CaseClassOption(x : Int, y : Int, z : Int);
