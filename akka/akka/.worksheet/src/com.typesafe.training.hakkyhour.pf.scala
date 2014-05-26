package com.typesafe.training.hakkyhour

object pf {;import org.scalaide.worksheet.runtime.library.WorksheetSupport._; def main(args: Array[String])=$execute{;$skip(96); 
  println("Welcome to the Scala worksheet");$skip(102); 
  
  val pf : PartialFunction[Any, Int] = {
  	case s: String => s.length;
  	case x: Int => x*x;
  };System.out.println("""pf  : PartialFunction[Any,Int] = """ + $show(pf ));$skip(10); val res$0 = 

  pf(10);System.out.println("""res0: Int = """ + $show(res$0));$skip(12); val res$1 = 
 	pf("abc");System.out.println("""res1: Int = """ + $show(res$1));$skip(21); val res$2 = 
 	pf.isDefinedAt(10);System.out.println("""res2: Boolean = """ + $show(res$2))}
 	
}
