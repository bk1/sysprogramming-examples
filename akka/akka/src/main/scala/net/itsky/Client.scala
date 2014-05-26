package net.itsky

import akka.actor.{ Actor, ActorLogging, Props, ActorRef }
import scala.concurrent.duration.FiniteDuration
import akka.actor.Cancellable
import scala.concurrent.duration._

class Client(val favoriteNumber: BigInt,
	     val squarer: ActorRef) extends Actor with ActorLogging {

  import Client._
  import Squarer._
  
  import context.dispatcher

  var cancellable : Option[Cancellable] = None

  squarer ! OrderSquare(favoriteNumber)

  log.info("client created")
  override def receive: Receive = {
    case Result(y, msg) => {
      val txt = "client with favoriteNumber=" + favoriteNumber + " received result=" + y + " (msg=" + msg + ")"
      log.info(txt)

      val waitingTimeMesc = (System.currentTimeMillis * 997) % 10000
      val waitingTime : FiniteDuration = waitingTimeMesc.toInt.millis
      val txt2 = "client with favoriteNumber=" + favoriteNumber + " will sleep for " + waitingTime + " msec"
      log.info(txt2)      
      context.system.scheduler.scheduleOnce(waitingTime, self, NextCalculation)
    }
    case NextCalculation => {
      val correction = (System.currentTimeMillis * 851) % 100
      val x = favoriteNumber + correction - 50
      log.info("client with favoriteNumber=" + favoriteNumber + " sending x=" + x)
      squarer ! OrderSquare(x)
    }
    case x => {
      log.warning("received unexpected message=" + x);
    }
  }
  
  override def postStop = {
    val txt = "client with favoriteNumber=" + favoriteNumber + ": Good-bye!"
    log.info(txt)
  }

}

object Client {

  private case object NextCalculation;

  def props(favoriteNumber: BigInt, squarer: ActorRef) : Props = {
    Props(new Client(favoriteNumber, squarer))
  }
}
