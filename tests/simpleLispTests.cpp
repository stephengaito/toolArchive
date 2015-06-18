#include <bandit/bandit.h>
using namespace bandit;

#include <string.h>
#include <stdio.h>
#include <exception>

#ifndef protected
#define protected public
#endif

#include <dynUtf8Parser/simpleLisp.h>

go_bandit([](){

  printf("\n----------------------------------\n");
  printf(  "SimpleLisp\n");
  printf(  " SimpleLisp = %zu bytes (%zu bits)\n", sizeof(SimpleLisp),  sizeof(SimpleLisp)*8);
  printf(  "----------------------------------\n");

  /// \brief We test the SimpleLisp class.
  describe("SimpleLisp", [](){

    it("Create a SimpleLisp parser and tokenize 'hello'", [](){
      SimpleLisp *sLisp = new SimpleLisp();
      AssertThat(sLisp, Is().Not().EqualTo((void*)0));
      const char *cString ="hello";
      Utf8Chars *someChars = new Utf8Chars(cString);
      PDMTracer *pdmTracer =
        new PDMTracer("Parse and tokenize 'hello'", stdout);
      pdmTracer->setCondition(PDMTracer::Progress |
                              PDMTracer::PDMStack |
                              PDMTracer::PDMTokens |
                              PDMTracer::PDMState);
//      Token *aToken = sLisp->parse(someChars, pdmTracer);
      Token *aToken = sLisp->parse(someChars, NULL);
      AssertThat(aToken, Is().Not().EqualTo((void*)0));
//      aToken->printOn(stdout);
      AssertThat(aToken->tokenId, Equals(SimpleLisp::Expression));
      AssertThat(aToken->tokens.getNumItems(), Equals(1));
      AssertThat(aToken->textStart, Equals(cString));
      AssertThat(aToken->textLength,
        Equals(someChars->getNumberOfBytesToRead()));
      AssertThat(aToken->tokens.itemArray[0]->tokenId, Equals(SimpleLisp::Normal));
      AssertThat(aToken->tokens.itemArray[0]->textStart[0], Equals('h'));
      AssertThat(aToken->tokens.itemArray[0]->tokens.numItems, Equals(0));
    });

    it("Create a SimpleLisp parser and tokenize '(hello, there)'", [](){
      SimpleLisp *sLisp = new SimpleLisp();
      AssertThat(sLisp, Is().Not().EqualTo((void*)0));
      const char *cString ="(hello, there)";
      Utf8Chars *someChars = new Utf8Chars(cString);
      PDMTracer *pdmTracer =
        new PDMTracer("Parse and tokenize '(hello, there)'", stdout);
      pdmTracer->setCondition(PDMTracer::Progress | PDMTracer::All |
                              PDMTracer::PDMStack |
                              PDMTracer::PDMTokens |
                              PDMTracer::PDMState);
//      Token *aToken = sLisp->parse(someChars, pdmTracer);
      Token *aToken = sLisp->parse(someChars, NULL);
      AssertThat(aToken, Is().Not().EqualTo((void*)0));
//      aToken->printOn(stdout);
      AssertThat(aToken->tokenId, Equals(SimpleLisp::Expression));
      AssertThat(aToken->tokens.getNumItems(), Equals(2));
      AssertThat(aToken->textStart, Equals(cString));
      AssertThat(aToken->textLength,
        Equals(someChars->getNumberOfBytesToRead()));
      Token *childToken = aToken->tokens.itemArray[0];
      AssertThat(childToken->ASSERT_EQUALS(SimpleLisp::Expression, "hello"), Is().True());
      AssertThat(childToken->tokens.numItems, Equals(1));
      childToken = childToken->tokens.itemArray[0];
      AssertThat(childToken->ASSERT_EQUALS(SimpleLisp::Normal, "hello"), Is().True());
      AssertThat(childToken->tokens.numItems, Equals(0));
      childToken = aToken->tokens.itemArray[1];
      AssertThat(childToken->ASSERT_EQUALS(SimpleLisp::Expression, "there"), Is().True());
      AssertThat(childToken->tokens.numItems, Equals(1));
      childToken = childToken->tokens.itemArray[0];
      AssertThat(childToken->ASSERT_EQUALS(SimpleLisp::Normal, "there"), Is().True());
      AssertThat(childToken->tokens.numItems, Equals(0));
    });

    it("Create a SimpleLisp parser and tokenize a complex multi-level expression", [](){
      SimpleLisp *sLisp = new SimpleLisp();
      AssertThat(sLisp, Is().Not().EqualTo((void*)0));
      const char *cString ="((hello, there), (this, (is, (a, (test)))))";
      Utf8Chars *someChars = new Utf8Chars(cString);
      PDMTracer *pdmTracer =
        new PDMTracer("Parse and tokenize a complex multi-level expression", stdout);
      pdmTracer->setCondition(PDMTracer::Progress |
                              PDMTracer::PDMStack |
                              PDMTracer::SimpleState |
                              PDMTracer::Transitions);
//      Token *aToken = sLisp->parse(someChars, pdmTracer);
      Token *aToken = sLisp->parse(someChars, NULL);
      AssertThat(aToken, Is().Not().EqualTo((void*)0));
//      aToken->printOn(stdout);
      AssertThat(aToken->tokenId, Equals(SimpleLisp::Expression));
      AssertThat(aToken->tokens.getNumItems(), Equals(2));
      AssertThat(aToken->textStart, Equals(cString));
      AssertThat(aToken->textLength,
        Equals(someChars->getNumberOfBytesToRead()));
      Token *childToken = aToken->tokens.itemArray[0];
      AssertThat(childToken->ASSERT_EQUALS(SimpleLisp::Expression, "(hello, there)"), Is().True());
      AssertThat(childToken->tokens.numItems, Equals(2));
      childToken = childToken->tokens.itemArray[0];
      AssertThat(childToken->ASSERT_EQUALS(SimpleLisp::Expression, "hello"), Is().True());
      AssertThat(childToken->tokens.numItems, Equals(1));
      childToken = childToken->tokens.itemArray[0];
      AssertThat(childToken->ASSERT_EQUALS(SimpleLisp::Normal, "hello"), Is().True());
      AssertThat(childToken->tokens.numItems, Equals(0));
      childToken = aToken->tokens.itemArray[0];
      childToken = childToken->tokens.itemArray[1];
      AssertThat(childToken->ASSERT_EQUALS(SimpleLisp::Expression, "there"), Is().True());
      AssertThat(childToken->tokens.numItems, Equals(1));
      childToken = childToken->tokens.itemArray[0];
      AssertThat(childToken->ASSERT_EQUALS(SimpleLisp::Normal, "there"), Is().True());
      AssertThat(childToken->tokens.numItems, Equals(0));
      //
      childToken = aToken->tokens.itemArray[1];
      AssertThat(childToken->ASSERT_EQUALS(SimpleLisp::Expression, "(this, (is, (a, (test))))"), Is().True());
      AssertThat(childToken->tokens.numItems, Equals(2));
      Token *childToken0 = childToken->tokens.itemArray[0];
      AssertThat(childToken0->ASSERT_EQUALS(SimpleLisp::Expression, "this"), Is().True());
      AssertThat(childToken0->tokens.numItems, Equals(1));
      childToken0 = childToken0->tokens.itemArray[0];
      AssertThat(childToken0->ASSERT_EQUALS(SimpleLisp::Normal, "this"), Is().True());
      AssertThat(childToken0->tokens.numItems, Equals(0));
      //
      childToken = childToken->tokens.itemArray[1];
      AssertThat(childToken->ASSERT_EQUALS(SimpleLisp::Expression, "(is, (a, (test)))"), Is().True());
      AssertThat(childToken->tokens.numItems, Equals(2));
      childToken0 = childToken->tokens.itemArray[0];
      AssertThat(childToken0->ASSERT_EQUALS(SimpleLisp::Expression, "is"), Is().True());
      AssertThat(childToken0->tokens.numItems, Equals(1));
      childToken0 = childToken0->tokens.itemArray[0];
      AssertThat(childToken0->ASSERT_EQUALS(SimpleLisp::Normal, "is"), Is().True());
      AssertThat(childToken0->tokens.numItems, Equals(0));
      //
      childToken = childToken->tokens.itemArray[1];
      AssertThat(childToken->ASSERT_EQUALS(SimpleLisp::Expression, "(a, (test))"), Is().True());
      AssertThat(childToken->tokens.numItems, Equals(2));
      childToken0 = childToken->tokens.itemArray[0];
      AssertThat(childToken0->ASSERT_EQUALS(SimpleLisp::Expression, "a"), Is().True());
      AssertThat(childToken0->tokens.numItems, Equals(1));
      childToken0 = childToken0->tokens.itemArray[0];
      AssertThat(childToken0->ASSERT_EQUALS(SimpleLisp::Normal, "a"), Is().True());
      AssertThat(childToken0->tokens.numItems, Equals(0));
      //
      childToken = childToken->tokens.itemArray[1];
      AssertThat(childToken->ASSERT_EQUALS(SimpleLisp::Expression, "(test)"), Is().True());
      AssertThat(childToken->tokens.numItems, Equals(1));
      childToken0 = childToken->tokens.itemArray[0];
      AssertThat(childToken0->ASSERT_EQUALS(SimpleLisp::Expression, "test"), Is().True());
      AssertThat(childToken0->tokens.numItems, Equals(1));
      childToken0 = childToken0->tokens.itemArray[0];
      AssertThat(childToken0->ASSERT_EQUALS(SimpleLisp::Normal, "test"), Is().True());
      AssertThat(childToken0->tokens.numItems, Equals(0));
    });

  }); // describe parser

});
