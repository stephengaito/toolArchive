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

//ParseTrees::Token *ParseTrees::getNewToken(size_t numSubTokens) {
//  return (Token*)tokenAllocator->allocateNewStructure(sizeof(Token)+numSubTokens);
//}
