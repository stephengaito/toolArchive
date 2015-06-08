#include <stdlib.h>

#include "parseTrees.h"

static const char *indents[] = {
  "", //0
  "  ", //1
  "    ", //2
  "      ", //3
  "        ", //4
  "          ", //5
  "            ", //6
  "              ", //7
  "                ", //8
  "                  ", //9
  "                    ", //10
  "                      ", //11
  "                        ", //12
  "                          ", //13
  "                            ", //14
  "                              ", //15
  "                                ", //16
  "                                  ", //17
  "                                    ", //18
  "                                      ", //19
  "                                      - "  //20
};

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
  VarArray<ParseTrees::Token*> *someTokens) {
  Token *newToken =
    (Token*)tokenAllocator->allocateNewStructure(sizeof(Token) +
      someTokens->getNumItems()*sizeof(Token*));
  newToken->wrappedId  = wrappedTokenId,
  newToken->textStart  = textStart;
  newToken->textLength = textLength;
  newToken->numTokens  = someTokens->getNumItems();
  if (newToken->numTokens) {
    someTokens->copyItems(newToken+sizeof(Token),
                         (someTokens->getNumItems())*sizeof(Token*));
  }
  return newToken;
}

void ParseTrees::printTokenOn(ParseTrees::Token *token,
                              FILE *outFile, size_t indent) {
  if (!outFile) return;
  if (20 < indent) indent = 20;
  if (!token) {
    fprintf(outFile, "%s(NULL TOKEN)\n", indents[indent]);
    return;
  }
  const char *text = strndup(token->textStart, token->textLength);
  fprintf(outFile, "%s%lu(%lu) [%s]\n",
          indents[indent], unwrapToken(token->wrappedId),
          token->numTokens, text);
  free((void*)text);
  for (size_t i = 0; i < token->numTokens; i++) {
    printTokenOn(token->tokens[i], outFile, indent+1);
  }
}
