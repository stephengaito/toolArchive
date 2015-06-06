#ifndef PARSE_TREES_H
#define PARSE_TREES_H

#include "hat-trie.h"
#include "blockAllocator.h"
#include "streamRegistry.h"
#include "varArray.h"

#ifndef NUM_TOKEN_PTRS_PER_BLOCK
#define NUM_TOKEN_PTRS_PER_BLOCK 100
#endif

/// \brief A forest of Parse Trees over a collection of UTF8
/// characters.
///
/// *NOTE:* All of the UTF8 character streams which back these tokens
/// *should* be owned by and hence destroyed/freed by the
/// StreamRegistry associated with this forest of Parse Trees. To do
/// this each backing stream should be added to the ParseTrees instance.
class ParseTrees {
  public:

    /// \brief The token id for the parser.
    typedef value_t TokenId;

    /// \brief The token id wrapped with the ignore bit.
    typedef value_t WrappedTokenId;

    /// \brief The Token structre used by the parser.
    typedef struct Token {

      /// \brief The Wrapped Token ID of a given token.
      WrappedTokenId wrappedId;

      /// \brief The start of the stream from which this token was parsed.
      const char *textStart;

      /// \brief The length of text from which this token was parsed.
      size_t      textLength;

      /// \brief The number of child tokens which make up this token.
      size_t numTokens;

      /// \brief A possibly empty array of child tokens.
      struct Token *tokens[0];
    } Token;

    typedef VarArray<Token*> TokenArray;

    /// \brief Create a forest of ParseTrees.
    ParseTrees(void);

    /// \brief Destroy the forest of ParseTrees.
    ///
    /// *NOTE:* All UTF8 character streams added to this ParseTrees
    /// instance will be destroyed as well.  Which means, in turn,
    /// all of the C-strings associated with each Utf8Chars instance
    /// will potentially be freed as well (depending upon the explicit
    /// ownership registered with each Utf8Chars instance).
    ~ParseTrees(void);

    /// \brief Add a C-string as a stream over which tokens may be parsed.
    ///
    /// Internally a Utf8Chars instance will be created with the given
    /// ownership models. The associated C-string will be freed depending
    /// upon the given owernship model.
    void addStream(const char* someUtf8Chars,
                   Utf8Chars::Ownership ownership = Utf8Chars::DoNotOwn) {
      streams->addStream(someUtf8Chars, ownership);
    }

    /// \brief Add a Utf8Chars instance to the registry/collection of
    /// UTF8 character streams which back this forest of parse trees.
    void addStream(Utf8Chars *someUtf8Chars) {
      streams->addStream(someUtf8Chars);
    }

    /// \brief Allocate a new token with tokenId aTokenId.
    Token *allocateNewToken(WrappedTokenId aWrappedTokenId,
                            const char *textStart,
                            size_t textLength,
                            VarArray<Token*> &someTokens);

    bool ignoreToken(WrappedTokenId wrappedTokenId) {
      return wrappedTokenId & 0x1;
    }

    TokenId unwrapToken(WrappedTokenId wrappedTokenId) {
      return wrappedTokenId >> 1;
    }

    static WrappedTokenId wrapToken(TokenId tokenId, bool ignoreToken) {
      return (( tokenId << 1 ) | ( ignoreToken ? 0x1 : 0x0));
    }

  private:

    /// \brief The BlockAllocator which is used to allocate new tokens.
    BlockAllocator *tokenAllocator;

    /// \brief The registry/collection of UTF8 character streams over
    /// which these tokens are taken.
    StreamRegistry *streams;
};

#endif
