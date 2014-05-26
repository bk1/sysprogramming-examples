package net.itsky

import akka.actor.{ Actor, ActorLogging, Props, ActorRef }
import scala.concurrent.duration.FiniteDuration
import akka.actor.Cancellable

class Client(val favoriteNumber: BigInt,
	     val squarer: ActorRef) extends Actor with ActorLogging {

  import Client._
  import Squarer._
  
  import context.dispatcher

  var cancellable : Option[Cancellable] = None

  squarer ! OrderSquare(favoriteNumber)

  log.info("client created")
  override def receive: Receive = {
    case Result(y) => {
      val txt = "client with favoriteNumber=" + favoriteNumber + " received result=" + y
      println(txt)
      log.info(txt)
      val r = System.currentTimeMillis % 10000
      Thread.sleep(r)
      val x = favoriteNumber + r - 5000
      println("client with favoriteNumber=" + favoriteNumber + " sending x=" + x)
      squarer ! OrderSquare(x)
    }
  }
  
  override def postStop = {
    val txt = "client with favoriteNumber=" + favoriteNumber + ": Good-bye!"
    println(txt)
    log.info(txt)
  }

}

object Client {

  def props(favoriteNumber: BigInt, squarer: ActorRef) : Props = {
    Props(new Client(favoriteNumber, squarer))
  }
}
