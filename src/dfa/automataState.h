#ifndef AUTOMATA_STATE_H
#define AUTOMATA_STATE_H

#include "dfa/pdmTracer.h"

namespace DeterministicFiniteAutomaton {

  /// \brief The PushDownAutomata's state.
  class AutomataState {
    public:

      bool invariant1(void) const {
        if (allocator  == NULL) {
          if (dState   != NULL) return false;
          if (iterator != NULL) return false;
        }
        return true;
      }
      bool invariant2(void) const {
        if (dState != NULL) {
          if (iterator == NULL) return false;
          if (iterator->origDState != dState) return false;
        }
        return true;
      }
      bool invariant3(void) const {
        if ((stream  != NULL) && (!stream->invariant())) return false;
        return true;
      }
      bool invariant4(void) const {
        if ((token   != NULL) && (!token->invariant()))  return false;
        return true;
      }
      bool invariant5(void) const {
        if ((message != NULL) &&
            (!Utf8Chars::validUtf8Chars(message, strlen(message)))) return false;
        return true;
      }
      bool invariant(void) const {
        return (invariant1() &&
                invariant2() &&
                invariant3() &&
                invariant4() &&
                invariant5());
      }

      AutomataState(void) {
        allocator = NULL;
        iterator  = NULL;
        stream    = NULL;
        dState    = NULL;
        token     = NULL;
        message   = NULL;
        ASSERT_INVARIANT5;
      }

      void initialize(StateAllocator *anAllocator,
                      Utf8Chars      *aStream,
                      State          *aDState,
                      const char     *aMessage) {
        allocator = anAllocator;
        ASSERT(allocator);
        dState   = allocator->clone(aDState);
        iterator = allocator->getNewIteratorOn(dState);
        ASSERT(aStream);
        stream   = aStream->clone();
        token    = new Token();
        message  = strdup(aMessage);
        ASSERT_INVARIANT5;
      }

      void update(State *aDState,
                  const char *aMessage,
                  bool cloneToken    = false,
                  bool clearOldState = false) {
        ASSERT(allocator);
        ASSERT(aDState);
        if (clearOldState && dState)  allocator->unallocateState(dState);
        dState   = allocator->clone(aDState);

        if (clearOldState && iterator) delete iterator;
        if (dState) iterator = allocator->getNewIteratorOn(dState);

        Utf8Chars *oldStream = stream;
        if (stream) {
          stream   = stream->clone(!cloneToken);
          ASSERT(stream->invariant1());
          ASSERT(stream->invariant2());
          ASSERT(stream->invariant3());
        }
        if (clearOldState && oldStream) delete oldStream;

        Token *oldToken = token;
        if (cloneToken) {
          if (token) token = token->deepClone();
          else token = new Token();
        } else {
          token = new Token();
        }
        if (clearOldState && oldToken) delete oldToken;

        if (clearOldState && message) free((void*)message);
        message  = strdup(aMessage);
        ASSERT_INVARIANT5;
      }

      void copyFrom(const AutomataState &other,
                    bool keepStreamPosition = false,
                    bool clearOldState = true) {
        ASSERT_INVARIANT5;
        ASSERT(other.invariant1());
        ASSERT(other.invariant2());
        ASSERT(other.invariant3());
        ASSERT(other.invariant4());
        ASSERT(other.invariant5());
        ASSERT(allocator || other.allocator);
        if (!allocator) allocator = other.allocator;

        if (keepStreamPosition && other.stream) {
          other.stream->updatePositionFrom(stream);
        }
        if (clearOldState && stream) delete stream;
        stream   = other.stream;

        if (clearOldState && iterator) delete iterator;
        iterator = other.iterator;

        if (clearOldState && dState) allocator->unallocateState(dState);
        dState   = other.dState;

        if (clearOldState && token) delete token;
         token = other.token;

        if (clearOldState && message) free((void*)message);
        message = other.message;
        ASSERT_INVARIANT5;
      }

      void clear(void) {
        ASSERT_INVARIANT5;
        if (iterator) delete iterator;
        iterator  = NULL;
        if (stream)   delete stream;
        stream    = NULL;
        if (dState && allocator) allocator->unallocateState(dState);
        dState    = NULL;
        if (token)   delete token;
        token     = NULL;
        if (message)  free((void*)message);
        message   = NULL;
        allocator = NULL; // we do not own the allocator
      }

      NFAStateIterator *getIterator(void) {
        ASSERT_INVARIANT5;
        return iterator;
      }

      Utf8Chars *getStream(void) {
        ASSERT_INVARIANT5;
        return stream;
      }

      const char *getMessage(void) {
        ASSERT_INVARIANT5;
        return message;
      }

      void setMessage(const char *aMessage) {
        if (message) free((void*)message);
        message = strdup(aMessage);
        ASSERT_INVARIANT5;
      }

      State *getDState(void) {
        ASSERT_INVARIANT5;
        return dState;
      }
      void setDState(State *aDState) {
        ASSERT(allocator);
        ASSERT(aDState);
        dState = allocator->clone(aDState);
        iterator = allocator->getNewIteratorOn(dState);
        ASSERT_INVARIANT5;
      }
      void clearNFAState(NFA::State *nfaState) {
        ASSERT(allocator);
        ASSERT(nfaState);
        ASSERT(dState);
        allocator->clearNFAState(dState, nfaState);
        ASSERT_INVARIANT5;
      }
      NFA::State *stateMatchesToken(State *tokenStates) {
        ASSERT(allocator);
        ASSERT(tokenStates);
        ASSERT(dState);
        ASSERT_INVARIANT5;
        return allocator->stateMatchesToken(dState, tokenStates);
      }

      void setTokenId(Token::TokenId tokenId) {
        ASSERT(token);
        token->setId(tokenId);
        ASSERT_INVARIANT5;
      }

      void setTokenText(void) {
        ASSERT(token);
        ASSERT(stream);
        token->setText(stream->getStart(), stream->getNumberOfBytesRead());
        ASSERT_INVARIANT5;
      }

      void addChildToken(Token *childToken) {
        ASSERT(childToken->invariant());
        ASSERT(token);
        ASSERT(token->invariant());
        token->addChildToken(childToken);
        ASSERT_INVARIANT5;
      }

      Token *releaseToken(void) {
        ASSERT_INVARIANT5;
        Token *oldToken = token;
        token = new Token();
        ASSERT_INVARIANT5;
        return oldToken;
      }

    protected:

      void operator=(const AutomataState &other) {
        ASSERT(other.invariant1());
        ASSERT(other.invariant2());
        ASSERT(other.invariant3());
        allocator = other.allocator;
        iterator  = other.iterator;
        stream    = other.stream;
        dState    = other.dState;
        token     = other.token;
        message   = other.message;
        ASSERT_INVARIANT5;
      }

      /// \brief The allocator associated with this AutomataState.
      StateAllocator *allocator;

      /// \brief An iterator over a copy of the current DFA::State.
      ///
      /// When automata states are poped, the iterator is used
      /// to continue searching for viable alternative paths.
      NFAStateIterator *iterator;

      /// \brief The stream of UTF8 characters which have not yet
      /// been recognized.
      Utf8Chars *stream;

      /// \brief A copy of the current DFA::State.
      ///
      /// As each reStart NFA::State alternatives are tried,
      /// the corresponding NFA::State bits in this DFA::State
      /// are cleared, so that if no reStart NFA::State(s)
      /// succeed we can try to compute the next DFA:State
      /// using the non-reStart NFA::States.
      State *dState;

      /// \brief A copy of the currently partially constructed token
      Token *token;

      /// \brief A useful message used by the Tracer
      const char *message;

      friend class PDMTracer;
      friend class VarArray<AutomataState>;
  }; // class AutomataState
};  // namespace DeterministicFiniteAutomaton

#endif
