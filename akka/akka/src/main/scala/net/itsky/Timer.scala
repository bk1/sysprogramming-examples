package net.itsky

/** measure the time consumption of an operation */
object Timer {
  
  def time(b: (Long=>Unit), x:Long): Long = {
    val t0 : Long = System.currentTimeMillis
    b(x)
    val t1 : Long = System.currentTimeMillis
    val dt = t1 - t0
    val s : String = dt.toString()
    System.out.println(s)
    dt
  }
   
  //time(busy, 10)
}
