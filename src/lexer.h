#ifndef LEXER_H
#define LEXER_H

#include "dfa.h"

/// \brief The Lexer class brings together the Classifier/NFA/DFA classes
/// into the standard arrangement required for a simple Lexer.
class Lexer {
  public:

    /// \brief A TokenId is a user assigned NFA::TokenId (Hat-Trie::value_t).
    typedef NFA::TokenId TokenId;

    /// \brief Create a Lexer.
    Lexer(void) {
      classifier = new Classifier();
      nfa        = new NFA(classifier);
      dfa        = NULL;
    }

    /// \brief Setup the Classifier to classify white space using the
    /// classSet_t 1.
    ///
    /// No classification is made if the Lexer has already been compiled.
    void classifyWhiteSpace(void) {
      if(!dfa) classifier->classifyWhiteSpace(1);
     };

    /// \brief Add a Regular-Expression/TokenId to the Lexer.
    ///
    /// No addition is made if the Lexer has already been compiled.
    void addToken(const char *regExp, TokenId aTokenId) {
      if (!dfa) nfa->addRegularExpressionForTokenId(regExp, aTokenId);
    }

    /// \brief Compile the Regular-Expression/TokenId information.
    ///
    /// After a Lexer has been compiled no further classifications can
    /// be made, or Regular-Expression/TokenIds can be added.
    void compile(void) {
      if (!dfa) dfa = new DFA(nfa);
    }

    /// \brief Get the next token while scanning the Utr8Chars provided.
    ///
    /// If the Lexer has not yet been compiled, the null tokenId (-1)
    //  is returned.
    TokenId getNextTokenId(Utf8Chars *someChars) {
      if (dfa) return dfa->getNextTokenId(someChars);
      return -1;
    }

  private:

    /// \brief The Classifier used to classify UTF8 characters.
    Classifier *classifier;

    /// \brief The NFA used to scan Utf8Chars streams.
    NFA *nfa;

    /// \brief The DFA used to scan Utf8Chars streams.
    ///
    /// The DFA is compiled from the NFA by the compile method.
    DFA *dfa;

};

#endif

