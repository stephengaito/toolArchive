#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "dynUtf8Parser/dfa/pushDownMachine.h"

using namespace DeterministicFiniteAutomaton;

Token *PushDownMachine::runFromUsing(NFA::StartStateId startStateId,
                                     Utf8Chars *charStream,
                                     PDMTracer *pdmTracer,
                                     bool       partialOK) {

  if (pdmTracer) pdmTracer->setPDM(this);

  curState.initialize(dfa, charStream, startStateId);

  restart:
  while(true) {
    ASSERT(stack.invariant());
    ASSERT(curState.invariant());
    if (pdmTracer) pdmTracer->reportState();

    // scan current dfa state for ReStart NFA states
    if (pdmTracer) pdmTracer->checkForRestart();
    while(NFA::State *nfaState = curState.getIterator()->nextState()) {
      if (nfaState->matchType == NFA::ReStart) {
        // we need to try this path
        //
        // setup the backtrack state...
        curState.setStateType(AutomataState::ASBackTrack);
        // clear this NFA::State out of the backTrack DFA state
        curState.clearNFAStatesWithSameRestartState(nfaState->matchData.r);
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
        goto restart;
      }
    }
    // we have scanned the dfa state for any ReStart NFA states
    // and none remain.... so we now transition to the next DFA state
    utf8Char_t nextChar = curState.getStream()->nextUtf8Char();
    if (pdmTracer) pdmTracer->reportChar(nextChar);
    State *nextDFAState =
      dfa->getNextDFAState(curState.getDState(), nextChar);

    if (nextDFAState) {
      // we have a suitable nextDFAState...
      // so we greedily restart with the new nextDFAState
      if (pdmTracer) pdmTracer->nextDFAState();
      curState.setDState(nextDFAState);
      goto restart;
    }

    // there is no suitable nextDFAState given the current character
    // so push that character back (unless the nextChar is NULL).
    if (nextChar.c[0]) curState.getStream()->backup();

    // does the current DFAState contain a token(match) NFA::State?
    NFA::State *tokenNFAState =
        curState.stateMatchesToken(dfa->getTokensState());
    if (tokenNFAState && (tokenNFAState->matchType == NFA::Token)) {
      if (pdmTracer) pdmTracer->match(tokenNFAState);
      // we have a match... wrap up this token
      curState.setTokenId(Token::unWrapTokenId(tokenNFAState->matchData.t));
      curState.setTokenText();
      Token *token = curState.releaseToken();

      // we have found a match...
      // so pop the stack until we reach a continue state
      popUntil(AutomataState::ASContinue, pdmTracer);

      if (stack.getNumItems()) {
        // we have reached a continue state
        // so pop the stack keeping the current stream and restart
        popKeepStreamPosition(pdmTracer); // use the continue state
        if (pdmTracer) pdmTracer->reportDFAState();
        if (Token::ignoreToken(tokenNFAState->matchData.t)) goto restart;
        curState.addChildToken(token);
        goto restart;
      }

      if (partialOK || curState.getStream()->atEnd()) {
        // we have a match, the stack is empty and ...
        // we are at the end of the stream
        // (or we are happy with a partial match)...
        // so return this token and we are done!
        if (pdmTracer) pdmTracer->done();
        curState.clear();
        return token;
      }

      // we have a match, the stack is empty BUT
      // we have not finished scanning the string...
      // so return the NULL token we have FAILED.
      if (pdmTracer) pdmTracer->failedWithStream();
      curState.clear();
      return NULL;
    }

    // there is no suitable nextDFAState given the current character
    // AND the current DFAState does not contain a suitable token(match)
    // so we need to backtrack and try the next possible path
    if (pdmTracer) pdmTracer->backtrack();
    popUntil(AutomataState::ASBackTrack, pdmTracer);

    if (!stack.getNumItems()) {
      // there are no alternate paths...
      // ... so we give up by returning the NULL token.
      if (pdmTracer) pdmTracer->failedBacktrack();
      curState.clear();
      return NULL;
    }

    popResetStreamPosition(pdmTracer);
    // goto restart;
  }
  // if we have reached this point we have failed!
  if (pdmTracer) pdmTracer->errorReturn();
  curState.clear();
  return NULL;
}
