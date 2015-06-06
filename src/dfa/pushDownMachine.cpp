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

  curState.stream   = charStream->clone();
  curState.dState   = dfa->getDFAStartState(startStateId);
  curState.iterator = allocator->getNewIteratorOn(curState.dState);
  curState.tokens   = new ParseTrees::TokenArray();
  curState.message  = nfa->getStartState(startStateId)->message;

  restart:
  while(true) {
    if (pdmTracer) pdmTracer->reportDFAState();
    // scan current dfa state for ReStart NFA states
    if (curState.iterator) {
      while(NFA::State *nfaState = curState.iterator->nextState()) {
        if (nfaState->matchType == NFA::ReStart) {
          // we need to try this path
          allocator->clearNFAState(curState.dState, nfaState);
          // push current autoamta state to clean up if this path fails.
          push(pdmTracer, "continuation");
          // now set up the continuation state and push it
          curState.iterator = NULL; // we explicitly have no iterator
          curState.dState = dfa->getDFAStateFromNFAState(nfaState);
          push(pdmTracer, "reStart");
          // now set up the subDFA state
          curState.dState = dfa->getDFAStartState(nfaState->matchData.r);
          curState.iterator = allocator->getNewIteratorOn(curState.dState);
          if (pdmTracer) pdmTracer->restart(nfaState);
          goto restart;
        }
      }
    }
    // we have scanned the dfa state for any ReStart NFA states
    // and none remain.... so we now transition to the next DFA state
    utf8Char_t nextChar = curState.stream->nextUtf8Char();
    if (pdmTracer) pdmTracer->reportChar(nextChar);
    State *nextDFAState =
      dfa->getNextDFAState(curState.dState, nextChar);

    if (nextDFAState) {
      // we have a suitable nextDFAState...
      // does it contain an token NFA::State?
      NFA::State *tokenNFAState =
        allocator->stateMatchesToken(nextDFAState, dfa->getTokensState());
      if (tokenNFAState && (tokenNFAState->matchType == NFA::Token)) {
        // we have a match... wrap up this token
        ParseTrees::Token *token = NULL;
        if (!ParseTrees::ignoreToken(tokenNFAState->matchData.t)) {
          token = parseTrees->allocateNewToken(tokenNFAState->matchData.t,
            curState.stream->getStart(),
            curState.stream->getNumberOfBytesRead(),
            curState.tokens);
        }
        if (stack.getNumItems()) {
          // we have successfully recoginized a sub state
          // now pop the stack keeping the current stream and restart
          popKeepStream(pdmTracer);
          if (token) curState.tokens->pushItem(token);
          goto restart;
        }
        // we have a match BUT the stack is empty
        if (curState.stream->atEnd()) {
          // we have a match, the stack is empty and ...
          // we are at the end of the stream...
          // so return this token and we are done!
          return token;
        }
        // we have a match, the stack is empty BUT
        // we have not finished scanning the string...
        // so return the NULL token we have FAILED.
        return NULL;
      }
      // we have a nextDFAState but no match...
      // so just restart with the new nextDFAState
      curState.dState = allocator->clone(nextDFAState);
      goto restart;
    }
    // there is no suitable nextDFAState given the current character
    // so we need to backtrack and try the next possible path
    if (!stack.getNumItems()) {
      // there are no alternate paths...
      // ... so we give up by returning the NULL token.
      return NULL;
    }
    pop(pdmTracer);
    // we need to pop twice if state we first poped has no iterator...
    // since any state with no iterator is a continuation state
    // NOT a backtrack state.
    if (!curState.iterator) pop(pdmTracer);
    // goto restart;
  }
  // if we have reached this point we have failed!
  return NULL;
}
