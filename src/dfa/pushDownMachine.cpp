#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "dfa/dfa.h"

using namespace DeterministicFiniteAutomaton;

ParseTrees::Token *PushDownMachine::runFrom(NFA::StartStateId startStateId,
                                            Utf8Chars *charStream) {

  curState.stream   = charStream.clone();
  curState.dState   = dfa->getStartState(startStateId);
  curState.iterator = dfa->getIteratorOn(curState.dState);

  restart:
  while(true) {
    // scan current dfa state for ReStart NFA states
    if (curState.iterator) {
      while(NFA::State *nfaState = curState.iterator.nextState()) {
        if (nfaState->matchType == NFA::ReStart) {
          // we need to try this path
          dfa->clearNFAStateFromDFAState(nfaState, curState.dState);
          // push current autoamta state to clean up if this path fails.
          push();
          // now set up the continuation state and push it
          curState.iterator = NULL; // we explicitly have no iterator
          curState.dState = dfa->getDFAStateFromNFAState(nfaState);
          push();
          // now set up the subDFA state
          curState.dState = dfa->getStartState(nfaState->matchDate.r);
          curState.iterator = dfa->getIteratorOn(curState.dState);
          goto restart;
        }
      }
    }
    // we have scanned the dfa state for any ReStart NFA states
    // and none remain.... so we now transition to the next DFA state
    State *nextDFAState =
      dfa->getNextDFAState(curState.dState, curState.stream->nextUtf8Char());

    if (nextDFAState) {
      // we have a suitable nextDFAState...
      // does it contain an token NFA::State?
      NFA::State *tokenNFAState =
        allocator->stateMatchesToken(nextDFAState, dfa->getTokensState());
      if (tokenNFAState) {
        // we have a match... wrap up this token
        if (stack.getNumItems()) {
          // we have successfully recoginized a sub state
          // now pop the stack keeping the current stream and restart
          stack.popKeepStream();
          curState.tokens->pushItem(token);
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
    stack.pop();
    // we need to pop twice if state we first poped has no iterator...
    // since any state with no iterator is a continuation state
    // NOT a backtrack state.
    if (!curState.iterator) stack.pop();
    // goto restart;
  }
  // if we have reached this point we have failed!
  return NULL;
}
