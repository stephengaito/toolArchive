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
///
/// **TODO make TokenArray back into VarArray<Token> rather than the less
/// cache aware (less localized) VarArray<Token*>.**
class Token {
  public:

    /// \brief The token id for the parser.
    typedef value_t TokenId;

    /// \brief The token id wrapped with the ignore bit.
    typedef value_t WrappedTokenId;

    bool invariant(void) const {
      if (!tokens.invariant())
        throw AssertionFailure("child tokens failed invariant");
      if (!Utf8Chars::validUtf8Chars(textStart, textLength))
        throw AssertionFailure("token text not valid UTF8");
      return true;
    }

    Token(TokenId aTokenId, const char *someText) {
      tokenId    = aTokenId;
      textStart  = someText;
      textLength = strlen(someText);
      ASSERT(invariant());
    }

    Token(void) : tokens() {
      tokenId    = 0;
      textStart  = NULL;
      textLength = 0;
      ASSERT(invariant());
    }

    /// \brief Destroy the forest of ParseTrees.
    ///
    /// *NOTE:* All UTF8 character streams added to this ParseTrees
    /// instance will be destroyed as well.  Which means, in turn,
    /// all of the C-strings associated with each Utf8Chars instance
    /// will potentially be freed as well (depending upon the explicit
    /// ownership registered with each Utf8Chars instance).
    ~Token(void) {
      ASSERT(invariant());
      tokenId    = 0;
      textStart  = NULL;
      textLength = 0;
      tokens.~TokenArray();
     }

    Token *clone(void) {
      Token *token = new Token();
      token->copyFrom(this);
      return token;
    }

    void setText(const char *aTextStart, size_t aTextLength) {
      textStart  = aTextStart;
      textLength = aTextLength;
      ASSERT(invariant());
    }

    void setId(TokenId aTokenId) {
      tokenId = aTokenId;
    }

    void addChildToken(Token *childToken) {
      ASSERT(childToken->invariant());
      tokens.pushItem(childToken);
      ASSERT(invariant());
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

#define ASSERT_EQUALS(tokenId, someChars) assertEquals(tokenId, someChars, __FILE__, __LINE__)

    bool assertEquals(TokenId aTokenId,
                      const char *someChars,
                      const char *filename,
                      const unsigned int linenumber) {
      if (tokenId != aTokenId)
        throw AssertionFailure("incorrect tokenId", filename, linenumber);
      if (textLength != strlen(someChars))
        throw AssertionFailure("incorrect text length", filename, linenumber);
      if (strncmp(textStart, someChars, textLength) != 0)
        throw AssertionFailure("incorrect text", filename, linenumber);
      return true;
    }

    bool hasChildren(size_t numChildren) {
      if (tokens.getNumItems() != numChildren) return false;
      return true;
    }

  protected:

    void copyFrom(const Token *other) {
      ASSERT(other->invariant());
      tokenId    = other->tokenId;
      textStart  = other->textStart;
      textLength = other->textLength;
      tokens     = other->tokens;
      ASSERT(invariant());
    }

    void operator=(const Token &other) {
      ASSERT(other.invariant());
      tokenId    = other.tokenId;
      textStart  = other.textStart;
      textLength = other.textLength;
      tokens     = other.tokens;
      ASSERT(invariant());
    }

    /// \brief The Wrapped Token ID of a given token.
    TokenId tokenId;

    /// \brief The start of the stream from which this token was parsed.
    const char *textStart;

    /// \brief The length of text from which this token was parsed.
    size_t      textLength;

    class TokenArray : public VarArray<Token*> {
      public:
       bool invariant(void) const {
         if (!VarArray<Token*>::invariant())
           throw AssertionFailure("TokenArray VarArray invariant failed");

         for (size_t i = 0 ; i < numItems; i++) {
           if (!itemArray[i]->invariant())
             throw AssertionFailure("Token failed invariant");
         }
         return true;
       }
    };

    /// \brief The number of child tokens which make up this token.
    TokenArray tokens;

    friend class VarArray<Token>;
};

#endif
