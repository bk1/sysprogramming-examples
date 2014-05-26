addSbtPlugin("com.typesafe.sbt" % "sbt-atmos" % "0.2.3")

addSbtPlugin("com.typesafe.sbt" % "sbt-scalariform" % "1.1.0")

// Actually IDE specific settings belong into ~/.sbt/,
// but in order to ease the setup for the training we put the following here:

addSbtPlugin("com.github.mpeltonen" % "sbt-idea" % "1.5.1")

addSbtPlugin("com.typesafe.sbteclipse" % "sbteclipse-plugin" % "2.3.0")
