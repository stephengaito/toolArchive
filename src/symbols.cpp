#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "symbols.h"


Symbols::Symbols(void) {
  symbol2SymbolEntryMap = hattrie_create();
  symbolAllocator       = new BlockAllocator(10*sizeof(SymbolEntry));
};

Symbols::~Symbols(void) {
  if (symbol2SymbolEntryMap) hattrie_free(symbol2SymbolEntryMap);
  if (symbolAllocator) symbolAllocator->~BlockAllocator();
};

Symbols::SymbolEntry *Symbols::registerSymbol(const char *symbol,
  size_t symbolLen) {
  SymbolEntry **sePtr = (SymbolEntry **)hattrie_get(symbol2SymbolEntryMap,
                                                    symbol, symbolLen);
  if (!sePtr) return NULL;
  if (!*sePtr) {
    // we need to allocate a new SymbolEntry
    *sePtr =
      (SymbolEntry*)symbolAllocator->allocateNewStructure(sizeof(SymbolEntry));
  }
  return *sePtr;
};

Symbols::SymbolEntry *Symbols::getSymbol(const char *symbol,
  size_t symbolLen) {
  SymbolEntry **sePtr = (SymbolEntry **)hattrie_tryget(symbol2SymbolEntryMap,
                                                       symbol, symbolLen);
  if (!sePtr) return NULL;
  return *sePtr;
};
