//-*- coding: utf-8-unix -*-
/**
 * (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-05-20
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 *
 */
// import scala.collection.immutable.Map
import scala.collection.mutable.HashMap
import scala.collection.immutable.Map

object MapAsFunction {
  def main(args : Array[String]) : Unit = {
    val hm : HashMap[Int,Int] = HashMap(2 -> 4, 3 -> 9, 5 -> 25, 7 -> 49)

    val f : ((Int) => Int) = hm
    
    val list = List(2,3,7,5,2,3,5).map(f)
    println("hm=" + hm + " list=" + list)

    hm(11)=121
    hm(13)=169
    val list2 = List(2,3,7).map(f)
    println("hm=" + hm + " list2=" + list2)

    val m : Map[Int,Int] = Map(1 -> 3, 2->8, 4->24, 6->48, 10->120)
    println("m=" + m)
    // not possible:
    val m1214 = m +(12->168, 16->288)
    val g : ((Int) => Int) = m1214
    val list3 = List(1,2,6).map(g)
    println("m1214=" + m1214 + " list3=" + list3)
    println("g=" + g)
  }
}
