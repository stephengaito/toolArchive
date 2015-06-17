# Examples

In this directory we sketch the parser for our two primary examples:

* **[Markdown]{markdown)** is a simple (subset) of the 
[Markdown](http://daringfireball.net/projects/markdown/syntax). It 
should provide a "gentle" introduction to the use of the dynUtf8Parser. 
Note that as this is just an example, it is in no sense complete.

* **[LaTeXMarkdow](LaTeXMarkDown)** is a simple parser which converts a 
mixture of Markdown, LaTeX and Tikz into HTML for use writing Science 
and Mathematics on the web.

* **[diSimplexEngine](diSimplexEngine)** is a parser for a new 
meta-language for Mathematics.

# Dependencies

All examples in this collection build against the released/installed 
versions of both HAT-trie and dynamicUtf8Parser libraries.

# Building

Each example in this collection is a completely separate build environment.

To build any of the examples, git clone a copy of the *whole* project 
into a directory, for example "dynamicUTF8Parser"

    git clone https://github.com/stephengaito/dynamicUTF8Parser

then follow the instructions for make releasing the dynamicUtf8Parser 
library. Once these have been released/installed into your local 
system, make a sister directory (*beside* the dynamicUTF8Parser 
sources) to build the examples, for example "dynamicUTF8ParserMarkdown"

    mkdir dynamicUTF8ParserMarkdown

in the build directory use CMake to configure and make to build 
the project

    cd dynamicUTF8ParserMarkdown
    cmake ../dynamicUTF8Parser/examples/markdown

Once you have run the cmake command, on Linux, you have the following 
make targets:

* **make src** builds the dynUtf8Parser library

* **make tests** build both libraries as well as all of the tests and 
  runs the dynUtf8ParserTests binary (i.e. *just* the dynUtf8Parser 
  tests).

* **make release** builds all of the above and then installs into the 
  CMAKE_INSTALL_PREFIX directories.

Typically I build using LLVM's clang. I have also occasionaly built 
using gcc/g++.

