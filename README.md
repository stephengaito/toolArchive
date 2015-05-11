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

## LICENSES

The core project uses the MIT license for all code unless otherwise 
noted. The MIT license can be found in the file LICENSE.

This project uses git subtree to include code taken from Daniel C. 
Jones's [HAT-Trie](https://github.com/dcjones/hat-trie) implementation, 
its license can be found in the file HAT-Trie/COPYING. Currently this 
code is used unchanged from commit 
[25f9e94659db94e1e5fa13dc83b82c6f5d801d1f](https://github.com/dcjones/hat-trie/commit/25f9e94659db94e1e5fa13dc83b82c6f5d801d1f).
