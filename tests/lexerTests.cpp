#include <bandit/bandit.h>
using namespace bandit;

#include <string.h>
#include <stdio.h>
#include <exception>

#ifndef private
#define private public
#endif

#include <lexer.h>

enum LexerTestTokens {
  WhiteSpace,
  NonWhiteSpace
};

go_bandit([](){

  printf("\n----------------------------------\n");
  printf(  "lexer\n");
//  printf(  " NFAState = %zu bytes (%zu bits)\n", sizeof(NFA::State),  sizeof(NFA::State)*8);
  printf(  "----------------------------------\n");

  /// \brief We test the Lexer class.
  describe("Lexer", [](){

    it("Create a Lexer and tokenize if then else", [&](){
      Lexer *lexer = new Lexer();
      AssertThat(lexer, Is().Not().EqualTo((void*)0));
      AssertThat(lexer->dfa, Is().EqualTo((void*)0));
      lexer->classifyWhiteSpace();
      AssertThat(lexer->dfa, Is().EqualTo((void*)0));
      lexer->addToken("[whiteSpace]+", WhiteSpace);
      AssertThat(lexer->dfa, Is().EqualTo((void*)0));
      lexer->addToken("[!whiteSpace]+", NonWhiteSpace);
      AssertThat(lexer->dfa, Is().EqualTo((void*)0));
      lexer->compile();
      AssertThat(lexer->dfa, Is().Not().EqualTo((void*)0));
      Utf8Chars *someChars = new Utf8Chars(" if A then B else C ");
      ParseTrees::Token *aToken = lexer->parseFromUsing("start", someChars);
      AssertThat(aToken, Is().Not().EqualTo((void*)0));
      AssertThat(aToken->wrappedId, Is().EqualTo(0));
      AssertThat(someChars->getNextByte(), Is().EqualTo('i'));
      someChars->backup();
      aToken = lexer->parseFromUsing("start", someChars);
      AssertThat(aToken, Is().Not().EqualTo((void*)0));
      AssertThat(aToken->wrappedId, Is().EqualTo(1));
      AssertThat(someChars->getNextByte(), Is().EqualTo(' '));
      someChars->backup();
      aToken = lexer->parseFromUsing("start", someChars);
      AssertThat(aToken, Is().Not().EqualTo((void*)0));
      AssertThat(aToken->wrappedId, Is().EqualTo(0));
      AssertThat(someChars->getNextByte(), Is().EqualTo('A'));
      someChars->backup();
      aToken = lexer->parseFromUsing("start", someChars);
      AssertThat(aToken, Is().Not().EqualTo((void*)0));
      AssertThat(aToken->wrappedId, Is().EqualTo(1));
      AssertThat(someChars->getNextByte(), Is().EqualTo(' '));
      someChars->backup();
      aToken = lexer->parseFromUsing("start", someChars);
      AssertThat(aToken, Is().Not().EqualTo((void*)0));
      AssertThat(aToken->wrappedId, Is().EqualTo(0));
      AssertThat(someChars->getNextByte(), Is().EqualTo('t'));
      someChars->backup();
    });

  }); // describe Lexer

});
