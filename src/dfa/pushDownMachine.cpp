#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "dfa/pushDownMachine.h"

using namespace DeterministicFiniteAutomaton;

ParseTrees::Token *PushDownMachine::runFromUsing(NFA::StartStateId startStateId,
                                                 Utf8Chars *charStream,
                                                 PushDownMachine::Tracer *pdmTracer) {

  if (pdmTracer) pdmTracer->setPDM(this);

  curState = AutomataState(allocator, charStream,
                           dfa->getDFAStartState(startStateId),
                           nfa->getStartState(startStateId)->message);

  restart:
  while(true) {
    if (pdmTracer) pdmTracer->reportState();
    // scan current dfa state for ReStart NFA states
    if (pdmTracer) pdmTracer->checkForRestart();
    while(NFA::State *nfaState = curState.getIterator()->nextState()) {
      if (nfaState->matchType == NFA::ReStart) {
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
      if (pdmTracer) pdmTracer->nextDFAState();
      // so we greedily restart with the new nextDFAState
      curState.setDState(nextDFAState);
      goto restart;
    }
    // there is no suitable nextDFAState given the current character
    // does the current DFAState contain a token(match) NFA::State?
    NFA::State *tokenNFAState =
        curState.stateMatchesToken(dfa->getTokensState());
    if (tokenNFAState && (tokenNFAState->matchType == NFA::Token)) {
      curState.getStream()->backup();
      if (pdmTracer) pdmTracer->match(tokenNFAState);
      // we have a match... wrap up this token
      ParseTrees::Token *token = NULL;
      if (!ParseTrees::ignoreToken(tokenNFAState->matchData.t)) {
        token = parseTrees->allocateNewToken(tokenNFAState->matchData.t,
          curState.getStream()->getStart(),
          curState.getStream()->getNumberOfBytesRead(),
          curState.getTokens());
      }
      if (stack.getNumItems()) {
        // we have successfully recoginized a sub state
        // now pop the stack keeping the current stream and restart
        swap(pdmTracer);
        popKeepStream(pdmTracer); // ignore this backtrack state
        popKeepStream(pdmTracer); // use the continue state
        if (pdmTracer) pdmTracer->reportDFAState();
        if (token) curState.getTokens()->pushItem(token);
        goto restart;
      }
      // we have a match BUT the stack is empty
      if (curState.getStream()->atEnd()) {
        if (pdmTracer) pdmTracer->done();
        // we have a match, the stack is empty and ...
        // we are at the end of the stream...
        // so return this token and we are done!
        return token;
      }
      if (pdmTracer) pdmTracer->failedWithStream();
      // we have a match, the stack is empty BUT
      // we have not finished scanning the string...
      // so return the NULL token we have FAILED.
      return NULL;
    }
    // there is no suitable nextDFAState given the current character
    // AND the current DFAState does not contain a suitable token(match)
    // so we need to backtrack and try the next possible path
    if (!stack.getNumItems()) {
      if (pdmTracer) pdmTracer->failedBacktrack();
      // there are no alternate paths...
      // ... so we give up by returning the NULL token.
      return NULL;
    }
    if (pdmTracer) pdmTracer->backtrack();
    pop(pdmTracer); // ignore the continue state
    pop(pdmTracer); // use the backtrack state
    // goto restart;
  }
  // if we have reached this point we have failed!
  if (pdmTracer) pdmTracer->error();
  return NULL;
}
