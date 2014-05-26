// -*- coding: utf-8-unix -*-
package net.itsky

import akka.actor.{ Actor, ActorLogging, Props, ActorRef }

class Squarer() extends Actor with ActorLogging {
  import Squarer._

  var complaints = 0
  
  override def receive: Receive = {
    case OrderSquare(x: BigInt) => {
      val y = x*x
      val msg = "x=" + x + " y=x^2=" + y;
      log.info("squarer squaring: " + msg)
      sender ! Result(y, msg)
    }
    case OrderCube(x: BigInt) => {
      val y = x*x*x
      val msg = "x=" + x + " y=x^3=" + y;
      log.info("squarer cubing: " + msg)
      sender ! Result(y, msg)
    }
  }
}

object Squarer {
  case class OrderSquare(x: BigInt)

  case class OrderCube(x: BigInt)

  case class Result(y : BigInt, s : String)
  
  def props(): Props = {
    Props(new Squarer())

  }
}
