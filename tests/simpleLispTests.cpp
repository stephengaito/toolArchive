#include <bandit/bandit.h>
using namespace bandit;

#include <string.h>
#include <stdio.h>
#include <exception>

#ifndef protected
#define protected public
#endif

#include <simpleLisp.h>

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
      AssertThat(aToken->tokens.itemArray[0].tokenId, Equals(SimpleLisp::NonWhiteSpace));
      AssertThat(aToken->tokens.itemArray[0].textStart[0], Equals('h'));
      AssertThat(aToken->tokens.itemArray[0].tokens.numItems, Equals(0));
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
      AssertThat(aToken->tokens.itemArray[0].tokenId, Equals(SimpleLisp::Expression));
      AssertThat(aToken->tokens.itemArray[0].textStart[0], Equals('h'));
      AssertThat(aToken->tokens.itemArray[0].tokens.numItems, Equals(0));
      AssertThat(aToken->tokens.itemArray[1].tokenId, Equals(SimpleLisp::Expression));
      AssertThat(aToken->tokens.itemArray[1].textStart[0], Equals('t'));
      AssertThat(aToken->tokens.itemArray[1].tokens.numItems, Equals(0));
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
                              PDMTracer::PDMTokens |
                              PDMTracer::PDMState);
      Token *aToken = sLisp->parse(someChars, pdmTracer);
//      Token *aToken = sLisp->parse(someChars, NULL);
      AssertThat(aToken, Is().Not().EqualTo((void*)0));
      aToken->printOn(stdout);
      AssertThat(aToken->tokenId, Equals(SimpleLisp::Expression));
      AssertThat(aToken->tokens.getNumItems(), Equals(2));
      AssertThat(aToken->textStart, Equals(cString));
      AssertThat(aToken->textLength,
        Equals(someChars->getNumberOfBytesToRead()));
      AssertThat(aToken->tokens.itemArray[0].tokenId, Equals(SimpleLisp::Expression));
      AssertThat(aToken->tokens.itemArray[0].textStart[0], Equals('h'));
      AssertThat(aToken->tokens.itemArray[0].tokens.numItems, Equals(0));
      AssertThat(aToken->tokens.itemArray[1].tokenId, Equals(SimpleLisp::Expression));
      AssertThat(aToken->tokens.itemArray[1].textStart[0], Equals('t'));
      AssertThat(aToken->tokens.itemArray[1].tokens.numItems, Equals(0));
    });

  }); // describe parser

});
