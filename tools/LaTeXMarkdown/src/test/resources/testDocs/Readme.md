# Test documents

This is a collection of test documents for the LaTeXMarkdown project.

This collection has 4 classes of documents:

* **1_easy** test documents which should be easy to parse. Each document tends
to focus on one aspect of the parser.

* **2_medium** test documents which should be moderately difficult to parse.
Each document will tend to concentrate on important edge cases.

* **3_wild** test documents which come from real world documentation. Each
document should parse correctly but one or two hard but unusual edge cases might
be altered to ensure they can be parsed. This directory will contain a
discussion of the expected way to "solve" these unusal edge cases
("word-arounds").

* **4_hard** test documents which come from the actual documentation of either
Markdown or LaTeX. These documents cover the whole of the Markdown or LaTeX
syntax, but might not ever parse. This directory will contain a discussion of
what aspects of the whole syntax does not currently (or will never) parse
correctly.

Each of these classes of documents has three different sub-classes
(sub-directories):

* **Markdown** documents which only contain valid markdown syntax.

* **LaTeX** documents which only contain valid LaTeX syntax.

* **Mixed** documents which contain mixtures of Markdown and LaTex.

# Sources

Many of these test documents are original to this project. However some come
from other projects, or other people's work (for example from
[ArXiv](http://www.arxiv.org)). The origin and copyright licences for these
documents are noted in a `Readme.md` or `License` file in each sub-collection
when the documents are not original to this project.

# Goals

The LaTeXMarkdown project has two primary goals:

* for the **FandianPF project**, to provide a LaTeX->HTML/MathJax translator
which is able to perform (simple) "macro expansion" of a range of text meant for
the web which contains significant amounts of LaTeX.

* for the **DiSimplex project**, to provide a LaTeX->DiSiTT translator to
extract the DiSiTT based proof structures for evaluation. The texts in this case
will usually, though not exclusively, be complete LaTeX documents with
significant amounts of DiSiTT based proofs to check.

## Current most important goals

Our current most important goals are to provide a LaTeX parser for complete
LaTeX documents with significant amounts of DiSiTT based proofs to check. In
particular LaTeX documents with sizable DiSiTT proofs in them may make
significant use of the LaTeX \input command, so we need to produce tests which
make use of \input command.

