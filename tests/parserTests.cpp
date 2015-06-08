#include <bandit/bandit.h>
using namespace bandit;

#include <string.h>
#include <stdio.h>
#include <exception>

#ifndef private
#define private public
#endif

#include <parser.h>

enum ParserTestTokens {
  WhiteSpace,
  NonWhiteSpace,
  Text
};

go_bandit([](){

  printf("\n----------------------------------\n");
  printf(  "Parser\n");
//  printf(  " Parser = %zu bytes (%zu bits)\n", sizeof(Parser),  sizeof(Parser)*8);
  printf(  "----------------------------------\n");

  /// \brief We test the Parser class.
  describe("Parser", [](){

#ifdef NOT_DEFINED

    it("Create a Parser and tokenize 'if A then B else C'", [&](){
      Parser *parser = new Parser();
      AssertThat(parser, Is().Not().EqualTo((void*)0));
      AssertThat(parser->dfa, Is().EqualTo((void*)0));
      parser->classifyWhiteSpace();
      AssertThat(parser->dfa, Is().EqualTo((void*)0));
      parser->addToken("whiteSpace", "[whiteSpace]+", WhiteSpace);
      AssertThat(parser->dfa, Is().EqualTo((void*)0));
      parser->addToken("nonWhiteSpace", "[!whiteSpace]+", NonWhiteSpace);
      AssertThat(parser->dfa, Is().EqualTo((void*)0));
      parser->addToken("start", "({whiteSpace}|{nonWhiteSpace})*", Text);
      AssertThat(parser->dfa, Is().EqualTo((void*)0));
      parser->compile();
      AssertThat(parser->dfa, Is().Not().EqualTo((void*)0));
      Utf8Chars *someChars = new Utf8Chars("  if A then B else C ");
      PDMTracer *pdmTracer =
        new PDMTracer("Parse and tokenize 'if then else'", stdout);
      ParseTrees::Token *aToken =
        parser->parseFromUsing("start", someChars, pdmTracer);
      AssertThat(aToken, Is().Not().EqualTo((void*)0));
      AssertThat(aToken->wrappedId, Is().EqualTo(0));
      AssertThat(someChars->getNextByte(), Is().EqualTo('i'));
      someChars->backup();
      aToken = parser->parseFromUsing("start", someChars);
      AssertThat(aToken, Is().Not().EqualTo((void*)0));
      AssertThat(aToken->wrappedId, Is().EqualTo(1));
      AssertThat(someChars->getNextByte(), Is().EqualTo(' '));
      someChars->backup();
      aToken = parser->parseFromUsing("start", someChars);
      AssertThat(aToken, Is().Not().EqualTo((void*)0));
      AssertThat(aToken->wrappedId, Is().EqualTo(0));
      AssertThat(someChars->getNextByte(), Is().EqualTo('A'));
      someChars->backup();
      aToken = parser->parseFromUsing("start", someChars);
      AssertThat(aToken, Is().Not().EqualTo((void*)0));
      AssertThat(aToken->wrappedId, Is().EqualTo(1));
      AssertThat(someChars->getNextByte(), Is().EqualTo(' '));
      someChars->backup();
      aToken = parser->parseFromUsing("start", someChars);
      AssertThat(aToken, Is().Not().EqualTo((void*)0));
      AssertThat(aToken->wrappedId, Is().EqualTo(0));
      AssertThat(someChars->getNextByte(), Is().EqualTo('t'));
      someChars->backup();
    });

#endif

  }); // describe parser

});
