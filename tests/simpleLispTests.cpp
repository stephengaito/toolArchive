#include <string.h>
#include <stdio.h>
#include <exception>

#include <cUtils/specs/specs.h>

#ifndef protected
#define protected public
#endif

#include <dynUtf8Parser/simpleLisp.h>

/// \brief We test the SimpleLisp class.
describe(SimpleLisp) {

  specSize(SimpleLisp);

  it("Create a SimpleLisp parser and tokenize 'hello'") {
    SimpleLisp *sLisp = new SimpleLisp();
    shouldNotBeNULL(sLisp);
    const char *cString ="hello";
    Utf8Chars *someChars = new Utf8Chars(cString);
    PDMTracer *pdmTracer =
      new PDMTracer("Parse and tokenize 'hello'", stdout);
    pdmTracer->setCondition(PDMTracer::Progress |
                            PDMTracer::PDMStack |
                            PDMTracer::PDMTokens |
                            PDMTracer::PDMState);
//    Token *aToken = sLisp->parse(someChars, pdmTracer);
    Token *aToken = sLisp->parse(someChars, NULL);
    shouldNotBeNULL(aToken);
//    aToken->printOn(stdout);
    shouldBeEqual(aToken->tokenId, (SimpleLisp::Expression));
    shouldBeEqual(aToken->tokens.getNumItems(), (1));
    shouldBeEqual(aToken->textStart, (cString));
    shouldBeEqual(aToken->textLength,
      (someChars->getNumberOfBytesToRead()));
    shouldBeEqual(aToken->tokens.itemArray[0]->tokenId, (SimpleLisp::Normal));
    shouldBeEqual(aToken->tokens.itemArray[0]->textStart[0], ('h'));
    shouldBeEqual(aToken->tokens.itemArray[0]->tokens.numItems, (0));
    delete aToken;
    delete pdmTracer;
    delete someChars;
    delete sLisp;
  } endIt();

  it("Create a SimpleLisp parser and tokenize '(hello, there)'") {
    SimpleLisp *sLisp = new SimpleLisp();
    shouldNotBeNULL(sLisp);
    const char *cString ="(hello, there)";
    Utf8Chars *someChars = new Utf8Chars(cString);
    PDMTracer *pdmTracer =
      new PDMTracer("Parse and tokenize '(hello, there)'", stdout);
    pdmTracer->setCondition(PDMTracer::Progress | PDMTracer::All |
                            PDMTracer::PDMStack |
                            PDMTracer::PDMTokens |
                            PDMTracer::PDMState);
//   Token *aToken = sLisp->parse(someChars, pdmTracer);
    Token *aToken = sLisp->parse(someChars, NULL);
    shouldNotBeNULL(aToken);
//    aToken->printOn(stdout);
    shouldBeEqual(aToken->tokenId, (SimpleLisp::Expression));
    shouldBeEqual(aToken->tokens.getNumItems(), (2));
    shouldBeEqual(aToken->textStart, (cString));
    shouldBeEqual(aToken->textLength,
      (someChars->getNumberOfBytesToRead()));
    Token *childToken = (aToken->tokens.itemArray[0]);
    shouldBeTrue(childToken->ASSERT_EQUALS(SimpleLisp::Expression, "hello"));
    shouldBeEqual(childToken->tokens.numItems, (1));
    childToken = (childToken->tokens.itemArray[0]);
    shouldBeTrue(childToken->ASSERT_EQUALS(SimpleLisp::Normal, "hello"));
    shouldBeEqual(childToken->tokens.numItems, (0));
    childToken = (aToken->tokens.itemArray[1]);
    shouldBeTrue(childToken->ASSERT_EQUALS(SimpleLisp::Expression, "there"));
    shouldBeEqual(childToken->tokens.numItems, (1));
    childToken = (childToken->tokens.itemArray[0]);
    shouldBeTrue(childToken->ASSERT_EQUALS(SimpleLisp::Normal, "there"));
    shouldBeEqual(childToken->tokens.numItems, (0));
    delete aToken;
    delete pdmTracer;
    delete someChars;
    delete sLisp;
  } endIt();

  it("Create a SimpleLisp parser and tokenize a complex multi-level",
    "expression") {
    SimpleLisp *sLisp = new SimpleLisp();
    shouldNotBeNULL(sLisp);
    const char *cString ="((hello, there), (this, (is, (a, (test)))))";
    Utf8Chars *someChars = new Utf8Chars(cString);
    PDMTracer *pdmTracer =
      new PDMTracer("Parse and tokenize a complex multi-level expression", stdout);
    pdmTracer->setCondition(PDMTracer::Progress |
                            PDMTracer::PDMStack |
                            PDMTracer::SimpleState |
                            PDMTracer::Transitions);
//    Token *aToken = sLisp->parse(someChars, pdmTracer);
    Token *aToken = sLisp->parse(someChars, NULL);
    shouldNotBeNULL(aToken);
//    aToken->printOn(stdout);
    shouldBeEqual(aToken->tokenId, (SimpleLisp::Expression));
    shouldBeEqual(aToken->tokens.getNumItems(), (2));
    shouldBeEqual(aToken->textStart, (cString));
    shouldBeEqual(aToken->textLength,
      (someChars->getNumberOfBytesToRead()));
    Token *childToken = (aToken->tokens.itemArray[0]);
    shouldBeTrue(childToken->ASSERT_EQUALS(SimpleLisp::Expression, "(hello, there)"));
    shouldBeEqual(childToken->tokens.numItems, (2));
    childToken = (childToken->tokens.itemArray[0]);
    shouldBeTrue(childToken->ASSERT_EQUALS(SimpleLisp::Expression, "hello"));
    shouldBeEqual(childToken->tokens.numItems, (1));
    childToken = (childToken->tokens.itemArray[0]);
    shouldBeTrue(childToken->ASSERT_EQUALS(SimpleLisp::Normal, "hello"));
    shouldBeEqual(childToken->tokens.numItems, (0));
    childToken = (aToken->tokens.itemArray[0]);
    childToken = (childToken->tokens.itemArray[1]);
    shouldBeTrue(childToken->ASSERT_EQUALS(SimpleLisp::Expression, "there"));
    shouldBeEqual(childToken->tokens.numItems, (1));
    childToken = (childToken->tokens.itemArray[0]);
    shouldBeTrue(childToken->ASSERT_EQUALS(SimpleLisp::Normal, "there"));
    shouldBeEqual(childToken->tokens.numItems, (0));
    //
    childToken = (aToken->tokens.itemArray[1]);
    shouldBeTrue(childToken->ASSERT_EQUALS(SimpleLisp::Expression, "(this, (is, (a, (test))))"));
    shouldBeEqual(childToken->tokens.numItems, (2));
    Token *childToken0 = (childToken->tokens.itemArray[0]);
    shouldBeTrue(childToken0->ASSERT_EQUALS(SimpleLisp::Expression, "this"));
    shouldBeEqual(childToken0->tokens.numItems, (1));
    childToken0 = (childToken0->tokens.itemArray[0]);
    shouldBeTrue(childToken0->ASSERT_EQUALS(SimpleLisp::Normal, "this"));
    shouldBeEqual(childToken0->tokens.numItems, (0));
    //
    childToken = (childToken->tokens.itemArray[1]);
    shouldBeTrue(childToken->ASSERT_EQUALS(SimpleLisp::Expression, "(is, (a, (test)))"));
    shouldBeEqual(childToken->tokens.numItems, (2));
    childToken0 = (childToken->tokens.itemArray[0]);
    shouldBeTrue(childToken0->ASSERT_EQUALS(SimpleLisp::Expression, "is"));
    shouldBeEqual(childToken0->tokens.numItems, (1));
    childToken0 = (childToken0->tokens.itemArray[0]);
    shouldBeTrue(childToken0->ASSERT_EQUALS(SimpleLisp::Normal, "is"));
    shouldBeEqual(childToken0->tokens.numItems, (0));
    //
    childToken = (childToken->tokens.itemArray[1]);
    shouldBeTrue(childToken->ASSERT_EQUALS(SimpleLisp::Expression, "(a, (test))"));
    shouldBeEqual(childToken->tokens.numItems, (2));
    childToken0 = (childToken->tokens.itemArray[0]);
    shouldBeTrue(childToken0->ASSERT_EQUALS(SimpleLisp::Expression, "a"));
    shouldBeEqual(childToken0->tokens.numItems, (1));
    childToken0 = (childToken0->tokens.itemArray[0]);
    shouldBeTrue(childToken0->ASSERT_EQUALS(SimpleLisp::Normal, "a"));
    shouldBeEqual(childToken0->tokens.numItems, (0));
    //
    childToken = (childToken->tokens.itemArray[1]);
    shouldBeTrue(childToken->ASSERT_EQUALS(SimpleLisp::Expression, "(test)"));
    shouldBeEqual(childToken->tokens.numItems, (1));
    childToken0 = (childToken->tokens.itemArray[0]);
    shouldBeTrue(childToken0->ASSERT_EQUALS(SimpleLisp::Expression, "test"));
    shouldBeEqual(childToken0->tokens.numItems, (1));
    childToken0 = (childToken0->tokens.itemArray[0]);
    shouldBeTrue(childToken0->ASSERT_EQUALS(SimpleLisp::Normal, "test"));
    shouldBeEqual(childToken0->tokens.numItems, (0));
    delete aToken;
    delete pdmTracer;
    delete someChars;
    delete sLisp;
  } endIt();

} endDescribe(SimpleLisp);
