#include <stdlib.h>

#include "parseTrees.h"

ParseTrees::ParseTrees(void) {
  tokenAllocator =
    new BlockAllocator(NUM_TOKEN_PTRS_PER_BLOCK*sizeof(void*));
  streams = new StreamRegistry();
}

ParseTrees::~ParseTrees(void) {
  if (tokenAllocator) delete tokenAllocator;
  tokenAllocator = NULL;

  if (streams) delete streams;
  streams = NULL;
}

ParseTrees::Token *ParseTrees::allocateNewToken(ParseTrees::WrappedTokenId wrappedTokenId,
  const char *textStart, size_t textLength,
  VarArray<ParseTrees::Token*> &someTokens) {
  Token *newToken =
    (Token*)tokenAllocator->allocateNewStructure(sizeof(Token) +
      someTokens.getNumItems()*sizeof(Token*));
  newToken->wrappedId  = wrappedTokenId,
  newToken->textStart  = textStart;
  newToken->textLength = textLength;
  newToken->numTokens  = someTokens.getNumItems();
  if (newToken->numTokens) {
    someTokens.copyItems(newToken+sizeof(Token),
                         someTokens.getNumItems()*sizeof(Token*));
  }
  return newToken;
}
