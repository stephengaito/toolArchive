#ifndef PUSH_DOWN_MACHINE_INLINE_H
#define PUSH_DOWN_MACHINE_INLINE_H

      /// \brief Push the current automata state on to the top
      /// of the push down automata's state stack.
      void push(PDMTracer *pdmTracer, State *aDState, const char *message) {
        if (pdmTracer) pdmTracer->push();
        stack.pushItem(curState);
        curState.update(aDState, message);
      }

      /// \brief Swap the top two elements of the AutomataState stack.
      ///
      /// Do nothing if there are only one item or less on the stack.
      void swap(PDMTracer *pdmTracer) {
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
      void pop(PDMTracer *pdmTracer, bool keepStream = false) {
        AutomataState tmpState = stack.popItem();
        curState.copyFrom(tmpState, keepStream);
        if (pdmTracer) pdmTracer->pop(keepStream);
      }

      /// \brief Pop the current automata state off the top of the
      /// push down automata's state stack, *keeping* the current
      /// stream location.
      void popKeepStream(PDMTracer *pdmTracer) {
        pop(pdmTracer, true);
      }

      void setupRestart(NFA::State *nfaState, PDMTracer *pdmTracer) {
        // we need to try this path
        //
        // prepare messages for each possible AutomataState
        size_t messageSize = strlen(curState.getMessage())+50;
        char backTrackMessage[messageSize];
        strcpy(backTrackMessage, "backtrack<");
        strcat(backTrackMessage, curState.getMessage());
        strcat(backTrackMessage, ">");
        char continueMessage[messageSize];
        strcpy(continueMessage, "continue<");
        strcat(continueMessage, curState.getMessage());
        strcat(continueMessage, ">");
        NFA::State *reStartNFAState =
          nfa->getStartState(nfaState->matchData.r);
        char restartMessage[strlen(reStartNFAState->message)+50];
        strcpy(restartMessage, "restart{");
        strcat(restartMessage, reStartNFAState->message);
        strcat(restartMessage, "}");
        //
        // mark the curState as the backTrack state
        curState.setMessage(backTrackMessage);
        //
        // clear this NFA::State out of the backTrack DFA state
        curState.clearNFAState(nfaState);
        // push current autoamta state to clean up if this path fails.
        push(pdmTracer,
             dfa->getDFAStateFromNFAState(nfaState),
             continueMessage);
        // now set up the subDFA state
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
