#ifndef SYMBOLS_H
#define SYMBOLS_H

#include <hat-trie.h>

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
    typedef struct SymbolEntry_struct {

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

  private:

    /// \brief The symbol to symbolEntry mapping
    hattrie_t *symbol2SymbolEntryMap;

    /// \brief The vector of SymbolEntry blocks from which new
    /// SymbolEntries are allocated.
    SymbolEntry **symbolEntries;

    /// \brief A pointer to the next SymbolEntry to be allocated.
    SymbolEntry *curSymbolEntry;

    /// \brief A pointer to the last SymbolEntry in the current block.
    SymbolEntry *lastSymbolEntry;

    /// \brief The size, in terms of SymbolEntries, of a block of
    /// SymbolEntries.
    size_t symbolEntryBlockSize;

    /// \brief The index of the current/next block of SymbolEntries to
    /// be allocated.
    size_t curSymbolEntryBlock;

    /// \brief The number of SymbolEntry block pointers in the vector
    /// of SymbolEntry blocks (symbolEntries).
    size_t numSymbolEntryBlocks;
};


#endif
