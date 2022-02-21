# LaTeXMarkdown

**A LaTeX + Markdown + TikZ parser for the web.**

If you want to write scientific documents with any amount of mathematical
content, then at some point either [TeX](http://en.wikipedia.org/wiki/TeX) or
[LaTeX](http://en.wikipedia.org/wiki/LaTeX) will be your friend.

Mathematicians have complex concepts to discuss in precise ways, this generally
requires the creation of large numbers of symbols, "words" and or "names", for
these concepts. A well used symbol can replace reams of words but often requires
a precise physical arrangement between its sub-symbols. This is *one* of the
tasks for which TeX was designed by [Donald
Knuth](http://en.wikipedia.org/wiki/Donald_Knuth), on behalf of the
[AMS](http://www.ams.org).

While there are numerous "hacks" to provide mathematical content on the web,
none are as comfortable to write in or to read from as a LaTeX document typeset
into PDF. Up until about 5 years ago most approaches to typesetting a LaTeX
document for the web, consisted of various tools which extracted the
mathematical symbols into a very large number of small graphic images which were
then embeded into a corresponding HTML document. Given that many mathematical
documents use multiple symbols in each sentence, this is both painful to process
and read.

The AMS has recently sponsored the [MathJax](http://www.mathjax.org/) project to
provide "an open source JavaScript display engine for mathematics that works in
all browsers". MathJax targets the "math mode" parts of a TeX/LaTeX document and
uses various tools both inside a given browser and or remotely to provide very
satisfying display of mathematical symbols.

One of the most subtle but important abilities of TeX/LaTeX is the ability to
easily define macro commands which, when "expanded" by the TeX/LaTeX program,
provide detailed descriptions of how a symbol should be typeset. Careful
creation of domain specific collections of macros by an authour, can make a
"raw" TeX/LaTeX document reasonalby readable by anyone comfortable with
TeX/LaTeX (such as mathematical authours).

The other important ability of TeX/LaTex is to provide tools with which to
provide a "global" structure to a mathematical document. In particular, to
provide indexes of both concepts and symbols, to help a reader to navigate
through complex mathematical arguments.

Unfortunately MathJax does not really address the problem, for an authour, of
maintaining, nor, for a reader, of navigating through a large collection of
macros/concepts.

**One of LaTeXMarkdown's primary goals** is to enable both the maintenance and
*cataloguing* of large collections of TeX/LaTeX macros across collections of
related mathematical documents on the web. This should make it easier for a
group of authours to write coherent collections of mathematical discussion using
TeX/LaTeX macros. More importantly, it should help readers by enabling the
indexing of the concepts relevant to a body of mathematical work.

The web and PDF documents, provide very different but complimentary tools in
which to read mathematical arguments.

A PDF document, created using for example pdflatex, provides a "contained"
"enviroment" in which to read a "complete" mathematical argument. Such arguments
can be complex but it is critical that all of the ideas/assumptions required to
make a given mathematical argument to be precise, are provided in *one* place.
TeX/LaTeX's document structuring commands provide extreamly powerful tools to
maintain such "large" coherient discussions in one or more documents.

On the other hand, the web is better suited to navigating through large
collections of inter-related but smaller discussions. For such small
"documents", various "wiki markup languages", such as
[Markdown](http://en.wikipedia.org/wiki/Markdown), have been created to help
make the structure of such small documents readable for the authour.

**LaTeXMarkdown is designed to allow:**

* large collections of small mathematical documents to be written and indexed,
and

* (more traditional) large mathematical documents to be "exploded" onto the web
to enable community discussion of the contents.

* mathematical proofs to be automatically checked by the
[diSimplexEngine](https://github.com/diSimplex/diSimplexEngine).

# IVY repository

An IVY repository for development snapshots of this work can be found 
at:

> https://github.com/stephengaito/LaTeXMarkdown/tree/master/ivyRepo/

Released versions (when they are released) can be found at:

> https://github.com/fandianpf/LaTeXMarkdown/tree/master/ivyRepo/

