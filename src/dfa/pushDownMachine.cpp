#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "dfa/pushDownMachine.h"

using namespace DeterministicFiniteAutomaton;

Token *PushDownMachine::runFromUsing(NFA::StartStateId startStateId,
                                     Utf8Chars *charStream,
                                     PDMTracer *pdmTracer,
                                     bool       partialOK) {

  if (pdmTracer) pdmTracer->setPDM(this);

  curState.initialize(allocator, charStream,
                      dfa->getDFAStartState(startStateId),
                      nfa->getStartState(startStateId)->message);

  restart:
  while(true) {
    if (pdmTracer) pdmTracer->reportState();
    // scan current dfa state for ReStart NFA states
    if (pdmTracer) pdmTracer->checkForRestart();
    while(NFA::State *nfaState = curState.getIterator()->nextState()) {
      if (nfaState->matchType == NFA::ReStart) {
        setupRestart(nfaState, pdmTracer);
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
      if (!curState.getStream()->atEnd()) curState.getStream()->backup();
      if (pdmTracer) pdmTracer->match(tokenNFAState);
      // we have a match... wrap up this token
      curState.setTokenId(Token::unWrapTokenId(tokenNFAState->matchData.t));
      curState.setTokenText();

      if (stack.getNumItems()) {
        // we have successfully recoginized a sub state
        // now pop the stack keeping the current stream and restart
        Token *childToken = curState.releaseToken();
        swap(pdmTracer);
        popKeepStream(pdmTracer); // ignore this backtrack state
        popKeepStream(pdmTracer); // use the continue state
        if (pdmTracer) pdmTracer->reportDFAState();
        curState.addChildToken(childToken);
        goto restart;
      }

      // we have a match BUT the stack is empty
      if (partialOK || curState.getStream()->atEnd()) {
        if (pdmTracer) pdmTracer->done();
        // we have a match, the stack is empty and ...
        // we are at the end of the stream
        // (or we are happy with a partial match)...
        // so return this token and we are done!
        Token *token = curState.releaseToken();
        curState.clear();
        return token;
      }

      if (pdmTracer) pdmTracer->failedWithStream();
      // we have a match, the stack is empty BUT
      // we have not finished scanning the string...
      // so return the NULL token we have FAILED.
      curState.clear();
      return NULL;
    }

    // there is no suitable nextDFAState given the current character
    // AND the current DFAState does not contain a suitable token(match)
    // so we need to backtrack and try the next possible path
    if (!stack.getNumItems()) {
      if (pdmTracer) pdmTracer->failedBacktrack();
      // there are no alternate paths...
      // ... so we give up by returning the NULL token.
      curState.clear();
      return NULL;
    }

    if (pdmTracer) pdmTracer->backtrack();
    pop(pdmTracer); // ignore the continue state
    pop(pdmTracer); // use the backtrack state
    // goto restart;
  }
  // if we have reached this point we have failed!
  if (pdmTracer) pdmTracer->errorReturn();
  curState.clear();
  return NULL;
}
