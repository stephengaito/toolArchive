# LaTeXMarkdown Design

**A LaTeX + Markdown + TikZ parser for the web.**

The LaTeXMarkdown project provides a parser (initially) for both LaTeX and
Markdown. Our ultimate goal is to parse out an [Abstract Syntax
Tree](http://en.wikipedia.org/wiki/Abstract_syntax_tree) (AST) representation of
the LaTeXMarkdown document. Having the AST we can then write various tools to
manipulate this structure to either provide HTML/MathJax/MathML output, or input
to the diSimplexEngine.

[**LaTeX**](http://en.wikipedia.org/wiki/LaTeX) is (explicitly) a [context-free
language](http://en.wikipedia.org/wiki/Context-free_grammar), as such is it
"easy" to parse using most computational parsing tools.

[**Markdown**](http://en.wikipedia.org/wiki/Markdown) is (implicitly) a
[context-sensitive
language](http://en.wikipedia.org/wiki/Context-sensitive_grammar), as such it is
much harder to parse using most computational parsing tools. Typically, most
Markdown parsers, use multiple passes of regular expression matchers to "parse"
a Markdown document and translate it into (an explicilty context-free) HTML.

Markdown is primarily context sensitive on the line structure of a document.
LaTeX on the other hand more or less ignores this line structure.

The easiest way to parse Markdown is to break a document into a collection of
"Markdown blocks" and then to parse the context free part of the Markdown syntax
inside of each block.

Unfortunately, many LaTeX environments (\begin{xxx} ... \end{xxx}) will quite
often span multiple Markdown blocks. This makes the combined parsing of both
LaTeX and Markdown difficult.

Essentially we have two potential strategies:

* parse LaTeXMarkdown documents in one pass using one large context-sensitive
grammar, or

* parse LaTeXMarkdown documents in multiple passes using smaller grammars for
each of the LaTeX (contex-free) and Markdown (context-senstive) "parts".

If we parse LaTeXMarkdown in multiple passes we then have two subsiduary
questions:

* which do we parse first? 

* do we provide one AST which has both LaTeX and Markdown elements or do we
"just" care about the LaTeX elements?

Finally, a related question is: 

* do we allow the use of (semi-)arbitrary HTML in a LaTeXMarkdown document? 

Ultimately one AST which has both LaTeX and Markdown (and HTML?) elements is
probably the correct goal. However for the purposes of "expanding" LaTeX macros,
and providing input to the diSimplexEngine, an AST which only contains the LaTeX
elements is probably sufficient.

This suggests:

* we allow (semi-)arbitrary HTML to cover those aspects of HTML document markup
which are not easily captured by either (our) LaTeX parser or Markdown's syntax,

* we parse the harder context-sensitive Markdown syntax first into HTML using an
existing Markdown parser,

* we (re-)parse the resulting document using our LaTeX parser (which must be
tollerant of HTML structures).

One of the more serious drawbacks of this simplistic approach is that subtle
interactions between the LaTeX and Markdown syntaxes will be harder to identify.
This means that there will almost certainly be error prone edge cases for which
the authour will be presented with very little in the way of intelligible error
messages.  There will be dragons in the less explored regions of this syntax.

Given that we are pre-parsing the Markdown syntax directly into HTML, we choose
the *fastest* Markdown parser which covers enough of the Markdown syntax for our
needs.

## Potential problems

It is very important to notice that the Markdown block structures and the LaTeX
environment structures are almost always going to be out of sync with each
other. This means that it is important that the LaTeX parser either have a
unified AST with the Markdown parser, *or*, the LaTeX parser *must* be aware, at
the very least, of the HTML spans which are open at the beginning and ending of
any LaTeX struture (including a LaTeX command which *can* span many paragraphs).
In this way any open HTML spans can be automagically closed and re-opened around
the LaTeX structure.

## Ideas for future use

If we ever feel the need to produce a combined LaTeX and Markdown AST, then I
think that a dual parsing strategy is still possible. The first parse should be
to parse Markdown into its "block" structure, then to parse the LaTeX syntax.
However it is critical that the LaTeX parser use a "CharStream" which is based
upon the "stream" of Markdown blocks. This "Markdown based CharStream" would
then have the novel ability to merge blocks if the parsing of a given LaTeX
structure spans multiple Markdown blocks. This will require an existing Markdown
parser which parses into an extensible AST. It will also require some careful
programming of the Markdown based CharStream class.

# Tool choices

## Theoretical bias

Explicitly, FandianPF has the goal of scalling from a (google) tablet all the
way up into the cloud. There are a number of languages/platforms which span this
range of devices. Of these both
[Ruby](http://en.wikipedia.org/wiki/Ruby_%28programming_language%29) and
[Java](http://en.wikipedia.org/wiki/Java_%28software_platform%29) /
[Scala](http://en.wikipedia.org/wiki/Scala_%28programming_language%29) provide a
"nice" programming environment rich with continuations and the ability to create
(internal) [DSL](http://en.wikipedia.org/wiki/Domain-specific_language)s.

*One* of LaTeXMarkdown's goals is to provide a front-end input into the sister
project's [diSimplexEngine](https://github.com/diSimplex/diSimplexEngine).
diSimplexEngine is essentially a [proof
engine](http://en.wikipedia.org/wiki/Proof_assistant) whose proof objects are
Directed Simplicies rather than "textual" strings of a classical [mathematical
"language"](http://en.wikipedia.org/wiki/Theory_%28mathematical_logic%29). Since
[proof theory](http://en.wikipedia.org/wiki/Proof_theory) is intimately related
to [functional
programming](http://en.wikipedia.org/wiki/Functional_programming), most [proof
assistants](http://en.wikipedia.org/wiki/Proof_assistant) such as
[Agda](http://en.wikipedia.org/wiki/Agda_%28programming_language%29), or
[Coq](http://en.wikipedia.org/wiki/Coq), are implemented in functional languages
such as either
[Haskell](http://en.wikipedia.org/wiki/Haskell_%28programming_language%29) or
[OCaml](http://en.wikipedia.org/wiki/OCaml).

As will become obvious through the development of [Directed Simplicial Type
Theory](http://disimplex.github.io/) (DiSiTT), classical mathematical logic,
including classical function theory, has a deep one-dimensional bias. A deep
theory of [causality](http://en.wikipedia.org/wiki/Causality), for example any
theory [quantum](http://en.wikipedia.org/wiki/Quantum_mechanics)
[space-time](http://en.wikipedia.org/wiki/Spacetime), is profoundly
multi-dimensional. The functional programming language, Haskell, is a good
choice if the structures you are using are basically one-dimensional with a
cannonical structure, such as
[lists](http://en.wikipedia.org/wiki/List_%28abstract_data_type%29), or
[trees](http://en.wikipedia.org/wiki/Tree_%28data_structure%29).

DiSiTT, is explicitly multi-dimensional, and as such has no *cannonical*
structure. In particular, the essence of the diSimplexEngine, is to "walk" over
multiple Directed Simplicial Structures, while building or recognizing new
structures. Since these structures are manifestly *not* one-dimensional, a
classical functional programming language, such as Haskell, is not a good choice
in which to build the diSimplexEngine.

One of functional programming's *excellent* qualities, for the building of proof
engines, is its controlled approach to computational ["side
effects"](http://en.wikipedia.org/wiki/Side_effect_%28computer_science%29). One
of [Object Oriented
Programming](http://en.wikipedia.org/wiki/Object-oriented_programming)'s
*excellent* qualities, is the ability to build and "walk" complex
multidimensional structures. Both Scala and OCaml combine the functional as well
as the object oriented programming paradigms. Scala being based upon the Java
JVM *platform*, provides a natural choice for both the FandianPF and diSimplex
projects.

Ruby is Object oriented and can be programmed in a functional (side-effectless)
way. However a glance at the [Debian Compuer Language Benchmark
Games](http://benchmarksgame.alioth.debian.org/u64q/which-programs-are-fastest.php)
shows that *generally* Scala, Java and Haskell (both programming languages and
platforms) are an order of magnitude more performant than Ruby. Since the
diSimplexEngine will be bound by the need to swiftly manipulate lots of large
multi-dimensional structures, this suggests we use a hybrid Scala/Java approach
in the diSimplex project and hence for the LaTeXMarkdown project as well.

## Implementations

There are a number of [Markdown parser
implementations](http://www.w3.org/community/markdown/wiki/MarkdownImplementations)
in Java/Scala:

* [**Txtmark**](https://github.com/rjeschke/txtmark) seems to be largely hand
coded making extensive use of Java RegExps. It provides the greatest documented
speed by having the parsing and emitting, of HTML, being largely hand-coded.
Extension would be difficult and there is *no* accessible AST. It would *not* be
easy to change to emit LaTeX.

* [**Actuarius**](https://github.com/chenkelmann/actuarius) uses Scala's parser
combinators directly. This project would be easier to extend, but essentially
has no accessible AST.  It could with some work emit LaTeX.

* [**pegdown**](https://github.com/sirthias/pegdown) uses Mathias's sister
parboiled parser. This explicitly exposes an AST. This would be relatively easy
to extend and could either provide a combined Markdown/LaTeX AST or emit LaTeX
directly (for reparsing).

* [**Knockoff**](https://github.com/tristanjuricek/knockoff/) fairly clean and
simple design. This would be extensible and could be subclassed to emit LaTeX.
It does not have an accessible AST.

* [**MarkdownJ**](https://github.com/myabc/markdownj) not seriously considered.

* [**MarkdownPapers**](https://github.com/lruiz/MarkdownPapers) not seriously
considered.

* [**markdown4j** (google code)](https://code.google.com/p/markdown4j/) not
seriously considered.

* [**markdown4j** (github)](https://github.com/jdcasey/markdown4j) not seriously
considered.

Of some interest is [Txtmark's performance
comparision](https://github.com/rjeschke/txtmark#performance-comparison-of-markdown-processors-for-the-jvm).
This comparison suggests that *if* we are happy to transform Markdown to HTML
and then reparse, Txtmark would be the simplest and fastest tool to use. *If* we
were happy to transform Markdown to LaTeX and then reparse, Actuarius would
probably be the most performant and extensible choice. Finally, *if* we were to
attempt to build a Markdown based CharStream, we should use pegdown's AST.

Using Java/Scala there are a number of [parser
frameworks](http://en.wikipedia.org/wiki/Comparison_of_parser_generators) that I
have seriously considered:

* [**ANTLR**](http://www.antlr.org/) has been used previously to build a LaTeX
grammar for this project. This compiles the rules into Java code which uses a
combined Lexer/Top-down-parser approach to build an AST which is then walked to
manipulate the AST or to emit the final HTML/MathJax/MathML. ANTRL grammar
development suffers from some ambiguity effects which can make it hard to build
a working grammar, and certainly makes it non-trivial to extend an already
working grammar. However ANTRL4 and
[ANTRLWorks](http://tunnelvisionlabs.com/products/demo/antlrworks) makes
understanding these ambiguity effects considerably easier. We already have a
semi-working grammar for LaTeX and there is a fairly simple [BSD-open-source
HTML grammar](https://github.com/antlr/grammars-v4/tree/master/html).

* [**parboiled**](https://github.com/sirthias/parboiled) has been used to build
pegdown. This tool builds the rule structures in Java and then walks these
structures repeatedly to perform the parsing.

* [**Scala's parser
combinators**](https://github.com/scala/scala-parser-combinators) has been used
to build Actuarius. This builds Scala code which is executed to build a simple
AST/Emitter. While this has some similarities to Antlr's top-down processessing,
it is likely to be less performant. It is likely to be less prone to ambiguous
grammar effects, and hence easier to program/extend/maintain.

## Conclusion

I think that, at the moment, the best approach is to use Txtmark to parse the
Markdown into HTML and then combine the HTML and existing LaTeX ANTLR4 grammar
into a parser which can be used in either Java or Scala (or even JRuby).

# Other alternatives

One other goal of the LaTeXMarkdown project will be to provide an editor for
LaTeXMarkdown writting. One obvious way to do this would be to write
LaTeXMarkdown in JavaScript and run it in node.js environments. Node.js can
certainly be run in the cloud and most larger devices. It looks like it is
possible to run it in Android (and probably iOS) as well. Alas, JavaScript *is
not* my perfered language.
