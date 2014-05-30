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

# Ideas for future use

If we ever feel the need to produce a combined LaTeX and Markdown AST, then I
think that a dual parsing strategy is still possible. The first parse should be
to parse Markdown into its "block" structure, then to parse the LaTeX syntax.
However it is critical that the LaTeX parser use a "CharStream" which is based
upon the "stream" of Markdown blocks. This "Markdown based CharStream" would
then have the novel ability to merge blocks if the parsing of a given LaTeX
structure spans multiple Markdown blocks. This will require an existing Markdown
parser which parses into an extensible AST. It will also require some careful
programming of the Markdown based CharStream class.


