package net.itsky

import akka.actor.{ Actor, ActorLogging, Props, ActorRef }
import scala.concurrent.duration.FiniteDuration
import akka.actor.Cancellable

class Holder(var currentNumber : BigInt) extends Actor with ActorLogging { 

  import Holder._
  import Squarer._
  
  import context.dispatcher

  var cancellable : Option[Cancellable] = None

  log.info("holder created")

  override def receive: Receive = {
    case _ => {
      log.warning("no messages understood")
    }
  }
  
  override def postStop = {
    val txt = "Good-bye!"
    println(txt)
    log.info(txt)
  }

}

object Holder {

  def props(currentNumber : BigInt) : Props = {
    Props(new Holder(currentNumber))
  }
}
