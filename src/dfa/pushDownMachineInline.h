#ifndef PUSH_DOWN_MACHINE_INLINE_H
#define PUSH_DOWN_MACHINE_INLINE_H

      /// \brief Push the current automata state on to the top
      /// of the push down automata's state stack.
      void push(PDMTracer *pdmTracer,
                State *aDState,
                const char *message,
                bool cloneToken = false) {
        if (pdmTracer) pdmTracer->push(message,
          (cloneToken ? "clone Token / full stream" : "new Token / sub stream"));
        stack.pushItem(curState);
        curState.update(aDState, message, cloneToken, false); // DO NOT CLEAR OLD STATE
      }

      /// \brief Push the current automata state on to the top
      /// of the push down automata's state stack.
      void pushCloneToken(PDMTracer *pdmTracer,
                          State *aDState,
                          const char *message) {
        push(pdmTracer, aDState, message, true); // deepClone the token
      }

      /// \brief Push the current automata state on to the top
      /// of the push down automata's state stack.
      void pushNewToken(PDMTracer *pdmTracer,
                        State *aDState,
                        const char *message) {
        push(pdmTracer, aDState, message, false); // create a new token
      }

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
        // we need to try this path
        //
        // prepare messages for each possible AutomataState
        //
        // mark the curState as the backTrack state
        condMerge3(backTrackMessage, PDMTraceMessages(pdmTracer),
          "backtrack<", curState.getMessage(), ">", "backtrack");
        curState.setMessage(backTrackMessage);
        //
        // clear this NFA::State out of the backTrack DFA state
        curState.clearNFAState(nfaState);
        // push current autoamta state to clean up if this path fails.
        condMerge3(continueMessage, PDMTraceMessages(pdmTracer),
          "continue<", curState.getMessage(), ">", "continue");
        pushCloneToken(pdmTracer,
             dfa->getDFAStateFromNFAState(nfaState),
             continueMessage);
        // now set up the subDFA state
        NFA::State *reStartNFAState =
          nfa->getStartState(nfaState->matchData.r);
        condMerge3(restartMessage, PDMTraceRestartMessages(pdmTracer),
          "restart{", reStartNFAState->message, "}", "restart");
        pushNewToken(pdmTracer,
             dfa->getDFAStartState(nfaState->matchData.r),
             restartMessage);
        if (pdmTracer) pdmTracer->restart(nfaState);
      }

#endif
