#include <string.h>
#include <stdio.h>
#include <exception>
#include <execinfo.h>

#include <cUtils/specs/specs.h>

#ifndef protected
#define protected public
#endif

#include <dynUtf8Parser/parser.h>

enum ParserTestTokens {
  WhiteSpace=1,
  NonWhiteSpace=2,
  Text=4
};

/// \brief We test the Parser class.
pending_describe(Parser) {

  specSize(Parser);

  it("Create a Parser and tokenize 'if A then B else C'") {
    Parser *parser = new Parser();
    shouldNotBeNULL(parser);
    shouldBeNULL(parser->dfa);
    parser->classifyWhiteSpace();
    shouldBeNULL(parser->dfa);
    parser->addRule("whiteSpace", "[whiteSpace]+", WhiteSpace);
    shouldBeNULL(parser->dfa);
    parser->addRule("nonWhiteSpace", "[!whiteSpace]+", NonWhiteSpace);
    parser->addRule("start", "({whiteSpace}|{nonWhiteSpace})*", Text);
    shouldBeNULL(parser->dfa);
    parser->compile();
    shouldNotBeNULL(parser->dfa);
    const char *cString ="  if A then B else C ";
    Utf8Chars *someChars = new Utf8Chars(cString);
    PDMTracer *pdmTracer =
      new PDMTracer("Parse and tokenize 'if then else'", stdout);
    pdmTracer->setCondition(PDMTracer::Progress |
                            PDMTracer::PDMStack |
                            PDMTracer::PDMTokens |
                            PDMTracer::PDMState);
//    Token *aToken = parser->parseFromUsing("start", someChars, pdmTracer);
    Token *aToken = parser->parseFromUsing("start", someChars, NULL);
    shouldNotBeNULL(aToken);
//    aToken->printOn(stdout);
    shouldBeEqual(aToken->tokenId, 4);
    shouldBeEqual(aToken->tokens.getNumItems(), (13));
    shouldBeEqual(aToken->textStart, (cString));
    shouldBeEqual(aToken->textLength,
      (someChars->getNumberOfBytesToRead()));
    shouldBeEqual(aToken->tokens.itemArray[0]->tokenId, (1));
    shouldBeEqual(aToken->tokens.itemArray[0]->textStart[0], (' '));
    shouldBeEqual(aToken->tokens.itemArray[0]->tokens.numItems, (0));
    shouldBeEqual(aToken->tokens.itemArray[1]->tokenId, (2));
    shouldBeEqual(aToken->tokens.itemArray[1]->textStart[0], ('i'));
    shouldBeEqual(aToken->tokens.itemArray[1]->tokens.numItems, (0));
    shouldBeEqual(aToken->tokens.itemArray[2]->tokenId, (1));
    shouldBeEqual(aToken->tokens.itemArray[2]->textStart[0], (' '));
    shouldBeEqual(aToken->tokens.itemArray[2]->tokens.numItems, (0));
    shouldBeEqual(aToken->tokens.itemArray[3]->tokenId, (2));
    shouldBeEqual(aToken->tokens.itemArray[3]->textStart[0], ('A'));
    shouldBeEqual(aToken->tokens.itemArray[3]->tokens.numItems, (0));
    shouldBeEqual(aToken->tokens.itemArray[4]->tokenId, (1));
    shouldBeEqual(aToken->tokens.itemArray[4]->textStart[0], (' '));
    shouldBeEqual(aToken->tokens.itemArray[4]->tokens.numItems, (0));
    shouldBeEqual(aToken->tokens.itemArray[5]->tokenId, (2));
    shouldBeEqual(aToken->tokens.itemArray[5]->textStart[0], ('t'));
    shouldBeEqual(aToken->tokens.itemArray[5]->tokens.numItems, (0));
    shouldBeEqual(aToken->tokens.itemArray[6]->tokenId, (1));
    shouldBeEqual(aToken->tokens.itemArray[6]->textStart[0], (' '));
    shouldBeEqual(aToken->tokens.itemArray[6]->tokens.numItems, (0));
    shouldBeEqual(aToken->tokens.itemArray[7]->tokenId, (2));
    shouldBeEqual(aToken->tokens.itemArray[7]->textStart[0], ('B'));
    shouldBeEqual(aToken->tokens.itemArray[7]->tokens.numItems, (0));
    shouldBeEqual(aToken->tokens.itemArray[8]->tokenId, (1));
    shouldBeEqual(aToken->tokens.itemArray[8]->textStart[0], (' '));
    shouldBeEqual(aToken->tokens.itemArray[8]->tokens.numItems, (0));
    shouldBeEqual(aToken->tokens.itemArray[9]->tokenId, (2));
    shouldBeEqual(aToken->tokens.itemArray[9]->textStart[0], ('e'));
    shouldBeEqual(aToken->tokens.itemArray[9]->tokens.numItems, (0));
    shouldBeEqual(aToken->tokens.itemArray[10]->tokenId, (1));
    shouldBeEqual(aToken->tokens.itemArray[10]->textStart[0], (' '));
    shouldBeEqual(aToken->tokens.itemArray[10]->tokens.numItems, (0));
    shouldBeEqual(aToken->tokens.itemArray[11]->tokenId, (2));
    shouldBeEqual(aToken->tokens.itemArray[11]->textStart[0], ('C'));
    shouldBeEqual(aToken->tokens.itemArray[11]->tokens.numItems, (0));
    shouldBeEqual(aToken->tokens.itemArray[12]->tokenId, (1));
    shouldBeEqual(aToken->tokens.itemArray[12]->textStart[0], (' '));
    shouldBeEqual(aToken->tokens.itemArray[12]->tokens.numItems, (0));
    delete aToken;
    delete parser;
  } endIt();

  pending_it("Create a Parser and tokenize 'if A then B else C' ignoring whiteSpace") {
    Parser *parser = new Parser();
    shouldNotBeNULL(parser);
    shouldBeNULL(parser->dfa);
    parser->classifyWhiteSpace();
    shouldBeNULL(parser->dfa);
    parser->addRuleIgnoreToken("whiteSpace", "[whiteSpace]+", WhiteSpace);
    shouldBeNULL(parser->dfa);
    parser->addRule("nonWhiteSpace", "[!whiteSpace]+", NonWhiteSpace);
    shouldBeNULL(parser->dfa);
    parser->addRule("start", "({whiteSpace}|{nonWhiteSpace})*", Text);
    shouldBeNULL(parser->dfa);
    parser->compile();
    shouldNotBeNULL(parser->dfa);
    const char *cString ="  if A then B else C ";
    Utf8Chars *someChars = new Utf8Chars(cString);
    PDMTracer *pdmTracer =
      new PDMTracer("Parse and tokenize 'if then else'", stdout);
    pdmTracer->setCondition(PDMTracer::Progress |
                            PDMTracer::PDMStack |
                            PDMTracer::PDMTokens |
                            PDMTracer::PDMState);
//    Token *aToken = parser->parseFromUsing("start", someChars, pdmTracer);
    Token *aToken = parser->parseFromUsing("start", someChars, NULL);
    shouldNotBeNULL(aToken);
//    aToken->printOn(stdout);
    shouldBeEqual(aToken->tokenId, (4));
    shouldBeEqual(aToken->tokens.getNumItems(), (6));
    shouldBeEqual(aToken->textStart, (cString));
    shouldBeEqual(aToken->textLength,
      (someChars->getNumberOfBytesToRead()));
    shouldBeEqual(aToken->tokens.itemArray[0]->tokenId, (2));
    shouldBeEqual(aToken->tokens.itemArray[0]->textStart[0], ('i'));
    shouldBeEqual(aToken->tokens.itemArray[0]->tokens.numItems, (0));
    shouldBeEqual(aToken->tokens.itemArray[1]->tokenId, (2));
    shouldBeEqual(aToken->tokens.itemArray[1]->textStart[0], ('A'));
    shouldBeEqual(aToken->tokens.itemArray[1]->tokens.numItems, (0));
    shouldBeEqual(aToken->tokens.itemArray[2]->tokenId, (2));
    shouldBeEqual(aToken->tokens.itemArray[2]->textStart[0], ('t'));
    shouldBeEqual(aToken->tokens.itemArray[2]->tokens.numItems, (0));
    shouldBeEqual(aToken->tokens.itemArray[3]->tokenId, (2));
    shouldBeEqual(aToken->tokens.itemArray[3]->textStart[0], ('B'));
    shouldBeEqual(aToken->tokens.itemArray[3]->tokens.numItems, (0));
    shouldBeEqual(aToken->tokens.itemArray[4]->tokenId, (2));
    shouldBeEqual(aToken->tokens.itemArray[4]->textStart[0], ('e'));
    shouldBeEqual(aToken->tokens.itemArray[4]->tokens.numItems, (0));
    shouldBeEqual(aToken->tokens.itemArray[5]->tokenId, (2));
    shouldBeEqual(aToken->tokens.itemArray[5]->textStart[0], ('C'));
    shouldBeEqual(aToken->tokens.itemArray[5]->tokens.numItems, (0));
    delete aToken;
    delete parser;
  } endIt();

} endDescribe(Parser);
