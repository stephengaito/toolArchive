#include <bandit/bandit.h>
using namespace bandit;

#include <string.h>
#include <stdio.h>
#include <exception>

#ifndef private
#define private public
#endif

#include <lexer.h>

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
      lexer->classifyWhiteSpace();
      lexer->addToken("[whiteSpace]+", WhiteSpace);
      lexer->addToken("[!whiteSpace]+", NonWhiteSpace);
      lexer->compile();
      Utf8Chars *someChars = new Utf8Chars(" if A then B else C ");
      Lexer::Token *aToken = lexer->getNextToken(someChars);
      AssertThat(aToken, Is().Not().EqualTo((void*)0));
    });

  }); // describe Lexer

});
