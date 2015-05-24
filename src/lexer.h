#ifndef LEXER_H
#define LEXER_H

#include "dfa.h"


class Lexer {
  public:
    typedef NFA::tokenId_t tokenId_t;
    typedef DFA::Token Token;

    Lexer(void) {
      classifier = new Classifier();
      nfa        = new NFA(classifier);
      dfa        = NULL;
    }

    void classifyWhiteSpace(void) {
      if(!dfa) classifier->classifyWhiteSpace(1);
     };

    void addToken(const char *regExp, tokenId_t aTokenId) {
      if (!dfa) nfa->addRegularExpressionForToken(regExp, aTokenId);
    }

    void compile(void) {
      if (!dfa) dfa = new DFA(nfa);
    }

    Token *getNextToken(Utf8Chars *someChars) {
      if (dfa) return dfa->getNextToken(someChars);
      return DFA::nullToken;
    }

  private:
    Classifier *classifier;
    NFA *nfa;
    DFA *dfa;

};

#endif

