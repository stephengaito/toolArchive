#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "dynUtf8Parser/dfa/dfa.h"

using namespace DeterministicFiniteAutomaton;

#ifndef NUM_DFA_STATES_PER_BLOCK
#define NUM_DFA_STATES_PER_BLOCK 20
#endif

DFA::DFA(NFA *anNFA) {
  nfa = anNFA;
  allocator = new StateAllocator(nfa);
  nextStateMapping = new NextStateMapping(allocator);

  numStartStates = nfa->getNumberStartStates();
  startState = (State**)calloc(numStartStates, sizeof(State*));
  tokensState =  allocator->allocateANewState(); // get space for the tokensDState
};

DFA::~DFA(void) {
  nfa        = NULL;  // we do NOT own the NFA.
  if (nextStateMapping) delete nextStateMapping;
  nextStateMapping = NULL;

  if (startState) free(startState);
  startState     = NULL;
  numStartStates = 0;
  tokensState    = NULL;

  if (allocator) delete allocator;
  allocator       = NULL;
}

void DFA::addNFAStateToDFAState(State *dfaState, NFA::State *nfaState) {
  if (nfaState == NULL) return;
  allocator->setNFAState(dfaState, nfaState);
  switch (nfaState->matchType) {
    case NFA::Token:
      allocator->setNFAState(tokensState, nfaState);
      break;
    case  NFA::Split:
      /* follow unlabeled arrows */
      addNFAStateToDFAState(dfaState, nfaState->out);
      addNFAStateToDFAState(dfaState, nfaState->out1);
      break;
    default:
      break;
  }
}

State *DFA::getDFAStartState(NFA::StartStateId startStateId) {
  if (numStartStates <= startStateId) return NULL;
  if (!startState[startStateId]) {
    // we have not previously computed this startState... so compute it now
    startState[startStateId] = allocator->allocateANewState();
    addNFAStateToDFAState(startState[startStateId],
                          nfa->getStartState(startStateId));
    nextStateMapping->registerState(startState[startStateId]);
  }
  return startState[startStateId];
}

State *DFA::computeNextDFAState(State *curDFAState,
                                utf8Char_t c,
                                Classifier::classSet_t classificationSet) {
  NFA::State *nfaState;

  State *nextGenericDFAState;
  nextGenericDFAState = allocator->allocateANewState();

  State *nextSpecificDFAState;
  nextSpecificDFAState = allocator->allocateANewState();

  NFAStateIterator nfaStateIter = allocator->newIteratorOn(curDFAState);
  while (NFA::State *nfaState = nfaStateIter.nextState()) {
    switch (nfaState->matchType) {
      case NFA::Character:
        if (nfaState->matchData.c.u == c.u) {
          addNFAStateToDFAState(nextSpecificDFAState, nfaState->out);
        }
        break;
      case NFA::ClassSet:
        if (nfaState->matchData.s & classificationSet) {
          addNFAStateToDFAState(nextGenericDFAState, nfaState->out);
        }
        break;
      default:
        // do nothing
        break;
    }
  }

  State **specificNextState = NULL;
  State **genericNextState = NULL;
  // now check if we need to store the specific nextDFAState
  if (allocator->isSubStateOf(nextSpecificDFAState, nextGenericDFAState) ||
      allocator->isStateEmpty(nextSpecificDFAState)) {
    // since the specific DFA::State is a subState of the generic
    // and since we will register the generic DFA::State we do not
    // need to do anything more this specific DFA::State is no longer
    // needed
    allocator->unallocateState(nextSpecificDFAState);
  } else {
    // the specific DFA::State is NOT a substate of the generic
    // DFA::State so we want to store the specific state as well and
    // return it
    specificNextState =
      nextStateMapping->getNextStateByCharacter(curDFAState, c);
    ASSERT(specificNextState); // Hat-Trie error
    // merge the generic states into the specific...
    allocator->mergeStateWith(nextSpecificDFAState, nextGenericDFAState);
    // ensure we use the registered DFA::State if any...
    *specificNextState =
      nextStateMapping->registerState(nextSpecificDFAState);
    if (*specificNextState != nextSpecificDFAState) {
      // This specific DFA::State is a copy of the already registered
      // DFA::State so it is no longer needed
      allocator->unallocateState(nextSpecificDFAState);
    }
  }
  if (allocator->isStateEmpty(nextGenericDFAState)) {
    // the generic state is empty so we do not store it
    allocator->unallocateState(nextGenericDFAState);
  } else {
    // there is a next generic DFA::State...
    // SO ...
    // always store the generic (classification based) nextGenericDFAState
    genericNextState =
      nextStateMapping->getNextStateByClass(curDFAState, classificationSet);
    ASSERT(genericNextState); // Hat-Trie error
    // ensure we use the registered DFA::State if any...
    *genericNextState =
      nextStateMapping->registerState(nextGenericDFAState);
    if (*genericNextState != nextGenericDFAState) {
      // This generic DFA::State is a copy of the already registered
      // DFA::Sstate so it is no longer needed
      allocator->unallocateState(nextGenericDFAState);
      // ensure nextGenericDFAState is a valid DFA::State
      // and the one used in the map
      nextGenericDFAState = *genericNextState;
    }
  }
  if (specificNextState && *specificNextState) return *specificNextState;
  if (genericNextState  && *genericNextState)  return *genericNextState;
  return NULL;
}

/* Run DFA to determine whether it matches s. */
State *DFA::getNextDFAState(State *curDFAState,
                            utf8Char_t curChar) {
  State *nextDFAState = NULL;
  // try to find an already computed nextDFAState using the more general
  // character classification.
  Classifier::classSet_t classificationSet =
    nfa->getClassifier()->getClassSet(curChar);
  // now explicitly compute a new nextDFAState
  return computeNextDFAState(curDFAState, curChar, classificationSet);
}
