#ifndef PUSH_DOWN_MACHINE_H
#define PUSH_DOWN_MACHINE_H

#include "dfa/automataState.h"

namespace DeterministicFiniteAutomaton {

  /// \brief A PushDownMachine object is used to run a DFA.
  ///
  /// It can be run from any registered start state using
  /// any Utf8Chars.
  class PushDownMachine {

    public:
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
                                      PDMTracer *pdmTracer = NULL) {
        return runFromUsing(nfa->findStartStateId(startStateName),
                            charStream, pdmTracer);
      }

      /// \brief Run the PushDownAutomata from any the given start
      /// state using the Utf8Chars stream provided.
      ParseTrees::Token *runFromUsing(NFA::StartStateId startStateId,
                                      Utf8Chars *charStream,
                                      PDMTracer *pdmTracer = NULL);

    private:

#include "pushDownMachineInline.h"

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
      friend class PDMTracer;

  }; // class AutomataState
};  // namespace DeterministicFiniteAutomaton

#endif
