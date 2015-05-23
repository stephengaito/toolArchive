#ifndef PARSER_H
#define PARSER_H

/** \mainpage Dynamic UTF8 Parser index page

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

We make use of Joakim Karlsson's [Bandit C++ testing
framework](https://github.com/joakimkarlsson/bandit).

*/

#endif
