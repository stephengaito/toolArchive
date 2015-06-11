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

    bool invariant(void) const {
      if (!tokens.invariant()) return false;
      return Utf8Chars::validUtf8Chars(textStart, textLength);
    }

    Token(void) : tokens() {
      tokenId    = 0;
      textStart  = NULL;
      textLength = 0;
      ASSERT_INVARIANT;
    }

    /// \brief Destroy the forest of ParseTrees.
    ///
    /// *NOTE:* All UTF8 character streams added to this ParseTrees
    /// instance will be destroyed as well.  Which means, in turn,
    /// all of the C-strings associated with each Utf8Chars instance
    /// will potentially be freed as well (depending upon the explicit
    /// ownership registered with each Utf8Chars instance).
    ~Token(void) {
      ASSERT_INVARIANT;
      tokenId    = 0;
      textStart  = NULL;
      textLength = 0;
      tokens.~TokenArray();
     }

//    void shallowCopyFrom(const Token &other) {
//      tokenId    = other.tokenId;
//      textStart  = other.textStart;
//      textLength = other.textLength;
//      tokens.shallowCopyFrom(other.tokens);
//    }

//    Token *shallowClone(void) {
//      Token *token = new Token();
//      token->shallowCopyFrom(*this);
//      return token;
//    }

    void deepCopyFrom(const Token &other) {
      ASSERT(other.invariant());
      Token nullToken;
      tokenId    = other.tokenId;
      textStart  = other.textStart;
      textLength = other.textLength;
      tokens.deepCopyFrom(other.tokens);
      ASSERT_INVARIANT;
    }

    Token *deepClone(void) {
      Token *token = new Token();
      token->deepCopyFrom(*this);
      return token;
    }

    void setText(const char *aTextStart, size_t aTextLength) {
      textStart  = aTextStart;
      textLength = aTextLength;
      ASSERT_INVARIANT;
    }

    void setId(TokenId aTokenId) {
      tokenId = aTokenId;
    }

    void addChildToken(Token *childToken) {
      ASSERT(childToken->invariant());
      tokens.pushItem(*childToken);
      ASSERT_INVARIANT;
    }

    static WrappedTokenId wrapTokenId(TokenId aTokenId, bool ignoreToken) {
      return (( aTokenId << 1 ) | ( ignoreToken ? 0x1 : 0x0));
    }

    static bool ignoreToken(WrappedTokenId wrappedId) {
      return wrappedId & 0x1;
    }

    static TokenId unWrapTokenId(WrappedTokenId wrappedId) {
      return wrappedId >> 1;
    }

    void printOn(FILE *outFile, size_t indent = 0);

  protected:

    void operator=(const Token &other) {
      ASSERT(other.invariant());
      tokenId    = other.tokenId;
      textStart  = other.textStart;
      textLength = other.textLength;
      tokens     = other.tokens;
      ASSERT_INVARIANT;
    }

    /// \brief The Wrapped Token ID of a given token.
    TokenId tokenId;

    /// \brief The start of the stream from which this token was parsed.
    const char *textStart;

    /// \brief The length of text from which this token was parsed.
    size_t      textLength;

    class TokenArray : public VarArray<Token> {
      public:
       bool invariant(void) const {
         if (!VarArray<Token>::invariant()) return false;

         for (size_t i = 0 ; i < numItems; i++) {
           if (!itemArray[i].invariant()) return false;
         }
         return true;
       }

      void operator=(const TokenArray &other) {
        ASSERT(other.invariant());
        numItems  = 0;
        arraySize = 0;
        if (itemArray) free(itemArray);
        itemArray = NULL;
        for (size_t i = 0; i < numItems; i++) {
          pushItem(other.itemArray[i]);
        }
        ASSERT_INVARIANT;
      }
    };

    /// \brief The number of child tokens which make up this token.
    TokenArray tokens;

    friend class VarArray<Token>;
};

#endif
