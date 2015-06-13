#ifndef PUSH_DOWN_MACHINE_H
#define PUSH_DOWN_MACHINE_H

#include "dfa/automataStack.h"

namespace DeterministicFiniteAutomaton {

  /// \brief A PushDownMachine object is used to run a DFA.
  ///
  /// It can be run from any registered start state using
  /// any Utf8Chars.
  class PushDownMachine {

    public:
      bool invariant(void) const {
        return curState.invariant();
      }

      /// \brief Create a new PushDownMachine instance.
      PushDownMachine(DFA *aDFA) {
        dfa        = aDFA;
        nfa        = dfa->getNFA();
        allocator  = dfa->getStateAllocator();
        ASSERT(invariant());
      }

      /// \brief Run the PushDownAutomata from any the given start
      /// state using the Utf8Chars stream provided.
      Token *runFromUsing(const char *startStateName,
                          Utf8Chars *charStream,
                          PDMTracer *pdmTracer = NULL,
                          bool       partialOk = false) {
        return runFromUsing(nfa->findStartStateId(startStateName),
                            charStream, pdmTracer, partialOk);
      }

      /// \brief Run the PushDownAutomata from any the given start
      /// state using the Utf8Chars stream provided.
      Token *runFromUsing(NFA::StartStateId startStateId,
                          Utf8Chars *charStream,
                          PDMTracer *pdmTracer = NULL,
                          bool       partialOk = false);

    protected:

#include "pushDownMachineInline.h"

      /// \brief The DFA integrated by this PushDownAutomata.
      DFA *dfa;

      /// \brief The NFA integrated by this PushDownAutomata.
      NFA *nfa;

      /// \brief The DFA::State allocator associated with this
      /// DFA.
      StateAllocator *allocator;

      /// \brief The current state of this PushDownAutomata.
      AutomataState curState;

      /// \brief The push down stack for this PushDownAutomata.
      AutomataStack stack;

      /// Allow complete access from the associated tracer.
      friend class PDMTracer;

  }; // class AutomataState
};  // namespace DeterministicFiniteAutomaton

#endif
