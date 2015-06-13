#ifndef AUTOMATA_STATE_H
#define AUTOMATA_STATE_H

#include "dfa/pdmTracer.h"

namespace DeterministicFiniteAutomaton {

  /// \brief The PushDownAutomata's state.
  class AutomataState {
    public:

      enum AutomataStateType {
        ASInvalid=0, ASCall=1, ASBackTrack=2
      };

      bool invariant(void) const {
        if (allocator  == NULL) {
          if (dState   != NULL) throw AssertionFailure("dstate not NULL");
          if (iterator != NULL) throw AssertionFailure("iterator not NULL");
        }
        if (dState != NULL) {
          if (iterator == NULL) throw AssertionFailure("iterator should not be NULL");
          if (iterator->origDState != dState)
            throw AssertionFailure("iterator on wrong dState");
        }
        if ((stream  != NULL) && (!stream->invariant()))
          throw AssertionFailure("stream failed invariant");
        if ((token   != NULL) && (!token->invariant()))
          throw AssertionFailure("token failed invariant");
        if ((message != NULL) &&
            (!Utf8Chars::validUtf8Chars(message, strlen(message))))
          throw AssertionFailure("message corrupted");
        return true;
      }

      AutomataState(void) {
        automataStateType = ASInvalid;
        startStateId      = 0;
        allocator         = NULL;
        iterator          = NULL;
        stream            = NULL;
        dState            = NULL;
        token             = NULL;
        message           = NULL;
        ASSERT(invariant());
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
        automataStateType = ASInvalid;
        startStateId      = 0;
        ASSERT(invariant());
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
          ASSERT(stream->invariant());
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
        ASSERT(invariant());
      }

      void copyFrom(const AutomataState &other,
                    bool keepStreamPosition = false,
                    bool clearOldState = true) {
        ASSERT(invariant());
        ASSERT(other.invariant());
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
        ASSERT(invariant());
      }

      void clear(void) {
        ASSERT(invariant());
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
        ASSERT(invariant());
        return iterator;
      }

      Utf8Chars *getStream(void) {
        ASSERT(invariant());
        return stream;
      }

      const char *getMessage(void) {
        ASSERT(invariant());
        return message;
      }

      void setMessage(const char *aMessage) {
        if (message) free((void*)message);
        message = strdup(aMessage);
        ASSERT(invariant());
      }

      State *getDState(void) {
        ASSERT(invariant());
        return dState;
      }
      void setDState(State *aDState) {
        ASSERT(allocator);
        ASSERT(aDState);
        dState = allocator->clone(aDState);
        iterator = allocator->getNewIteratorOn(dState);
        ASSERT(invariant());
      }
      void clearNFAState(NFA::State *nfaState) {
        ASSERT(allocator);
        ASSERT(nfaState);
        ASSERT(dState);
        allocator->clearNFAState(dState, nfaState);
        ASSERT(invariant());
      }
      NFA::State *stateMatchesToken(State *tokenStates) {
        ASSERT(allocator);
        ASSERT(tokenStates);
        ASSERT(dState);
        ASSERT(invariant());
        return allocator->stateMatchesToken(dState, tokenStates);
      }

      void setTokenId(Token::TokenId tokenId) {
        ASSERT(token);
        token->setId(tokenId);
        ASSERT(invariant());
      }

      void setTokenText(void) {
        ASSERT(token);
        ASSERT(stream);
        token->setText(stream->getStart(), stream->getNumberOfBytesRead());
        ASSERT(invariant());
      }

      void addChildToken(Token *childToken) {
        ASSERT(childToken->invariant());
        ASSERT(token);
        ASSERT(token->invariant());
        token->addChildToken(childToken);
        ASSERT(invariant());
      }

      Token *releaseToken(void) {
        ASSERT(invariant());
        Token *oldToken = token;
        token = new Token();
        ASSERT(invariant());
        return oldToken;
      }

    protected:

      void operator=(const AutomataState &other) {
        ASSERT(other.invariant());
        automataStateType = other.automataStateType;
        startStateId      = other.startStateId;
        message           = other.message;
        allocator         = other.allocator;
        dState            = other.dState;
        iterator          = other.iterator;
        stream            = other.stream;
        token             = other.token;
        ASSERT(invariant());
      }

      AutomataStateType automataStateType;

      NFA::StartStateId startStateId;

      /// \brief A useful message used by the Tracer
      const char *message;

      /// \brief The allocator associated with this AutomataState.
      StateAllocator *allocator;

      /// \brief A copy of the current DFA::State.
      ///
      /// As each reStart NFA::State alternatives are tried,
      /// the corresponding NFA::State bits in this DFA::State
      /// are cleared, so that if no reStart NFA::State(s)
      /// succeed we can try to compute the next DFA:State
      /// using the non-reStart NFA::States.
      State *dState;

      /// \brief An iterator over a copy of the current DFA::State.
      ///
      /// When automata states are poped, the iterator is used
      /// to continue searching for viable alternative paths.
      NFAStateIterator *iterator;

      /// \brief The stream of UTF8 characters which have not yet
      /// been recognized.
      Utf8Chars *stream;

      /// \brief A copy of the currently partially constructed token
      Token *token;

      friend class PDMTracer;
      friend class VarArray<AutomataState>;
  }; // class AutomataState
};  // namespace DeterministicFiniteAutomaton

#endif
