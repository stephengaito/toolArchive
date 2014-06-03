
// To use sbt with Eclipse see: https://github.com/typesafehub/sbteclipse

// set the name of the project
name := "LaTeXMarkdown"

version := "0.1"

organization := "org.fandianpf"

// reduce the maximum number of errors shown by the Scala compiler
maxErrors := 20

// append several options to the list of options passed to the Java compiler
//javacOptions ++= Seq("-source", "1.5", "-target", "1.5")

// append -deprecation to the options passed to the Scala compiler
//scalacOptions += "-deprecation"


///////////////////////////////////////////////////////////////////////////
// Dependencies
//
libraryDependencies += "org.yaml" % "snakeyaml" % "1.13"

// Required for the project/Build.scala (TestDocs)
// The following MUST be manuall placed into project/lib directory
//libraryDependencies += "org.antlr" % "antlr4-runtime" % "4.2.2"

///////////////////////////////////////////////////////////////////////////
// Tasks
lazy val testDocs = taskKey[Unit]("Runs ANTLR4's testRig against approriate files in the testDocs directory")

//testDocs := TestDocs.walk("testDocs")
testDocs in Test := TestDocs.walk((fullClasspath in Test).value)

///////////////////////////////////////////////////////////////////////////
//
// SBT-ANTLR4 plugin
//
// see: https://github.com/ihji/sbt-antlr4
antlr4Settings

antlr4PackageName in Antlr4 := Some("org.fandianpf.latexmarkdown.parser")

antlr4GenListener in Antlr4 := true // default: true

antlr4GenVisitor in Antlr4 := true  // default: false

////////////////////////////////////////////////////////////////////////////
//
// Publishing
//
// disable using the Scala version in output paths and artifacts
// see: http://stackoverflow.com/questions/8288859/how-do-you-remove-the-scala-version-postfix-from-artifacts-builtpublished-wi/8291387#8291387
crossPaths := false

// Use an IVY style repository
publishMavenStyle := false

publishTo := Some(Resolver.file("file",  new File( "ivyRepo" )) )

