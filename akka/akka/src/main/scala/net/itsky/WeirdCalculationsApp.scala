// -*- coding: utf-8-unix -*-

package net.itsky

import akka.actor.{ ActorSystem, Props, Actor, ActorLogging }
import akka.actor.ActorDSL._;
import scala.annotation.tailrec

object WeirdCalculationsApp extends App {

  implicit val system = ActorSystem("sysprog-system")
  
  val sqRef = system.actorOf(Squarer.props(), "squarer")

  commandLoop()

  @tailrec
  private def commandLoop(): Unit = {
    val command = readLine()
    println("command=" + command)
    command match {
      case "c" => { 
        val fn : BigInt = ((System.currentTimeMillis * 731) % 100+50)
        println("new client fn=" + fn)
        val client = system.actorOf(Client.props(fn, sqRef))
        // context.watch(client)
        commandLoop()
      }
      case "s" => {
        println("s:\n------------------------------------------------------------\nshutdown\n------------------------------------------------------------")
        system.shutdown()
      }        
    }
  }
}
