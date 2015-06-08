#ifndef PUSH_DOWN_MACHINE_INLINE_H
#define PUSH_DOWN_MACHINE_INLINE_H

      /// \brief Push the current automata state on to the top
      /// of the push down automata's state stack.
      void push(PDMTracer *pdmTracer, State *aDState, const char *message) {
        if (pdmTracer) pdmTracer->push();
        stack.pushItem(curState);
        curState.update(aDState, message, false); // DO NOT CLEAR OLD STATE
      }

      /// \brief Swap the top two elements of the AutomataState stack.
      ///
      /// Do nothing if there are only one item or less on the stack.
      void swap(PDMTracer *pdmTracer) {
        if (pdmTracer && (1 < stack.getNumItems())) pdmTracer->swap();
        stack.swapTopTwoItems();
      }

      /// \brief Pop the current automata state off of the top
      /// of the push down automata's state stack.
      ///
      /// If keepStreamTokens is true, then the popped stream/tokesn
      /// are replaced by the pre-popped stream/tokens (keeping the
      /// currently parsed location and collection of tokens).
      void pop(PDMTracer *pdmTracer, bool keepStreamTokens = false) {
        curState.copyFrom(stack.popItem(), keepStreamTokens, true);// CLEAR OLD STATE
        if (pdmTracer) pdmTracer->pop(keepStreamTokens);
      }

      /// \brief Pop the current automata state off the top of the
      /// push down automata's state stack, *keeping* the current
      /// stream location.
      void popKeepStreamTokens(PDMTracer *pdmTracer) {
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
        push(pdmTracer,
             dfa->getDFAStateFromNFAState(nfaState),
             continueMessage);
        // now set up the subDFA state
        NFA::State *reStartNFAState =
          nfa->getStartState(nfaState->matchData.r);
        condMerge3(restartMessage, PDMTraceRestartMessages(pdmTracer),
          "restart{", reStartNFAState->message, "}", "restart");
        push(pdmTracer,
             dfa->getDFAStartState(nfaState->matchData.r),
             restartMessage);
        if (pdmTracer) pdmTracer->restart(nfaState);
      }

      ParseTrees::Token *wrapUpToken(NFA::State *tokenNFAState) {
        ParseTrees::Token *token = NULL;
        if (!ParseTrees::ignoreToken(tokenNFAState->matchData.t)) {
          token = parseTrees->allocateNewToken(tokenNFAState->matchData.t,
            curState.getStream()->getStart(),
            curState.getStream()->getNumberOfBytesRead(),
            curState.getTokens());
        }
        return token;
      }

#endif
