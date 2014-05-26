// -*- coding: utf-8-unix -*-
package net.itsky

import akka.actor.{ Actor, ActorLogging, Props, ActorRef }

class Squarer() extends Actor with ActorLogging {
  import Squarer._

  var complaints = 0
  
  override def receive: Receive = {
    case OrderSquare(x: BigInt) => {
      val y = x*x
      println("squarer squaring: x=" + x + " -> y=" + y)
      sender ! Result(y)
    }
    case OrderCube(x: BigInt) => {
      val y = x*x*x
      println("squarer cubing: x=" + x + " -> y=" + y)
      sender ! Result(y)
    }
  }
}

object Squarer {
  case class OrderSquare(x: BigInt)

  case class OrderCube(x: BigInt)

  case class Result(y : BigInt)
  
  def props(): Props = {
    Props(new Squarer())

  }
}
