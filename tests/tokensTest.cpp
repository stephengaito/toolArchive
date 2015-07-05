#include <string.h>
#include <stdio.h>
#include <exception>

#include <cUtils/specs/specs.h>

#ifndef protected
#define protected public
#endif

#include <dynUtf8Parser/tokens.h>

/// \brief We test the correctness of the ParseTrees class.
describe(Tokens) {

  specSize(Token);

  it("Should be able to create a null token") {
    Token *token = new Token();
    shouldNotBeNULL(token);
    shouldBeZero(token->tokenId);
    shouldBeNULL((void*)token->textStart);
    shouldBeZero(token->textLength);
    shouldBeZero(token->tokens.numItems);
    shouldBeZero(token->tokens.arraySize);
    shouldBeNULL(token->tokens.itemArray);
    delete token;
  } endIt();

  pending_it("Should be able to setId/setText/addToken and copyFrom") {
    Token *token = new Token();
    shouldNotBeNULL(token);
    shouldBeZero(token->tokenId);
    shouldBeNULL((void*)token->textStart);
    shouldBeZero(token->textLength);
    shouldBeZero(token->tokens.numItems);
    shouldBeZero(token->tokens.arraySize);
    shouldBeNULL(token->tokens.itemArray);
    //
    // test setId
    //
    token->setId(1);
    shouldBeEqual(token->tokenId, 1);
    //
    // test setText
    //
    const char *someText = "this is some text";
    token->setText(someText, strlen(someText));
    shouldBeEqual((void*)(token->textStart), (void*)someText);
    shouldBeEqual(token->textStart, someText);
    shouldBeEqual(token->textLength, strlen(someText));
    shouldBeZero(token->tokens.numItems);
    shouldBeZero(token->tokens.arraySize);
    shouldBeNULL(token->tokens.itemArray);
    //
    // now test addChildToken
    //
    Token *childToken = new Token();
    shouldNotBeNULL(childToken);
    childToken->setId(2);
    const char *childText = "this is some text for the child";
    childToken->setText(childText, strlen(childText));
    shouldBeZero(childToken->tokens.numItems);
    shouldBeZero(childToken->tokens.arraySize);
    shouldBeNULL(childToken->tokens.itemArray);
    token->addChildToken(childToken);
    shouldBeEqual(token->tokens.numItems, 1);
    shouldNotBeZero(token->tokens.arraySize);
    shouldNotBeNULL(token->tokens.itemArray);
    shouldBeEqual(token->tokens.itemArray[0]->tokenId, 2);
    shouldBeEqual((void*)(token->tokens.itemArray[0]->textStart), (void*)childText);
    shouldBeEqual(token->tokens.itemArray[0]->textLength, strlen(childText));
    shouldBeZero(token->tokens.itemArray[0]->tokens.numItems);
    shouldBeZero(token->tokens.itemArray[0]->tokens.arraySize);
    shouldBeNULL(token->tokens.itemArray[0]->tokens.itemArray);
    //
    // now test copyFrom
    //
    Token tokenCopy;
    tokenCopy = *token;
    shouldBeEqual(tokenCopy.tokenId, token->tokenId);
    shouldBeEqual((void*)(tokenCopy.textStart), (void*)(token->textStart));
    shouldBeEqual(tokenCopy.textLength, token->textLength);
    shouldBeEqual(tokenCopy.tokens.numItems, 1);
    shouldBeEqual(tokenCopy.tokens.numItems, token->tokens.numItems);
    shouldBeEqual(tokenCopy.tokens.arraySize, token->tokens.arraySize);
    shouldNotBeEqual(tokenCopy.tokens.itemArray, token->tokens.itemArray);
    shouldBeEqual(tokenCopy.tokens.itemArray[0]->tokenId, 2);
    shouldBeEqual((void*)(tokenCopy.tokens.itemArray[0]->textStart), (void*)childText);
    shouldBeEqual(tokenCopy.tokens.itemArray[0]->textLength, strlen(childText));
    shouldBeZero(tokenCopy.tokens.itemArray[0]->tokens.numItems);
    shouldBeZero(tokenCopy.tokens.itemArray[0]->tokens.arraySize);
    shouldBeNULL(tokenCopy.tokens.itemArray[0]->tokens.itemArray);
    //
    // now test clone
    //
    Token *tokenClone = token->clone();
    shouldBeEqual(tokenClone->tokenId, token->tokenId);
    shouldBeEqual((void*)(tokenClone->textStart), (void*)(token->textStart));
    shouldBeEqual(tokenClone->textLength, token->textLength);
    shouldBeEqual(tokenClone->tokens.numItems, 1);
    shouldBeEqual(tokenClone->tokens.numItems, token->tokens.numItems);
    shouldBeEqual(tokenClone->tokens.arraySize, token->tokens.arraySize);
    shouldNotBeEqual(tokenClone->tokens.itemArray, token->tokens.itemArray);
    shouldBeEqual(tokenClone->tokens.itemArray[0]->tokenId, 2);
    shouldBeEqual((void*)(tokenClone->tokens.itemArray[0]->textStart), (void*)childText);
    shouldBeEqual(tokenClone->tokens.itemArray[0]->textLength, strlen(childText));
    shouldBeZero(tokenClone->tokens.itemArray[0]->tokens.numItems);
    shouldBeZero(tokenClone->tokens.itemArray[0]->tokens.arraySize);
    shouldBeNULL(tokenClone->tokens.itemArray[0]->tokens.itemArray);

    delete tokenClone;
    tokenCopy.~Token();
    delete childToken;
    delete token;
  } endIt();

  pending_it("should be able to add deep collections of child tokens") {
    Token *token0 = new Token(1, "0Token");
    shouldNotBeNULL(token0);
    Token *token1 = new Token(2, "1Token");
    shouldNotBeNULL(token1);
    Token *token2 = new Token(3, "2Token");
    shouldNotBeNULL(token2);
    Token *token3 = new Token(4, "3Token");
    shouldNotBeNULL(token3);
    Token *token4 = new Token(5, "4Token");
    shouldNotBeNULL(token4);
    Token *token5 = new Token(6, "5Token");
    shouldNotBeNULL(token5);
    Token *token6 = new Token(7, "6Token");
    shouldNotBeNULL(token6);
    token5->addChildToken(token6);
    token4->addChildToken(token5);
    token3->addChildToken(token4);
    token2->addChildToken(token3);
    token1->addChildToken(token2);
    token0->addChildToken(token1);
    shouldBeEqual(token0->tokenId, 1);
    shouldBeEqual(token0->tokens.numItems, 1);
    //
    Token *childToken = token0->tokens.itemArray[0];
    shouldBeEqual(childToken->tokenId, token1->tokenId);
    shouldBeEqual(childToken->textStart, token1->textStart);
    shouldBeEqual(childToken->tokens.numItems, 1);
    //
    childToken = childToken->tokens.itemArray[0];
    shouldBeEqual(childToken->tokenId, token2->tokenId);
    shouldBeEqual(childToken->textStart, token2->textStart);
    shouldBeEqual(childToken->tokens.numItems, 1);
    //
    childToken = childToken->tokens.itemArray[0];
    shouldBeEqual(childToken->tokenId, token3->tokenId);
    shouldBeEqual(childToken->textStart, token3->textStart);
    shouldBeEqual(childToken->tokens.numItems, 1);
    //
    childToken = childToken->tokens.itemArray[0];
    shouldBeEqual(childToken->tokenId, token4->tokenId);
    shouldBeEqual(childToken->textStart, token4->textStart);
    shouldBeEqual(childToken->tokens.numItems, 1);
    //
    childToken = childToken->tokens.itemArray[0];
    shouldBeEqual(childToken->tokenId, token5->tokenId);
    shouldBeEqual(childToken->textStart, token5->textStart);
    shouldBeEqual(childToken->tokens.numItems, 1);
    //
    childToken = childToken->tokens.itemArray[0];
    shouldBeEqual(childToken->tokenId, token6->tokenId);
    shouldBeEqual(childToken->textStart, token6->textStart);
    shouldBeZero(childToken->tokens.numItems);
    delete token6;
    delete token5;
    delete token4;
    delete token3;
    delete token2;
    delete token1;
    delete token0;
 } endIt();

} endDescribe(Tokens);
