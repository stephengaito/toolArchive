#ifndef PUSH_DOWN_MACHINE_H
#define PUSH_DOWN_MACHINE_H

#include "dfa/dfa.h"

namespace DeterministicFiniteAutomaton {

  /// \brief A PushDownMachine object is used to run a DFA.
  ///
  /// It can be run from any registered start state using
  /// any Utf8Chars.
  class PushDownMachine {

    public:
      /// \brief A PushDownMachine::Tracer object is used to trace a
      /// given PushDownMachine's state transitions.
      class Tracer {

        public:

          /// \brief Create a new PushDownMachine instance.
          Tracer(const char *aMessage, FILE *aTraceFile = NULL) {
            pdm       = NULL;
            message   = aMessage;
            traceFile = aTraceFile;
          }

          /// \brief Destroy the tracer.
          ~Tracer(void) {
            pdm       = NULL; // we do not own the PDM
            message   = NULL; // we do not own the message
            traceFile = NULL; // we do not own the FILE
          }

          /// \brief Sets the associated PushDownMachine
          void setPDM(PushDownMachine *aPDM) {
            pdm = aPDM;
            fprintf(traceFile, "PDMTracer: %s\n", message);
          }

          void reportState(size_t indent = 0);

          void reportAutomataStack(size_t indent = 0);

          void reportNFAState(NFA::State *nfaState, size_t indent = 0);

          void reportDFAState(size_t indent = 0);

          void reportChar(utf8Char_t curChar, size_t indent = 0);

          void reportStreamPrefix();

          void reportStreamPostfix();

          void swap(size_t indent = 0);

          void push(size_t indent = 0);

          void pop(bool keepStream, size_t indent = 0);

          void checkForRestart(size_t indent = 0);

          /// \brief Trace the use of a restart state transition.
          void restart(NFA::State *nfaState, size_t indent = 0);

          void match(NFA::State *nfaState, size_t indent = 0);

          void done(size_t indent = 0);

          void failedWithStream(size_t indent = 0);

          void nextDFAState(size_t indent = 0);

          void failedBacktrack(size_t indent = 0);

          void backtrack(size_t indent = 0);

          void error(size_t indent = 0);

        private:

          /// \brief The currently associated PushDownMachine.
          ///
          /// Used to access the internal state of the PDM.
          PushDownMachine *pdm;

          /// \brief A message associated with this Tracer.
          const char *message;

          /// \brief Whether or not to trace the state transitions.
          FILE *traceFile;

      }; // class Tracer

      /// \brief Create a new PushDownMachine instance.
      PushDownMachine(DFA *aDFA) {
        dfa        = aDFA;
        nfa        = dfa->getNFA();
        allocator  = dfa->getStateAllocator();
        parseTrees = dfa->getParseTrees();
      }

      /// \brief Run the PushDownAutomata from any the given start
      /// state using the Utf8Chars stream provided.
      ParseTrees::Token *runFromUsing(const char *startStateName,
                                      Utf8Chars *charStream,
                                      PushDownMachine::Tracer *pdmTracer = NULL) {
        return runFromUsing(nfa->findStartStateId(startStateName),
                            charStream, pdmTracer);
      }

      /// \brief Run the PushDownAutomata from any the given start
      /// state using the Utf8Chars stream provided.
      ParseTrees::Token *runFromUsing(NFA::StartStateId startStateId,
                                      Utf8Chars *charStream,
                                      PushDownMachine::Tracer *pdmTracer = NULL);

    private:

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

          AutomataState(StateAllocator         *anAllocator,
                        Utf8Chars              *aStream,
                        State                  *aDState,
                        const char             *aMessage) {
            allocator = anAllocator;
            dState   = allocator->clone(aDState);
            iterator = allocator->getNewIteratorOn(dState);
            stream   = aStream->clone();
            tokens   = new ParseTrees::TokenArray();
            message  = strdup(aMessage);
          }

          void operator=(const AutomataState &other) {
            allocator = other.allocator;
            iterator  = other.iterator;
            stream    = other.stream;
            dState    = other.dState;
            tokens    = other.tokens;
            message   = other.message;
          }

          void update(State *aDState,
                      const char *aMessage,
                      bool clearOldState = false) {
            // TODO: CHECK MEMORY LEAK
            if (clearOldState && dState) allocator->unallocateState(dState);
            dState   = allocator->clone(aDState);

            if (clearOldState && iterator) delete iterator;
            iterator = allocator->getNewIteratorOn(dState);

            Utf8Chars *oldStream = stream;
            stream   = stream->clone();
            if (clearOldState && stream) delete oldStream;

            if (clearOldState && tokens) delete tokens;
            tokens   = new ParseTrees::TokenArray();

            if (clearOldState && message) free((void*)message);
            message  = strdup(aMessage);
          }

          void copyFrom(AutomataState &other, bool keepStream = false) {
            // TODO: CHECK MEMORY LEAK
            if (!keepStream) {
              if (stream) delete stream;
              stream   = other.stream;
            }

            if (dState) allocator->unallocateState(dState);
            dState   = other.dState;

            if (tokens) delete tokens;
            tokens = other.tokens;

            if (message) free((void*)message);
            message = other.message;
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

          friend class Tracer;
      };

      /// \brief Push the current automata state on to the top
      /// of the push down automata's state stack.
      void push(Tracer *pdmTracer, State *aDState, const char *message) {
        if (pdmTracer) pdmTracer->push();
        stack.pushItem(curState);
        curState.update(aDState, message);
      }

      /// \brief Swap the top two elements of the AutomataState stack.
      ///
      /// Do nothing if there are only one item or less on the stack.
      void swap(Tracer *pdmTracer) {
        if (stack.getNumItems() < 2) return;
        if (pdmTracer) pdmTracer->swap();
        AutomataState topState  = stack.popItem();
        AutomataState nextState = stack.popItem();
        stack.pushItem(topState);
        stack.pushItem(nextState);
      }

      /// \brief Pop the current automata state off of the top
      /// of the push down automata's state stack.
      ///
      /// If keepStream is true, then the popped stream is replaced
      /// by the pre-popped stream (keeping the currently parsed
      /// location).
      void pop(Tracer *pdmTracer, bool keepStream = false) {
        AutomataState tmpState = stack.popItem();
        curState.copyFrom(tmpState, keepStream);
        if (pdmTracer) pdmTracer->pop(keepStream);
      }

      /// \brief Pop the current automata state off the top of the
      /// push down automata's state stack, *keeping* the current
      /// stream location.
      void popKeepStream(Tracer *pdmTracer) {
        pop(pdmTracer, true);
      }

      /// \brief The DFA integrated by this PushDownAutomata.
      DFA *dfa;

      /// \brief The NFA integrated by this PushDownAutomata.
      NFA *nfa;

      /// \brief The DFA::State allocator associated with this
      /// DFA.
      StateAllocator *allocator;

      /// \brief The ParseTrees allocator associated with this
      /// DfA.
      ParseTrees *parseTrees;

      /// \brief The current state of this PushDownAutomata.
      AutomataState curState;

      /// \brief The push down stack for this PushDownAutomata.
      VarArray<AutomataState> stack;

      /// Allow complete access from the associated tracer.
      friend class PushDownMachineTracer;

  }; // class AutomataState
};  // namespace DeterministicFiniteAutomaton

#endif
