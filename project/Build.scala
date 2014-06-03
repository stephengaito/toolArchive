/*****************************************************************************

Scala build definitions for the LaTeXMarkdown project
 
We add a testDocs (aka td) task which walks the testDocs directory and runs the
org.antlr.testRig code on each *.md, or *.tex file found.

Ideas for these definitions have been taken from:

- http://www.scala-sbt.org/0.13.5/docs/Detailed-Topics/Tasks.html#defining-a-task
- http://www.scala-sbt.org/0.13.5/docs/Detailed-Topics/Tasks.html#separating-implementations

- http://www.scala-sbt.org/0.13/tutorial/Full-Def.html#When+to+use++files

- http://stackoverflow.com/questions/19441400/working-with-yaml-for-scala

- http://rosettacode.org/wiki/Walk_a_directory/Recursively#Scala

- http://stackoverflow.com/questions/1131925/how-do-i-sort-an-array-in-scala/7167092#7167092

- http://blog.bruchez.name/2011/10/scala-partial-functions-without-phd.html

**/

/**

The TestDocs object's walk method walks the 'testDocs' directory looking for
appropriate testDocs to run using the ANTLR4 testRig.

In each TestDocs directory, the walk method loads the tests.yaml file (if it
exists) to define which testDocs should be tested (and which should be ignored).

**/

import java.io.File
import java.net._
import org.antlr.v4.runtime.misc.TestRig
import scala.collection.Seq
import sbt.Attributed
//import LaTeXMarkdownTestRig

object TestDocs {

  val testFiles = new PartialFunction[File, File] {
    def apply(file: File) : File = file
    def isDefinedAt(file: File) : Boolean = 
      if (file.getName.endsWith(".md"))
        if (file.getName.contains("eadme.md")) false
        else true
      else if (file.getName.endsWith(".tex")) true
      else if (file.isDirectory) true 
      else false
  }  
  
  def findTestFiles(file: File): Iterable[File] = {
    val children = new Iterable[File] {
      def iterator = 
        if (file.isDirectory) 
          file.listFiles.collect(testFiles).sortWith((a, b) => (a.compareTo(b) < 0)).iterator 
        else Iterator.empty
    }
    Seq(file) ++: children.flatMap(findTestFiles(_))
  }
  
  def walk(testFullClasspath : Seq[Attributed[File]]) : Unit = {
    var paths = testFullClasspath.files.map((file) => file.toURI.toURL)
    for (path <- paths) println(path)
//    println("-------------")
//    val oldClassLoader = Thread.currentThread().getContextClassLoader()
//    val newClassLoader = new URLClassLoader(paths.toArray, oldClassLoader )
//    Thread.currentThread().setContextClassLoader(newClassLoader)
//    var cl = Thread.currentThread().getContextClassLoader().asInstanceOf[URLClassLoader]
//    var urls = cl.getURLs().iterator
//    for(url <- urls) println(url.getPath)
    println("-------------")
    //var tr = new LaTeXMarkdownTestRig(cl)
    println("loaded")
    val dir = new File("testDocs")
    //for(f <- walkTree(dir)) println(f)
    for(f <- findTestFiles(dir)) 
      if (!f.isDirectory) {
        println("\n\ntesting: ["+f.getPath+"]")
        val args = Array("org.fandianpf.latexmarkdown.parser.LaTex", "document", f.getPath)
        val tr = new TestRig(args.toArray)
        tr.process 
      }
    println("-------------")
//    Thread.currentThread().setContextClassLoader(oldClassLoader)
//    cl = Thread.currentThread().getContextClassLoader().asInstanceOf[URLClassLoader]
//    urls = cl.getURLs().iterator
//    for(url <- urls) println(url.getPath) */
  }
}
