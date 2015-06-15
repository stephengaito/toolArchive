#ifndef AUTOMATA_STATE_H
#define AUTOMATA_STATE_H

#include "dfa/pdmTracer.h"

namespace DeterministicFiniteAutomaton {

  /// \brief The PushDownMachine's state.
  class AutomataState {
    public:

      /// \brief The type of the current AutomataState.
      enum AutomataStateType {
        ASInvalid=0,
        ASBackTrack=1,
        ASContinue=2,
        ASRestart=3
      };

      /// \brief An invariant which should ALWAYS be true for any
      /// instance of a AutomataState class.
      ///
      /// Throws an AssertionFailure with a brief description of any
      /// inconsistencies discovered.
      bool invariant(void) const {
        if (dfa == NULL) {
          if (allocator != NULL)
            throw AssertionFailure("allocator not NULL when dfa is NULL");
        }
        if (allocator  == NULL) {
          if (dState   != NULL) throw AssertionFailure("dstate not NULL");
          if (iterator != NULL) throw AssertionFailure("iterator not NULL");
        }
        if (dState != NULL) {
          if (iterator == NULL)
            throw AssertionFailure("iterator should not be NULL");
          if (iterator->origDState != dState)
            throw AssertionFailure("iterator on wrong dState");
        }
        if ((stream  != NULL) && (!stream->invariant()))
          throw AssertionFailure("stream failed invariant");
        if ((token   != NULL) && (!token->invariant()))
          throw AssertionFailure("token failed invariant");
        return true;
      }

      /// \brief Create an empty AutomataState.
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

      /// \brief Initialize an existing AutomataState to the start
      /// state, aStartStateId, over the DFA, aDFA, running over the
      /// stream, aStream, of UTF8 characters.
      void initialize(DFA               *aDFA,
                      Utf8Chars         *aStream,
                      NFA::StartStateId  aStartStateId) {
        // TODO: should any old state be deleted?
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

      /// \brief Set the AutomataState to the start state, aStartStateId.
      void setStartStateId(NFA::StartStateId aStartStateId) {
        startStateId = aStartStateId;
        ASSERT(dfa);
        setDState(dfa->getDFAStartState(startStateId));
        ASSERT(invariant());
      }

      /// \brief Set the AutomataState to the the DFA State provided,
      /// clearing the old state if clearOldState is true.
      void setDState(State *aDState, bool clearOldState = false) {
        ASSERT(allocator);
        if (clearOldState && dState)  allocator->unallocateState(dState);
        dState   = allocator->clone(aDState);

        if (clearOldState && iterator) delete iterator;
        if (dState) iterator = allocator->getNewIteratorOn(dState);
        ASSERT(invariant());
      }

      /// \brief Clone the stream. If subStream is true then the cloned
      /// stream will only read from the parent stream's current position.
      ///
      /// Clear the old state if clearOldState is true.
      void cloneSubStream(bool subStream, bool clearOldState = false) {
        Utf8Chars *oldStream = stream;
        if (stream) {
          stream   = stream->clone(subStream);
          ASSERT(stream->invariant());
        }
        if (clearOldState && oldStream) delete oldStream;
        ASSERT(invariant());
      }

      /// \brief Clone the token. Simply create a new token if
      /// shouldCLoneToken is false.
      ///
      /// Clear the old state if clearOldState is true.
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

      /// \brief Copy the current AutomataState's state from the other
      /// AutomataState's state.
      ///
      /// The current AutomataState's stream position will kept if
      /// keepStreamPosition is true.
      ///
      /// Clear the old state if clearOldState is true.
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

      /// \brief Clear the AutomataState's state.
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

      /// \brief Get DFA state iterator associated with this AutomataState.
      NFAStateIterator *getIterator(void) {
        ASSERT(invariant());
        return iterator;
      }

      /// \brief Get the stream associated with this AutomataState.
      Utf8Chars *getStream(void) {
        ASSERT(invariant());
        return stream;
      }

      /// \brief Explicitly set the AutomataState's state type.
      void setStateType(AutomataStateType stateType) {
        automataStateType = stateType;
      }
      AutomataStateType getStateType(void) {
        return automataStateType;
      }

      /// \brief Return a textural description (for use by the
      /// PDMTracer) corresponding to the stateType provided.
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

      /// \brief Return a textural description (for use by the
      /// PDMTracer) of this AutomataState's state type.
      const char *getStateTypeMessage(void) {
        ASSERT(invariant());
        return getStateTypeMessage(automataStateType);
      }

      /// \brief Get the textural name/description of this
      /// AutomataState's start state.
      const char *getStartStateMessage(void) {
        ASSERT(dfa);
        NFA *nfa = dfa->getNFA();
        ASSERT(nfa);
        NFA::State *nfaState = nfa->getStartState(startStateId);
        ASSERT(nfaState);
        return nfaState->message;
      }

      /// \brief Ge the DFA State associated with this AutomataState.
      State *getDState(void) {
        ASSERT(invariant());
        return dState;
      }

      /// \brief Clear the NFA state out of this AutomataState's DFA
      /// State.
      void clearNFAState(NFA::State *nfaState) {
        ASSERT(allocator);
        ASSERT(nfaState);
        ASSERT(dState);
        allocator->clearNFAState(dState, nfaState);
        ASSERT(invariant());
      }

      /// \brief Clear all NFA states which have the same ReStart
      /// stateStateId out of this AutomataState's DFA State.
      void clearNFAStatesWithSameRestartState(NFA::StartStateId aStartStateId) {
        NFAStateIterator iterator = allocator->newIteratorOn(dState);
        while (NFA::State *nfaState = iterator.nextState()) {
          if ((nfaState->matchType == NFA::ReStart) &&
              (nfaState->matchData.r == aStartStateId)) {
            clearNFAState(nfaState);
          }
        }
      }

      /// \brief Return the first NFA::State which matches the
      /// tokenStates provided.
      NFA::State *stateMatchesToken(State *tokenStates) {
        ASSERT(allocator);
        ASSERT(tokenStates);
        ASSERT(dState);
        ASSERT(invariant());
        return allocator->stateMatchesToken(dState, tokenStates);
      }

      /// \brief Explicitly set the TokenId of this AutomataState's
      /// token.
      void setTokenId(Token::TokenId tokenId) {
        ASSERT(token);
        token->setId(tokenId);
        ASSERT(invariant());
      }

      /// \brief Explicitly set the text of this AutomataState's token.
      void setTokenText(void) {
        ASSERT(token);
        ASSERT(stream);
        token->setText(stream->getStart(), stream->getNumberOfBytesRead());
        ASSERT(invariant());
      }

      /// \brief Add a child token to this AutomataState's token.
      void addChildToken(Token *childToken) {
        ASSERT(childToken->invariant());
        ASSERT(token);
        ASSERT(token->invariant());
        token->addChildToken(childToken);
        ASSERT(invariant());
      }

      /// \brief Remove and return this AutomataState's token.
      ///
      /// On return this AutomataState will have a new token instance.
      Token *releaseToken(void) {
        ASSERT(invariant());
        Token *oldToken = token;
        token = new Token();
        ASSERT(invariant());
        return oldToken;
      }

    protected:

      /// \brief Copy the other AutomataState to this one.
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

      /// \brief The type of AutomataState.
      AutomataStateType automataStateType;

      /// \brief The (original) state state id for this AutomataState.
      NFA::StartStateId startStateId;

      /// \brief The DFA associated with this AutomataState.
      DFA *dfa;

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
