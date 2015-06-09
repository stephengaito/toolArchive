#ifndef TOKENS_H
#define TOKENS_H

#include "hat-trie.h"
#include "blockAllocator.h"
#include "streamRegistry.h"
#include "varArray.h"

/// \brief A forest of Parse Trees over a collection of UTF8
/// characters.
///
/// *NOTE:* All of the UTF8 character streams which back these tokens
/// *should* be owned by and hence destroyed/freed by the
/// StreamRegistry associated with this forest of Parse Trees. To do
/// this each backing stream should be added to the ParseTrees instance.
class Token {
  public:

    /// \brief The token id for the parser.
    typedef value_t TokenId;

    /// \brief The token id wrapped with the ignore bit.
    typedef value_t WrappedTokenId;

    typedef VarArray<Token> TokenArray;

    /// \brief The Wrapped Token ID of a given token.
    WrappedTokenId wrappedId;

    /// \brief The start of the stream from which this token was parsed.
    const char *textStart;

    /// \brief The length of text from which this token was parsed.
    size_t      textLength;

    /// \brief The number of child tokens which make up this token.
    TokenArray tokens;

    Token(void) {
      wrappedId  = 0;
      textStart  = NULL;
      textLength = 0;
      // tokens will be implicitly initialized from VarArray()
    }

    /// \brief Destroy the forest of ParseTrees.
    ///
    /// *NOTE:* All UTF8 character streams added to this ParseTrees
    /// instance will be destroyed as well.  Which means, in turn,
    /// all of the C-strings associated with each Utf8Chars instance
    /// will potentially be freed as well (depending upon the explicit
    /// ownership registered with each Utf8Chars instance).
    ~Token(void) {
      wrappedId  = 0;
      textStart  = NULL;
      textLength = 0;
      // tokens will be implicilty deleted by ~VarArray();
     }

    Token *clone(void) {
      Token *token = new Token();
      token->wrappedId  = wrappedId;
      token->textStart  = textStart;
      token->textLength = textLength;
      token->tokens.copyFrom(tokens);
      return token;
    }

    void setText(const char *aTextStart, size_t aTextLength) {
      textStart  = aTextStart;
      textLength = aTextLength;
    }

    void setId(TokenId aTokenId) {
      wrappedId = aTokenId; // TODO WRAP THIS?
    }

    static bool ignoreToken(WrappedTokenId wrappedTokenId) {
      return wrappedTokenId & 0x1;
    }

    static TokenId unwrapToken(WrappedTokenId wrappedTokenId) {
      return wrappedTokenId >> 1;
    }

    static WrappedTokenId wrapToken(TokenId tokenId, bool ignoreToken) {
      return (( tokenId << 1 ) | ( ignoreToken ? 0x1 : 0x0));
    }

    void printOn(FILE *outFile, size_t indent = 0);
};

#endif
