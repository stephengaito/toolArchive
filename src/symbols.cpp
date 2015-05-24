#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "symbols.h"


Symbols::Symbols(void) {
  symbol2SymbolEntryMap = hattrie_create();
  curSymbolEntry        = NULL;
  lastSymbolEntry       = NULL;
  symbolEntryBlockSize  = 20;
  curSymbolEntryBlock   = 0;
  numSymbolEntryBlocks  = 0;
};

Symbols::~Symbols(void) {
  if (symbol2SymbolEntryMap) hattrie_free(symbol2SymbolEntryMap);
  symbolEntries        = NULL;
  curSymbolEntry       = NULL;
  lastSymbolEntry      = NULL;
  symbolEntryBlockSize = 20;
  curSymbolEntryBlock  = 0;
  numSymbolEntryBlocks = 0;
};


Symbols::SymbolEntry *Symbols::registerSymbol(const char *symbol, size_t symbolLen) {
  SymbolEntry **sePtr = (SymbolEntry **)hattrie_get(symbol2SymbolEntryMap,
                                                    symbol, symbolLen);
  if (!sePtr) return NULL;
  if (!*sePtr) {
    // we need to allocate a new SymbolEntry
    if (lastSymbolEntry <= curSymbolEntry) {
      // we need to allocate a new block of SymbolEntries
      if (numSymbolEntryBlocks <= curSymbolEntryBlock) {
        // we need to expand the vector of SymbolEntry blocks
        SymbolEntry **oldEntries = symbolEntries;
        symbolEntries = (SymbolEntry**) calloc(numSymbolEntryBlocks + 10,
                                               sizeof(SymbolEntry*));
        if (oldEntries) {
          memcpy(symbolEntries, oldEntries,
            numSymbolEntryBlocks*sizeof(SymbolEntry*));
        }
        numSymbolEntryBlocks += 10;
      }
      symbolEntries[curSymbolEntryBlock] =
        (SymbolEntry*) calloc(symbolEntryBlockSize, sizeof(SymbolEntry));
      curSymbolEntry = symbolEntries[curSymbolEntryBlock];
      lastSymbolEntry = curSymbolEntry +
        symbolEntryBlockSize*sizeof(SymbolEntry);
      curSymbolEntryBlock++;
    }
    *sePtr = curSymbolEntry;
    curSymbolEntry++;
  }
  return *sePtr;
};

Symbols::SymbolEntry *Symbols::getSymbol(const char *symbol, size_t symbolLen) {
  SymbolEntry **sePtr = (SymbolEntry **)hattrie_tryget(symbol2SymbolEntryMap,
                                                       symbol, symbolLen);
  if (!sePtr) return NULL;
  return *sePtr;
};
