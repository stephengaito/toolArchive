#ifndef PUSH_DOWN_MACHINE_INLINE_H
#define PUSH_DOWN_MACHINE_INLINE_H

      /// \brief Swap the top two elements of the AutomataState stack.
      ///
      /// Do nothing if there are only one item or less on the stack.
      void swap(PDMTracer *pdmTracer) {
        if (pdmTracer && (1 < stack.getNumItems())) pdmTracer->swap();
        stack.swapTopTwoItems();
      }

      void popIgnore(PDMTracer *pdmTracer) {
        stack.popItem();
        if (pdmTracer) pdmTracer->pop("IGNORE");
      }

      /// \brief Pop the current automata state off of the top
      /// of the push down automata's state stack.
      ///
      /// If keepStream is true, then the popped stream
      /// are replaced by the pre-popped stream (keeping the
      /// currently parsed location).
      void pop(PDMTracer *pdmTracer, bool keepStreamPosition = false) {
        curState.copyFrom(stack.popItem(), keepStreamPosition, true);// CLEAR OLD STATE
        if (pdmTracer) {
          if (keepStreamPosition) pdmTracer->pop("keep stream position");
          else                    pdmTracer->pop("pop stream position");
        }
      }

      /// \brief Pop the current automata state off the top of the
      /// push down automata's state stack, *keeping* the current
      /// stream location.
      void popKeepStreamPosition(PDMTracer *pdmTracer) {
        pop(pdmTracer, true);
      }

      void setupRestart(NFA::State *nfaState, PDMTracer *pdmTracer) {
        ASSERT(dfa);
        ASSERT(nfa);
        ASSERT(nfaState);
        // we need to try this path
        //
        // setup the backtrack state...
        curState.setStateType(AutomataState::ASBackTrack);
        // clear this NFA::State out of the backTrack DFA state
        curState.clearNFAState(nfaState);
        if (pdmTracer) pdmTracer->push("TODO");
        stack.pushItem(curState);

        // setup the call continue state...
        curState.setStateType(AutomataState::ASContinue);
        curState.setDState(dfa->getDFAStateFromNFAState(nfaState));
        curState.cloneSubStream(false);
        curState.cloneToken(true);
        if (pdmTracer) pdmTracer->push("TODO");
        stack.pushItem(curState);

        // now set up the subDFA state
        curState.setStateType(AutomataState::ASRestart);
        curState.setStartStateId(nfaState->matchData.r);
        curState.cloneSubStream(true);
        curState.cloneToken(false);
        if (pdmTracer) pdmTracer->restart();
      }

#endif
