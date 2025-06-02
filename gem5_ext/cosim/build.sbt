ThisBuild / scalaVersion := "2.12.11"
ThisBuild / crossScalaVersions := Seq("2.12.4", "2.11.12")
ThisBuild / scalacOptions ++= Seq("-deprecation","-unchecked","-Xsource:2.11")
ThisBuild / libraryDependencies ++= Seq(
  "org.scalatest" %% "scalatest" % "3.0.1" % Test,
  "com.typesafe.play" %% "play-json" % "2.6.2",
  "edu.berkeley.cs" %% "chisel3" % "3.1.0"
)

lazy val mini = (project in file("src/chisel_design/riscv-mini"))
lazy val example = (project in file("src/chisel_design/example"))
  .dependsOn(mini % "compile->compile;compile->test")
lazy val root = (project in file("."))
  .dependsOn(mini % "compile->compile;compile->test", example)
