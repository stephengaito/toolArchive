#ifndef AUTOMATA_STATE_H
#define AUTOMATA_STATE_H

#include "dfa/pdmTracer.h"

namespace DeterministicFiniteAutomaton {

  /// \brief The PushDownAutomata's state.
  class AutomataState {
    public:

      AutomataState(void) {
        allocator = NULL;
        iterator  = NULL;
        stream    = NULL;
        dState    = NULL;
        tokens    = NULL;
        message   = NULL;
      }

      void initialize(StateAllocator *anAllocator,
                      Utf8Chars      *aStream,
                      State          *aDState,
                      const char     *aMessage) {
        allocator = anAllocator;
        dState   = allocator->clone(aDState);
        iterator = allocator->getNewIteratorOn(dState);
        stream   = aStream->clone();
        tokens   = new ParseTrees::TokenArray();
        message  = strdup(aMessage);
      }

      void update(State *aDState,
                  const char *aMessage,
                  bool clearOldState = false) {
        ASSERT(allocator);
        ASSERT(aDState);
        if (clearOldState && dState)  allocator->unallocateState(dState);
        dState   = allocator->clone(aDState);

        if (clearOldState && iterator) delete iterator;
        if (dState) iterator = allocator->getNewIteratorOn(dState);

        Utf8Chars *oldStream = stream;
        if (stream) stream   = stream->clone();
        if (clearOldState && oldStream) delete oldStream;

        if (clearOldState && tokens) delete tokens;
        tokens   = new ParseTrees::TokenArray();

        if (clearOldState && message) free((void*)message);
        message  = strdup(aMessage);
      }

      void copyFrom(const AutomataState &other,
                    bool keepStreamTokens = false,
                    bool clearOldState = true) {
        ASSERT(allocator || other.allocator);
        if (!allocator) allocator = other.allocator;

        if (!keepStreamTokens) {
          if (clearOldState && stream) delete stream;
          stream   = other.stream;
        }

        if (clearOldState && iterator) delete iterator;
        iterator = other.iterator;

        if (clearOldState && dState) allocator->unallocateState(dState);
        dState   = other.dState;

        if (!keepStreamTokens) {
          if (clearOldState && tokens) delete tokens;
          tokens = other.tokens;
        }

        if (clearOldState && message) free((void*)message);
        message = other.message;
      }

      void clear(void) {
        if (iterator) delete iterator;
        iterator  = NULL;
        if (stream)   delete stream;
        stream    = NULL;
        if (dState && allocator) allocator->unallocateState(dState);
        dState    = NULL;
        if (tokens)   delete tokens;
        tokens    = NULL;
        if (message)  free((void*)message);
        message   = NULL;
        allocator = NULL; // we do not own the allocator
      }

      NFAStateIterator *getIterator(void) { return iterator; }

      Utf8Chars *getStream(void) { return stream; }

      const char *getMessage(void) { return message; }
      void setMessage(const char *aMessage) {
        if (message) free((void*)message);
        message = strdup(aMessage);
      }

      State *getDState(void) { return dState; }
      void setDState(State *aDState) {
        dState = allocator->clone(aDState);
      }
      void clearNFAState(NFA::State *nfaState) {
        allocator->clearNFAState(dState, nfaState);
      }
      NFA::State *stateMatchesToken(State *tokenStates) {
        return allocator->stateMatchesToken(dState, tokenStates);
      }

      ParseTrees::TokenArray *getTokens(void) {
        return tokens;
      }

    private:

      void operator=(const AutomataState &other) {
        allocator = other.allocator;
        iterator  = other.iterator;
        stream    = other.stream;
        dState    = other.dState;
        tokens    = other.tokens;
        message   = other.message;
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

      /// \brief A copy of the current collection of parsed tokens
      ParseTrees::TokenArray *tokens;

      /// \brief A useful message used by the Tracer
      const char *message;

      friend class PDMTracer;
      friend class VarArray<AutomataState>;
  }; // class AutomataState
};  // namespace DeterministicFiniteAutomaton

#endif
