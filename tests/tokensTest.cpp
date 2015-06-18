#include <bandit/bandit.h>
using namespace bandit;

#include <string.h>
#include <stdio.h>
#include <exception>

#ifndef protected
#define protected public
#endif

#include <dynUtf8Parser/tokens.h>


go_bandit([](){

  printf("\n----------------------------------\n");
  printf(  "Tokens\n");
  printf(  "     Token = %zu bytes (%zu bits)\n", sizeof(Token), sizeof(Token)*8);
  printf(  "----------------------------------\n");

  /// \brief We test the correctness of the ParseTrees class.
  describe("Tokens", [](){

    it("Should be able to create a null token", [](){
      Token *token = new Token();
      AssertThat(token,  Is().Not().EqualTo((void*)0));
      AssertThat(token->tokenId,    Equals(0));
      AssertThat(token->textStart,  Equals((void*)0));
      AssertThat(token->textLength, Equals(0));
      AssertThat(token->tokens.numItems, Equals(0));
      AssertThat(token->tokens.arraySize, Equals(0));
      AssertThat(token->tokens.itemArray, Equals((void*)0));
      delete token;
    });

    it("Should be able to setId/setText/addToken and copyFrom", [](){
      Token *token = new Token();
      AssertThat(token,  Is().Not().EqualTo((void*)0));
      AssertThat(token->tokenId,    Equals(0));
      AssertThat(token->textStart,  Equals((void*)0));
      AssertThat(token->textLength, Equals(0));
      AssertThat(token->tokens.numItems, Equals(0));
      AssertThat(token->tokens.arraySize, Equals(0));
      AssertThat(token->tokens.itemArray, Equals((void*)0));
      //
      // test setId
      //
      token->setId(1);
      AssertThat(token->tokenId, Equals(1));
      //
      // test setText
      //
      const char *someText = "this is some text";
      token->setText(someText, strlen(someText));
      AssertThat(token->textStart, Equals((char*)someText));
      AssertThat(token->textStart, Equals(someText));
      AssertThat(token->textLength, Equals(strlen(someText)));
      AssertThat(token->tokens.numItems, Equals(0));
      AssertThat(token->tokens.arraySize, Equals(0));
      AssertThat(token->tokens.itemArray, Equals((void*)0));
      //
      // now test addChildToken
      //
      Token *childToken = new Token();
      AssertThat(childToken, Is().Not().EqualTo((void*)0));
      childToken->setId(2);
      const char *childText = "this is some text for the child";
      childToken->setText(childText, strlen(childText));
      AssertThat(childToken->tokens.numItems, Equals(0));
      AssertThat(childToken->tokens.arraySize, Equals(0));
      AssertThat(childToken->tokens.itemArray, Equals((void*)0));
      token->addChildToken(childToken);
      AssertThat(token->tokens.numItems, Equals(1));
      AssertThat(token->tokens.arraySize, Is().Not().EqualTo(0));
      AssertThat(token->tokens.itemArray, Is().Not().EqualTo((void*)0));
      AssertThat(token->tokens.itemArray[0]->tokenId, Equals(2));
      AssertThat(token->tokens.itemArray[0]->textStart, Equals((char*)childText));
      AssertThat(token->tokens.itemArray[0]->textLength, Equals(strlen(childText)));
      AssertThat(token->tokens.itemArray[0]->tokens.numItems, Equals(0));
      AssertThat(token->tokens.itemArray[0]->tokens.arraySize, Equals(0));
      AssertThat(token->tokens.itemArray[0]->tokens.itemArray, Equals((void*)0));
      //
      // now test copyFrom
      //
      Token tokenCopy;
      tokenCopy = *token;
      AssertThat(tokenCopy.tokenId, Equals(token->tokenId));
      AssertThat(tokenCopy.textStart, Equals((char*)token->textStart));
      AssertThat(tokenCopy.textLength, Equals(token->textLength));
      AssertThat(tokenCopy.tokens.numItems, Equals(1));
      AssertThat(tokenCopy.tokens.numItems, Equals(token->tokens.numItems));
      AssertThat(tokenCopy.tokens.arraySize, Equals(token->tokens.arraySize));
      AssertThat(tokenCopy.tokens.itemArray, Is().Not().EqualTo(token->tokens.itemArray));
      AssertThat(tokenCopy.tokens.itemArray[0]->tokenId, Equals(2));
      AssertThat(tokenCopy.tokens.itemArray[0]->textStart, Equals((char*)childText));
      AssertThat(tokenCopy.tokens.itemArray[0]->textLength, Equals(strlen(childText)));
      AssertThat(tokenCopy.tokens.itemArray[0]->tokens.numItems, Equals(0));
      AssertThat(tokenCopy.tokens.itemArray[0]->tokens.arraySize, Equals(0));
      AssertThat(tokenCopy.tokens.itemArray[0]->tokens.itemArray, Equals((void*)0));
      //
      // now test clone
      //
      Token *tokenClone = token->clone();
      AssertThat(tokenClone->tokenId, Equals(token->tokenId));
      AssertThat(tokenClone->textStart, Equals((char*)token->textStart));
      AssertThat(tokenClone->textLength, Equals(token->textLength));
      AssertThat(tokenClone->tokens.numItems, Equals(1));
      AssertThat(tokenClone->tokens.numItems, Equals(token->tokens.numItems));
      AssertThat(tokenClone->tokens.arraySize, Equals(token->tokens.arraySize));
      AssertThat(tokenClone->tokens.itemArray, Is().Not().EqualTo(token->tokens.itemArray));
      AssertThat(tokenClone->tokens.itemArray[0]->tokenId, Equals(2));
      AssertThat(tokenClone->tokens.itemArray[0]->textStart, Equals((char*)childText));
      AssertThat(tokenClone->tokens.itemArray[0]->textLength, Equals(strlen(childText)));
      AssertThat(tokenClone->tokens.itemArray[0]->tokens.numItems, Equals(0));
      AssertThat(tokenClone->tokens.itemArray[0]->tokens.arraySize, Equals(0));
      AssertThat(tokenClone->tokens.itemArray[0]->tokens.itemArray, Equals((void*)0));

      delete tokenClone;
      tokenCopy.~Token();
      delete childToken;
      delete token;
    });

    it("should be able to add deep collections of child tokens", [](){
      Token *token0 = new Token(1, "0Token");
      Token *token1 = new Token(2, "1Token");
      Token *token2 = new Token(3, "2Token");
      Token *token3 = new Token(4, "3Token");
      Token *token4 = new Token(5, "4Token");
      Token *token5 = new Token(6, "5Token");
      Token *token6 = new Token(7, "6Token");
      token5->addChildToken(token6);
      token4->addChildToken(token5);
      token3->addChildToken(token4);
      token2->addChildToken(token3);
      token1->addChildToken(token2);
      token0->addChildToken(token1);
      AssertThat(token0->tokenId, Equals(1));
      AssertThat(token0->tokens.numItems, Equals(1));
      //
      Token *childToken = token0->tokens.itemArray[0];
      AssertThat(childToken->tokenId, Equals(token1->tokenId));
      AssertThat(childToken->textStart, Equals(token1->textStart));
      AssertThat(childToken->tokens.numItems, Equals(1));
      //
      childToken = childToken->tokens.itemArray[0];
      AssertThat(childToken->tokenId, Equals(token2->tokenId));
      AssertThat(childToken->textStart, Equals(token2->textStart));
      AssertThat(childToken->tokens.numItems, Equals(1));
      //
      childToken = childToken->tokens.itemArray[0];
      AssertThat(childToken->tokenId, Equals(token3->tokenId));
      AssertThat(childToken->textStart, Equals(token3->textStart));
      AssertThat(childToken->tokens.numItems, Equals(1));
      //
      childToken = childToken->tokens.itemArray[0];
      AssertThat(childToken->tokenId, Equals(token4->tokenId));
      AssertThat(childToken->textStart, Equals(token4->textStart));
      AssertThat(childToken->tokens.numItems, Equals(1));
      //
      childToken = childToken->tokens.itemArray[0];
      AssertThat(childToken->tokenId, Equals(token5->tokenId));
      AssertThat(childToken->textStart, Equals(token5->textStart));
      AssertThat(childToken->tokens.numItems, Equals(1));
      //
      childToken = childToken->tokens.itemArray[0];
      AssertThat(childToken->tokenId, Equals(token6->tokenId));
      AssertThat(childToken->textStart, Equals(token6->textStart));
      AssertThat(childToken->tokens.numItems, Equals(0));
   });

  }); // describe ParseTrees

});
