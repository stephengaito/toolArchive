#ifndef TOKENS_H
#define TOKENS_H

#include "hattrie/hat-trie.h"
#include "cUtils/blockAllocator.h"
#include "cUtils/varArray.h"
#include "dynUtf8Parser/streamRegistry.h"

/// \brief A Token which can contain a subTree of child tokens over a
/// collection of UTF8 characters.
///
/// *NOTE:* All of the UTF8 character streams which back these tokens
/// *should* be owned by and hence destroyed/freed by the same
/// StreamRegistry. To do this each backing stream should be added to the
/// StreamRegistry instance.
///
/// **TODO make TokenArray back into VarArray<Token> rather than the
/// less cache aware (less localized) VarArray<Token*>.**
class Token {
  public:

    /// \brief The token id for the token.
    typedef value_t TokenId;

    /// \brief The token id wrapped with the ignore bit.
    ///
    /// This WrappedTokenId is used by the NFA::State to reduce storage
    /// requirements.
    typedef value_t WrappedTokenId;

    /// \brief An invariant which should ALWAYS be true for any
    /// instance of a Token class.
    ///
    /// Throws an AssertionFailure with a brief description of any
    /// inconsistencies discovered.
    bool invariant(void) const {
      if (!tokens.invariant())
        throw AssertionFailure("child tokens failed invariant");
      if (!Utf8Chars::validUtf8Chars(textStart, textLength))
        throw AssertionFailure("token text not valid UTF8");
      return true;
    }

    /// \brief Construct a childless token with the TokenId and text
    /// provided.
    Token(TokenId aTokenId, const char *someText) {
      tokenId    = aTokenId;
      textStart  = someText;
      textLength = strlen(someText);
      ASSERT(invariant());
      //printf("token: %p new Token(TokenId, const char*)\n", this);
    }

    /// \brief Construct a childless token with no token it or text.
    Token(void) : tokens() {
      tokenId    = 0;
      textStart  = NULL;
      textLength = 0;
      ASSERT(invariant());
      //printf("token: %p new Token(void*)\n", this);
    }

    /// \brief Destroy the token and all of its subtrees of child tokens.
    ~Token(void) {
      //printf("token: %p delete Token(void)\n", this);
      ASSERT_INSIDE_DELETE(invariant());
      tokenId    = 0;
      textStart  = NULL;
      textLength = 0;
      tokens.~TokenArray();
     }

    /// \brief Clone the token (providing a *deep* copy of all subtrees
    /// of child tokens).
    Token *clone(void) {
      Token *token = new Token();
      token->copyFrom(this);
      return token;
    }

    /// \brief Set the text of this token.
    void setText(const char *aTextStart, size_t aTextLength) {
      textStart  = aTextStart;
      textLength = aTextLength;
      ASSERT(invariant());
    }

    /// \brief Set the token id of this token.
    void setId(TokenId aTokenId) {
      tokenId = aTokenId;
    }

    /// \brief Add a Child token (making a *deep* copy of all of the
    /// child token's subtrees of subchild tokens).
    void addChildToken(Token *childToken) {
      ASSERT(childToken->invariant());
      tokens.pushItem(childToken->clone());
      ASSERT(invariant());
    }

    /// \brief Wrap the ignoreToken flag into the TokenId provided.
    static WrappedTokenId wrapTokenId(TokenId aTokenId, bool ignoreToken) {
      return (( aTokenId << 1 ) | ( ignoreToken ? 0x1 : 0x0));
    }

    /// \brief UnWrap the ignoreToken flag from the WrappedTokenId provided.
    static bool ignoreToken(WrappedTokenId wrappedId) {
      return wrappedId & 0x1;
    }

    /// \brief UnWrap the TokenId from the WrappedTokenId provided.
    static TokenId unWrapTokenId(WrappedTokenId wrappedId) {
      return wrappedId >> 1;
    }

    /// \brief Print the token on the FILE* provided.
    void printOn(FILE *outFile, size_t indent = 0);

#define ASSERT_EQUALS(tokenId, someChars) assertEquals(tokenId, someChars, __FILE__, __LINE__)

    /// \brief A simple helper method used by the tests to assert that
    /// a given token is has the TokenId and text provided.
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

    /// \brief Returns true if the token has one or more child tokens.
    bool hasChildren(size_t numChildren) {
      if (tokens.getNumItems() != numChildren) return false;
      return true;
    }

  protected:

    /// \brief Make a *deep* copy of the other token.
    void copyFrom(const Token *other) {
      ASSERT(other->invariant());
      tokenId    = other->tokenId;
      textStart  = other->textStart;
      textLength = other->textLength;
      tokens     = other->tokens;
      ASSERT(invariant());
    }

    /// \brief Make a *deep* copy of the other token.
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


    /// \brief The TokenArray class holds the collection of child tokens.
    class TokenArray : public VarArray<Token*> {
    public:

      /// \brief An invariant which should ALWAYS be true for any
      /// instance of a TokenArray class.
      ///
      /// Throws an AssertionFailure with a brief description of any
      /// inconsistencies discovered.
      bool invariant(void) const {
        if (!VarArray<Token*>::invariant())
          throw AssertionFailure("TokenArray VarArray invariant failed");

        for (size_t i = 0 ; i < numItems; i++) {
          if (!itemArray[i]->invariant())
            throw AssertionFailure("Token failed invariant");
        }
        return true;
      }

      /// \brief Delete all child tokens.
      ~TokenArray(void) {
        ASSERT_INSIDE_DELETE(invariant());
        for (size_t i = 0; i < numItems; i++) {
          delete itemArray[i];
        }
        VarArray::~VarArray();
      }

      /// \brief Make a *deep* copy of the token array ensuring any old
      /// tokens are properly deleted.
      void operator=(const TokenArray &other) {
        ASSERT(other.invariant());
        // start by correctly getting rid of any existing tokens
        for (size_t i = 0; i < numItems; i++) {
          delete itemArray[i];
        }
        // now there are no more items....
        numItems  = 0;
        for (size_t i = 0; i < other.numItems; i++) {
          // MAKE SURE WE HAVE A DEEP COPY BY CLONING
          pushItem(other.itemArray[i]->clone());
        }
        ASSERT(invariant());
      }

      /// \brief Print the child tokens on the FILE* provided.
      void printOn(FILE *outFile, size_t indent);

    };

    /// \brief The number of child tokens which make up this token.
    TokenArray tokens;
};

#endif
