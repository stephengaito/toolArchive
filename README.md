# Dynamic UTF8 Parser

## Overview

This parser is based upon ideas taken from Russ Cox's [Implementing 
Regular Expressions](https://swtch.com/~rsc/regexp/) extended with a 
non-standard syntax to provide the ability to specify a [PushDown 
Automata](https://en.wikipedia.org/wiki/Pushdown_automaton) and hence a 
parser for any context-free language.

This non-standard (PushDown) Nondeterministic Automata (NFA) syntax is 
documented in the nfaBuilderParser.cpp and nfaBuilderFragments.cpp 
files. The nfaBuilderParser method implements a simple (PushDown) 
Nondeterminisitc Automata (NFA) parser for our syntax which drives the 
methods in the nfaBuilderFragments.cpp file to build a push down 
automata. Alternatively you can use these methods directly 
programaticlly to build a pushdown autoamata on the fly.

If you do not use the "{<restartStateName>}" syntax then you have a 
standard regular expression which should have linear behaviour.

Being data driven, both the parser is based upon Daniel C. Jones's 
[HAT-Trie](https://github.com/dcjones/hat-trie) implementation.  The 
parser uses a HAT-Trie to provide the UTF8 character classifier, as 
well a various internal mappings.

The parser is actually UTF8 naive, other than understanding the 
*structure* of UTF8 characters, all character classes, such as 
whitespace, special characters, etc, *must* be loaded into the parser's 
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

To build the Dynamic UTF8 Parser, git clone a copy of the project into 
a directory, for example "dynamicUTF8Parser"

    git clone https://github.com/stephengaito/dynamicUTF8Parser

then make a sister directory (*beside* the dynamicUTF8Parser sources) 
for the build, for example "dynamicUTF8ParserBUILD"

    mkdir dynamicUTF8ParserBUILD

then in the build directory use CMake to configure and make to build 
the project

    cd dynamicUTF8ParserBUILD
    cmake ../dynamicUTF8Parser

Once you have run the cmake command, on Linux, you have the following 
make targets:

* **make hattrie** builds the hattrie library

* **make src** builds the dynUtf8Parser library

* **make tests** build both libraries as well as all of the tests and 
  runs the dynUtf8ParserTests binary (i.e. *just* the dynUtf8Parser 
  tests).

* **make release** builds all of the above and then installs into the 
  CMAKE_INSTALL_PREFIX directories.

Note that compiling the hat-trie.c source (via any of the make targets) 
issues an -Wabsolute-value warning. This can be safely ignored.

To run **all** tests type:

    make tests
    make test

(The HAT-Trie test "bench_sorted_iter" *will* take a couple of minutes)

To only run the Dynamic UTF8 Parser tests type:

    make tests

Typically *I* build using LLVM's clang. I have also occasionaly built 
using gcc/g++.

## LICENSES

The core project uses the MIT license for all code unless otherwise 
noted. The MIT license can be found in the file LICENSE.

This project uses git subtree to include code taken from Daniel C. 
Jones's [HAT-Trie](https://github.com/dcjones/hat-trie) implementation. 
A local copy of HAT-Trie's license can be found in the file 
HAT-Trie/COPYING. Currently this code is used unchanged from commit 
[25f9e94659db94e1e5fa13dc83b82c6f5d801d1f](https://github.com/dcjones/hat-trie/commit/25f9e94659db94e1e5fa13dc83b82c6f5d801d1f) 
merged April 2 2015.
