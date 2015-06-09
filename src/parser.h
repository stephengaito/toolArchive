#ifndef PARSER_H
#define PARSER_H

/** \mainpage Dynamic UTF8 Parser index page

This parser is based upon ideas taken from Russ Cox's [Implementing
Regular Expressions](https://swtch.com/~rsc/regexp/) to provide the
lexer and Bob Nystrom's [Pratt Parsers: Expression Parsing Made
Easy](http://journal.stuffwithstuff.com/2011/03/19/pratt-parsers-expression-pa$
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

#include "nfaBuilder.h"
#include "dfa/pushDownMachine.h"

using namespace DeterministicFiniteAutomaton;

/// \brief The Parser class brings together the Classifier/NFA/DFA classes
/// into the standard arrangement required for a simple Parser.
class Parser {
  public:

    /// \brief A TokenId is a user assigned NFA::TokenId (Hat-Trie::value_t).
    typedef Token::TokenId TokenId;

    /// \brief Create a Parser.
    Parser(void) {
      classifier = new Classifier();
      nfa        = new NFA(classifier);
      nfaBuilder = new NFABuilder(nfa);
      dfa        = NULL;
    }

    /// \brief Setup the Classifier to classify white space using the
    /// classSet_t 1.
    ///
    /// No classification is made if the Parser has already been compiled.
    void classifyWhiteSpace(void) {
      if(!dfa) classifier->classifyWhiteSpace(1);
     };

    /// \brief Add a Regular-Expression/TokenId to the Parser.
    ///
    /// No addition is made if the Parser has already been compiled.
    void addToken(const char *startStateName,
                  const char *regExp, TokenId aTokenId) {
      if (!dfa) {
        nfaBuilder->compileRegularExpressionForTokenId(startStateName,
                                                       regExp, aTokenId);
      }
    }

    /// \brief Compile the Regular-Expression/TokenId information.
    ///
    /// After a Parser has been compiled no further classifications can
    /// be made, or Regular-Expression/TokenIds can be added.
    void compile(void) {
      if (!dfa) {
        dfa = new DFA(nfa);
      }
    }

    /// \brief Get the next token while scanning the Utr8Chars provided.
    ///
    /// If the Parser has not yet been compiled, the null tokenId (-1)
    //  is returned.
    Token *parseFromUsing(const char *startStateName,
                          Utf8Chars *someChars,
                          PDMTracer *pdmTracer = NULL) {
      if (dfa) {
        PushDownMachine *pdm = new PushDownMachine(dfa);
        Token *result =
          pdm->runFromUsing(nfa->findStartStateId(startStateName),
                            someChars, pdmTracer);
        delete pdm;
        return result;
      }
      return NULL;
    }

  private:

    /// \brief The Classifier used to classify UTF8 characters.
    Classifier *classifier;

    /// \brief The NFA used to scan Utf8Chars streams.
    NFA *nfa;

    /// \brief The NFABuilder used to build the NFA from regular expressions.
    NFABuilder *nfaBuilder;

    /// \brief The DFA used to scan Utf8Chars streams.
    ///
    /// The DFA is compiled from the NFA by the compile method.
    DFA *dfa;
};

#endif

