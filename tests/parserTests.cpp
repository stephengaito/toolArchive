#include <bandit/bandit.h>
using namespace bandit;

#include <string.h>
#include <stdio.h>
#include <exception>
#include <execinfo.h>

#ifndef protected
#define protected public
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
  printf(  " Parser = %zu bytes (%zu bits)\n", sizeof(Parser),  sizeof(Parser)*8);
  printf(  "----------------------------------\n");

  /// \brief We test the Parser class.
  describe("Parser", [](){

    it("Create a Parser and tokenize 'if A then B else C'", [&](){
      Parser *parser = new Parser();
      AssertThat(parser, Is().Not().EqualTo((void*)0));
      AssertThat(parser->dfa, Is().EqualTo((void*)0));
      parser->classifyWhiteSpace();
      AssertThat(parser->dfa, Is().EqualTo((void*)0));
      parser->addRule("whiteSpace", "[whiteSpace]+", WhiteSpace);
      AssertThat(parser->dfa, Is().EqualTo((void*)0));
      parser->addRule("nonWhiteSpace", "[!whiteSpace]+", NonWhiteSpace);
      AssertThat(parser->dfa, Is().EqualTo((void*)0));
      parser->addRule("start", "({whiteSpace}|{nonWhiteSpace})*", Text);
      AssertThat(parser->dfa, Is().EqualTo((void*)0));
      parser->compile();
      AssertThat(parser->dfa, Is().Not().EqualTo((void*)0));
      const char *cString ="  if A then B else C ";
      Utf8Chars *someChars = new Utf8Chars(cString);
      PDMTracer *pdmTracer =
        new PDMTracer("Parse and tokenize 'if then else'", stdout);
      pdmTracer->setCondition(PDMTracer::Progress |
                              PDMTracer::PDMStack |
                              PDMTracer::PDMTokens |
                              PDMTracer::PDMState);
//      Token *aToken = parser->parseFromUsing("start", someChars, pdmTracer);
      Token *aToken = parser->parseFromUsing("start", someChars, NULL);
      AssertThat(aToken, Is().Not().EqualTo((void*)0));
//      aToken->printOn(stdout);
      AssertThat(aToken->tokenId, Equals(4));
      AssertThat(aToken->tokens.getNumItems(), Equals(13));
      AssertThat(aToken->textStart, Equals(cString));
      AssertThat(aToken->textLength,
        Equals(someChars->getNumberOfBytesToRead()));
      AssertThat(aToken->tokens.itemArray[0].tokenId, Equals(1));
      AssertThat(aToken->tokens.itemArray[0].textStart[0], Equals(' '));
      AssertThat(aToken->tokens.itemArray[0].tokens.numItems, Equals(0));
      AssertThat(aToken->tokens.itemArray[1].tokenId, Equals(2));
      AssertThat(aToken->tokens.itemArray[1].textStart[0], Equals('i'));
      AssertThat(aToken->tokens.itemArray[1].tokens.numItems, Equals(0));
      AssertThat(aToken->tokens.itemArray[2].tokenId, Equals(1));
      AssertThat(aToken->tokens.itemArray[2].textStart[0], Equals(' '));
      AssertThat(aToken->tokens.itemArray[2].tokens.numItems, Equals(0));
      AssertThat(aToken->tokens.itemArray[3].tokenId, Equals(2));
      AssertThat(aToken->tokens.itemArray[3].textStart[0], Equals('A'));
      AssertThat(aToken->tokens.itemArray[3].tokens.numItems, Equals(0));
      AssertThat(aToken->tokens.itemArray[4].tokenId, Equals(1));
      AssertThat(aToken->tokens.itemArray[4].textStart[0], Equals(' '));
      AssertThat(aToken->tokens.itemArray[4].tokens.numItems, Equals(0));
      AssertThat(aToken->tokens.itemArray[5].tokenId, Equals(2));
      AssertThat(aToken->tokens.itemArray[5].textStart[0], Equals('t'));
      AssertThat(aToken->tokens.itemArray[5].tokens.numItems, Equals(0));
      AssertThat(aToken->tokens.itemArray[6].tokenId, Equals(1));
      AssertThat(aToken->tokens.itemArray[6].textStart[0], Equals(' '));
      AssertThat(aToken->tokens.itemArray[6].tokens.numItems, Equals(0));
      AssertThat(aToken->tokens.itemArray[7].tokenId, Equals(2));
      AssertThat(aToken->tokens.itemArray[7].textStart[0], Equals('B'));
      AssertThat(aToken->tokens.itemArray[7].tokens.numItems, Equals(0));
      AssertThat(aToken->tokens.itemArray[8].tokenId, Equals(1));
      AssertThat(aToken->tokens.itemArray[8].textStart[0], Equals(' '));
      AssertThat(aToken->tokens.itemArray[8].tokens.numItems, Equals(0));
      AssertThat(aToken->tokens.itemArray[9].tokenId, Equals(2));
      AssertThat(aToken->tokens.itemArray[9].textStart[0], Equals('e'));
      AssertThat(aToken->tokens.itemArray[9].tokens.numItems, Equals(0));
      AssertThat(aToken->tokens.itemArray[10].tokenId, Equals(1));
      AssertThat(aToken->tokens.itemArray[10].textStart[0], Equals(' '));
      AssertThat(aToken->tokens.itemArray[10].tokens.numItems, Equals(0));
      AssertThat(aToken->tokens.itemArray[11].tokenId, Equals(2));
      AssertThat(aToken->tokens.itemArray[11].textStart[0], Equals('C'));
      AssertThat(aToken->tokens.itemArray[11].tokens.numItems, Equals(0));
      AssertThat(aToken->tokens.itemArray[12].tokenId, Equals(1));
      AssertThat(aToken->tokens.itemArray[12].textStart[0], Equals(' '));
      AssertThat(aToken->tokens.itemArray[12].tokens.numItems, Equals(0));
    });

    it("Create a Parser and tokenize 'if A then B else C' ignoring whiteSpace", [&](){
      Parser *parser = new Parser();
      AssertThat(parser, Is().Not().EqualTo((void*)0));
      AssertThat(parser->dfa, Is().EqualTo((void*)0));
      parser->classifyWhiteSpace();
      AssertThat(parser->dfa, Is().EqualTo((void*)0));
      parser->addRuleIgnoreToken("whiteSpace", "[whiteSpace]+", WhiteSpace);
      AssertThat(parser->dfa, Is().EqualTo((void*)0));
      parser->addRule("nonWhiteSpace", "[!whiteSpace]+", NonWhiteSpace);
      AssertThat(parser->dfa, Is().EqualTo((void*)0));
      parser->addRule("start", "({whiteSpace}|{nonWhiteSpace})*", Text);
      AssertThat(parser->dfa, Is().EqualTo((void*)0));
      parser->compile();
      AssertThat(parser->dfa, Is().Not().EqualTo((void*)0));
      const char *cString ="  if A then B else C ";
      Utf8Chars *someChars = new Utf8Chars(cString);
      PDMTracer *pdmTracer =
        new PDMTracer("Parse and tokenize 'if then else'", stdout);
      pdmTracer->setCondition(PDMTracer::Progress |
                              PDMTracer::PDMStack |
                              PDMTracer::PDMTokens |
                              PDMTracer::PDMState);
//      Token *aToken = parser->parseFromUsing("start", someChars, pdmTracer);
      Token *aToken = parser->parseFromUsing("start", someChars, NULL);
      AssertThat(aToken, Is().Not().EqualTo((void*)0));
//      aToken->printOn(stdout);
      AssertThat(aToken->tokenId, Equals(4));
      AssertThat(aToken->tokens.getNumItems(), Equals(6));
      AssertThat(aToken->textStart, Equals(cString));
      AssertThat(aToken->textLength,
        Equals(someChars->getNumberOfBytesToRead()));
      AssertThat(aToken->tokens.itemArray[0].tokenId, Equals(2));
      AssertThat(aToken->tokens.itemArray[0].textStart[0], Equals('i'));
      AssertThat(aToken->tokens.itemArray[0].tokens.numItems, Equals(0));
      AssertThat(aToken->tokens.itemArray[1].tokenId, Equals(2));
      AssertThat(aToken->tokens.itemArray[1].textStart[0], Equals('A'));
      AssertThat(aToken->tokens.itemArray[1].tokens.numItems, Equals(0));
      AssertThat(aToken->tokens.itemArray[2].tokenId, Equals(2));
      AssertThat(aToken->tokens.itemArray[2].textStart[0], Equals('t'));
      AssertThat(aToken->tokens.itemArray[2].tokens.numItems, Equals(0));
      AssertThat(aToken->tokens.itemArray[3].tokenId, Equals(2));
      AssertThat(aToken->tokens.itemArray[3].textStart[0], Equals('B'));
      AssertThat(aToken->tokens.itemArray[3].tokens.numItems, Equals(0));
      AssertThat(aToken->tokens.itemArray[4].tokenId, Equals(2));
      AssertThat(aToken->tokens.itemArray[4].textStart[0], Equals('e'));
      AssertThat(aToken->tokens.itemArray[4].tokens.numItems, Equals(0));
      AssertThat(aToken->tokens.itemArray[5].tokenId, Equals(2));
      AssertThat(aToken->tokens.itemArray[5].textStart[0], Equals('C'));
      AssertThat(aToken->tokens.itemArray[5].tokens.numItems, Equals(0));
    });

  }); // describe parser

});
