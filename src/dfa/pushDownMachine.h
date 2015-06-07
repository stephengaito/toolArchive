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
          Tracer(FILE *aTraceFile = NULL) {
            pdm        = NULL;
            traceFile  = aTraceFile;
          }

          /// \brief Destroy the tracer.
          ~Tracer(void) {
            pdm = NULL; // we do not own the PDM
            traceFile = NULL; // we do not own the FILE
          }

          /// \brief Sets the associated PushDownMachine
          void setPDM(PushDownMachine *aPDM) {
            pdm = aPDM;
          }


          void reportNFAState(NFA::State *nfaState, size_t indent = 0);

          void reportDFAState(size_t indent = 0);

          void reportChar(utf8Char_t curChar, size_t indent = 0);

          void reportStreamPrefix();

          void reportStreamPostfix();

          void push(size_t indent = 0);

          void pop(bool keepStream, size_t indent = 0);

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
      typedef struct AutomataState {

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

        const char *message;
      } AutomataState;

      /// \brief Push the current automata state on to the top
      /// of the push down automata's state stack.
      void push(Tracer *pdmTracer, const char *message) {
        if (pdmTracer) pdmTracer->push();
        stack.pushItem(curState);
        curState.iterator = NULL;
        curState.stream   = curState.stream->clone();
        curState.dState   = NULL;
        curState.tokens   = new ParseTrees::TokenArray();
        curState.message  = message;
      }

      /// \brief Pop the current automata state off of the top
      /// of the push down automata's state stack.
      ///
      /// If keepStream is true, then the popped stream is replaced
      /// by the pre-popped stream (keeping the currently parsed
      /// location).
      void pop(Tracer *pdmTracer, bool keepStream = false) {
        if (pdmTracer) pdmTracer->pop(keepStream);
        if (curState.iterator) delete curState.iterator;
        curState.iterator = NULL;

        Utf8Chars *keptStream = curState.stream;
        if (!keepStream) {
          if (curState.stream)   delete curState.stream;
          curState.stream   = NULL;
        }

        if (curState.dState)   allocator->unallocateState(curState.dState);
        curState.dState   = NULL;

        if (curState.tokens) delete curState.tokens;

        curState.message = NULL;

        curState = stack.popItem();

        if (keepStream) {
          if (curState.stream)   delete curState.stream;
          curState.stream   = keptStream;
        }
        keptStream = NULL;
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
