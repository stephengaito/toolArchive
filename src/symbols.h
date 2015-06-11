#ifndef SYMBOLS_H
#define SYMBOLS_H

#include <hat-trie.h>
#include "blockAllocator.h"

/// \brief The Symbols class is used to ensure that symbols are
/// unique across the system.
///
///
class Symbols {
  public:

    /// \brief Create a Symbols object
    Symbols(void);

    /// \brief Destroy a Symbols object
    ~Symbols(void);

    /// \brief A SymbolEntry captures all the information required by
    /// the system  about a particular symbol.
    typedef struct SymbolEntry {

    } SymbolEntry;

    /// \brief Register a new symbol.
    ///
    /// Returns a SymbolEntry for the given symbol.
    /// If the symbol has already been registered, returns the
    /// already registered SymbolEntry.
    SymbolEntry *registerSymbol(const char *symbol, size_t symbolLen);

    /// \brief Get the SymbolEntry for a symbol
    ///
    /// Returns the SymbolEntry associated with the given symbol.
    /// If the symbol has never been registered, returns NULL.
    SymbolEntry *getSymbol(const char * symbol, size_t symbolLen);

  protected:

    /// \brief The symbol to symbolEntry mapping
    hattrie_t *symbol2SymbolEntryMap;

    /// \brief A BlockAllocator which allocates new SymbolEntry
    /// structures.
    BlockAllocator *symbolAllocator;
};


#endif
