#ifndef AUTOMATA_STATE_H
#define AUTOMATA_STATE_H

#include "dfa/pdmTracer.h"

namespace DeterministicFiniteAutomaton {

  /// \brief The PushDownAutomata's state.
  class AutomataState {
    public:

      enum AutomataStateType {
        ASInvalid=0, ASBackTrack=1, ASContinue=2, ASRestart=3
      };

      bool invariant(void) const {
        if (dfa == NULL) {
          if (allocator != NULL) throw AssertionFailure("allocator not NULL when dfa is NULL");
        }
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
        ASSERT(invariant());
      }

      void initialize(DFA               *aDFA,
                      Utf8Chars         *aStream,
                      NFA::StartStateId  aStartStateId) {
        dfa = aDFA;
        ASSERT(dfa);
        allocator = dfa->getStateAllocator();
        ASSERT(allocator);
        setStartStateId(aStartStateId);
        ASSERT(aStream);
        stream   = aStream->clone();
        token    = new Token();
        automataStateType = ASRestart;
        ASSERT(invariant());
      }

      void setStartStateId(NFA::StartStateId aStartStateId) {
        startStateId = aStartStateId;
        ASSERT(dfa);
        setDState(dfa->getDFAStartState(startStateId));
        ASSERT(invariant());
      }

      void setDState(State *aDState, bool clearOldState = false) {
        ASSERT(allocator);
        if (clearOldState && dState)  allocator->unallocateState(dState);
        dState   = allocator->clone(aDState);

        if (clearOldState && iterator) delete iterator;
        if (dState) iterator = allocator->getNewIteratorOn(dState);
        ASSERT(invariant());
      }

      void cloneSubStream(bool subStream, bool clearOldState = false) {
        Utf8Chars *oldStream = stream;
        if (stream) {
          stream   = stream->clone(subStream);
          ASSERT(stream->invariant());
        }
        if (clearOldState && oldStream) delete oldStream;
        ASSERT(invariant());
      }

      void cloneToken(bool shouldCloneToken, bool clearOldState = false) {
        Token *oldToken = token;
        if (shouldCloneToken) {
          if (token) token = token->clone();
          else token = new Token();
        } else {
          token = new Token();
        }
        if (clearOldState && oldToken) delete oldToken;
        ASSERT(invariant());
      }

      void copyFrom(const AutomataState &other,
                    bool keepStreamPosition = false,
                    bool clearOldState = true) {
        ASSERT(invariant());
        ASSERT(other.invariant());

        automataStateType = other.automataStateType;
        startStateId      = other.startStateId;

        ASSERT(dfa       || other.dfa);
        if (!dfa) dfa = other.dfa;

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
        allocator = NULL; // we do not own the allocator
        dfa       = NULL; // we do not own the DFA
      }

      NFAStateIterator *getIterator(void) {
        ASSERT(invariant());
        return iterator;
      }

      Utf8Chars *getStream(void) {
        ASSERT(invariant());
        return stream;
      }

      void setStateType(AutomataStateType stateType) {
        automataStateType = stateType;
      }
      AutomataStateType getStateType(void) {
        return automataStateType;
      }

      static const char *getStateTypeMessage(AutomataStateType stateType) {
        switch(stateType) {
        case ASBackTrack:
          return "BackTrack";
        case ASContinue:
          return "Continue";
        case ASRestart:
          return "Restart";
        default:
          return "INVALID";
        }
        ASSERT_MESSAGE(false, "should never reach here");
      }

      const char *getStateTypeMessage(void) {
        ASSERT(invariant());
        return getStateTypeMessage(automataStateType);
      }

      const char *getStartStateMessage(void) {
        ASSERT(dfa);
        NFA *nfa = dfa->getNFA();
        ASSERT(nfa);
        NFA::State *nfaState = nfa->getStartState(startStateId);
        ASSERT(nfaState);
        return nfaState->message;
      }

      State *getDState(void) {
        ASSERT(invariant());
        return dState;
      }

      void clearNFAState(NFA::State *nfaState) {
        ASSERT(allocator);
        ASSERT(nfaState);
        ASSERT(dState);
        allocator->clearNFAState(dState, nfaState);
        ASSERT(invariant());
      }
      void clearNFAStatesWithSameRestartState(NFA::StartStateId aStartStateId) {
        NFAStateIterator iterator = allocator->newIteratorOn(dState);
        while (NFA::State *nfaState = iterator.nextState()) {
          if ((nfaState->matchType == NFA::ReStart) &&
              (nfaState->matchData.r == aStartStateId)) {
            clearNFAState(nfaState);
          }
        }
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
        dfa               = other.dfa;
        allocator         = other.allocator;
        dState            = other.dState;
        iterator          = other.iterator;
        stream            = other.stream;
        token             = other.token;
        ASSERT(invariant());
      }

      AutomataStateType automataStateType;

      NFA::StartStateId startStateId;

      /// \brief The allocator associated with this AutomataState.
      DFA *dfa;

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
