organization := "net.itsky"

name := "systemprogramming"

version := "3.0.0"

scalaVersion := "2.11.0"

libraryDependencies ++= Dependencies.sysprog

scalacOptions ++= List(
  "-unchecked",
  "-deprecation",
  "-Xlint",
  "-language:_",
  "-target:jvm-1.6",
  "-encoding", "UTF-8"
)

retrieveManaged := true

parallelExecution in Test := false

transitiveClassifiers := List("sources")

initialCommands in console := "import net.itsky._,akka.actor._"
