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
  WhiteSpace=1,
  NonWhiteSpace=2,
  Text=4
};

go_bandit([](){

  printf("\n----------------------------------\n");
  printf(  "Parser\n");
//  printf(  " Parser = %zu bytes (%zu bits)\n", sizeof(Parser),  sizeof(Parser)*8);
  printf(  "----------------------------------\n");

  /// \brief We test the Parser class.
  describe("Parser", [](){

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
      const char *cString ="  if A then B else C ";
      Utf8Chars *someChars = new Utf8Chars(cString);
      PDMTracer *pdmTracer =
        new PDMTracer("Parse and tokenize 'if then else'", stdout);
      pdmTracer->setCondition(PDMTracer::Progress | PDMTracer::PDMTokens | PDMTracer::PDMState);
      ParseTrees::Token *aToken =
        parser->parseFromUsing("start", someChars, pdmTracer);
      AssertThat(aToken, Is().Not().EqualTo((void*)0));
      ParseTrees::printTokenOn(aToken, stdout);
      AssertThat(aToken->wrappedId, Equals(8)); // tokenId:Text ignored:false
      AssertThat(aToken->numTokens, Equals(9));
      AssertThat(aToken->textStart, Equals(cString));
      AssertThat(aToken->textLength,
        Equals(someChars->getNumberOfBytesRead()));
    });

  }); // describe parser

});
