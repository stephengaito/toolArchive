#ifndef PUSH_DOWN_MACHINE_H
#define PUSH_DOWN_MACHINE_H

#include "dynUtf8Parser/dfa/automataState.h"

namespace DeterministicFiniteAutomaton {

  /// \brief A PushDownMachine object is used to run a DFA.
  ///
  /// It can be run from any registered start state using
  /// any Utf8Chars.
  class PushDownMachine {

    public:

      /// \brief An invariant which should ALWAYS be true for any
      /// instance of a PushDownMachine class.
      ///
      /// Throws an AssertionFailure with a brief description of any
      /// inconsistencies discovered.
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

      /// \brief Run the PushDownAutomata from the given start
      /// state using the Utf8Chars stream provided.
      ///
      /// If PDMTracer is not NULL then provide a detailed tracing of
      /// this PushDownMachine's state transitions.
      ///
      /// If paritalOk is true then it is acceptible to have unread
      /// stream when a complete match has been recognized.
      Token *runFromUsing(const char *startStateName,
                          Utf8Chars *charStream,
                          PDMTracer *pdmTracer = NULL,
                          bool       partialOk = false) {
        return runFromUsing(nfa->findStartStateId(startStateName),
                            charStream, pdmTracer, partialOk);
      }

      /// \brief Run the PushDownAutomata from the given start
      /// state using the Utf8Chars stream provided.
      ///
      /// If PDMTracer is not NULL then provide a detailed tracing of
      /// this PushDownMachine's state transitions.
      ///
      /// If paritalOk is true then it is acceptible to have unread
      /// stream when a complete match has been recognized.
      Token *runFromUsing(NFA::StartStateId startStateId,
                          Utf8Chars *charStream,
                          PDMTracer *pdmTracer = NULL,
                          bool       partialOk = false);

    protected:

      /// \brief Pop the current automata state off the top of the
      /// push down automata's state stack, *keeping* the current
      /// stream location.
      void popKeepStreamPosition(PDMTracer *pdmTracer) {
        // keepStreamState = true
        // clearOldState   = true
        if (pdmTracer) pdmTracer->pop("keep stream position");
        curState.copyFrom(stack.popItem(), true, true);
      }

      /// \brief Pop the current automata state off the top of the
      /// push down automata's state stack, *resetting* the current
      /// stream location.
      void popResetStreamPosition(PDMTracer *pdmTracer) {
        // keepStreamState = false
        // clearOldState   = true
        if (pdmTracer) pdmTracer->pop("pop stream position");
        curState.copyFrom(stack.popItem(), false, true);
      }

      /// \brief Pop the current automata state off the top of the
      /// push down automata's state stack *until* the state type is
      /// the required state type.
      void popUntil(AutomataState::AutomataStateType requiredStateType, 
                    PDMTracer *pdmTracer) {
        // TODO: how do we clean up unused state?
        //
        // continue poping until we reach the required state type
        while(stack.getNumItems() &&
              stack.getTop().getStateType() != requiredStateType) {
          if (pdmTracer) pdmTracer->pop("IGNORE looking for ",
            AutomataState::getStateTypeMessage(requiredStateType));
          stack.popItem();
        }
      }

      /// \brief The DFA integrated by this PushDownAutomata.
      DFA *dfa;

      /// \brief The NFA integrated by this PushDownAutomata.
      NFA *nfa;

      /// \brief The DFA::State allocator associated with this
      /// DFA.
      StateAllocator *allocator;

      /// \brief The current state of this PushDownAutomata.
      AutomataState curState;

      /// \brief AutomataStack class provides the VarArray
      /// implementation of the PushDownMachine's AutoamataState stack.
      ///
      /// It primary purpose is to provide explicit invariant checking.
      class AutomataStack : public VarArray<AutomataState> {

        public:

          /// \brief An invariant which should ALWAYS be true for any
          /// instance of a AutomataStack class.
          ///
          /// Throws an AssertionFailure with a brief description of any
          /// inconsistencies discovered.
          bool invariant(void) const {
            if (!VarArray<AutomataState>::invariant())
              throw AssertionFailure("AutomataState varArray failed invariant");

            for (size_t i = 0; i < numItems; i++) {
              if (!itemArray[i].invariant())
                throw AssertionFailure("AutomataState entry failed invariant");
            }
            return true;
          }
      };

      /// \brief The push down stack for this PushDownAutomata.
      AutomataStack stack;

      /// Allow complete access from the associated tracer.
      friend class PDMTracer;

  }; // class AutomataState
};  // namespace DeterministicFiniteAutomaton

#endif
