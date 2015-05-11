# Dynamic UTF8 Parser

## Overview

This parser is based upon ideas taken from Russ Cox's [Implementing 
Regular Expressions](https://swtch.com/~rsc/regexp/) to provide the 
lexer and Bob Nystrom's [Pratt Parsers: Expression Parsing Made 
Easy](http://journal.stuffwithstuff.com/2011/03/19/pratt-parsers-expression-parsing-made-easy/) 
for the parser.

Being data driven, both the lexer and the parser are based upon Daniel 
C. Jones's [HAT-Trie](https://github.com/dcjones/hat-trie) 
implementation.  The lexer uses a HAT-Trie to provide the UTF8 
character classifier, and the parser uses a HAT-Trie to provide the 
symbol table.

The lexer is actually UTF8 naive, other than understanding the 
*structure* of UTF8 characters, all character classes, such as 
whitespace, special characters, etc, *must* be loaded into the lexer's 
character classifier. (Semi-)standard classes are provided in a form 
that can be loaded if desired.

## Dependencies

All tests (except those provided by the HAT-Trie library) make use of 
Joakim Karlsson's [bandit C++ BDD testing 
framework](https://github.com/joakimkarlsson/bandit). Bandit is a 
header only collection of test tools. This means that the bandit files 
must be installed and in your build's include path.

## Building

The Dynamic UTF8 Parser can make use of LLVM's link time optimiztion. 
To do this we use the CMake build system.

To build this the Dynamic UTF8 Parser, git clone a copy of the project 
into a directory, for example "dynamicUTF8Parser"

    git clone https://github.com/stephengaito/dynamicUTF8Parser

then make a sister directory (*beside* the dynamicUTF8Parser sources) 
for the build, for example "dynamicUTF8ParserBUILD"

    mkdir dynamicUTF8ParserBUILD

then in the build directory use CMake to configure and make to build 
the project

    cd dynamicUTF8ParserBUILD
    cmake ../dynamicUTF8Parser
    make

Note that compiling the hat-trie.c source issues an -Wabsolute-value 
warning. This can be safely ignored.

To run all tests type:

    ctest

(The HAT-Trie test "bench_sorted_iter" *will* take a couple of minutes)

To only run the Dynamic UTF8 Parser tests type:

    ./dynUtf8ParserTests

## LICENSES

The core project uses the MIT license for all code unless otherwise 
noted. The MIT license can be found in the file LICENSE.

This project uses git subtree to include code taken from Daniel C. 
Jones's [HAT-Trie](https://github.com/dcjones/hat-trie) implementation. 
A local copy of HAT-Trie's license can be found in the file 
HAT-Trie/COPYING. Currently this code is used unchanged from commit 
[25f9e94659db94e1e5fa13dc83b82c6f5d801d1f](https://github.com/dcjones/hat-trie/commit/25f9e94659db94e1e5fa13dc83b82c6f5d801d1f) 
merged April 2 2015.
