/*
 * Regular expression implementation.
 * Supports only ( | ) * + ?.  No escapes.
 * Compiles to NFA and then simulates NFA
 * using Thompson's algorithm.
 * Caches steps of Thompson's algorithm to
 * build DFA on the fly, as in Aho's egrep.
 *
 * See also http://swtch.com/~rsc/regexp/ and
 * Thompson, Ken.  Regular Expression Search Algorithm,
 * Communications of the ACM 11(6) (June 1968), pp. 419-422.
 *
 * Copyright (c) 2007 Russ Cox.
 *
 * Extensive modifications for use as a utf8 lexer compiled by clang
 * are
 *   Copyright (c) 2015 Stephen Gaito
 *
 * Can be distributed under the MIT license, see bottom of file.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "dfa/dfa.h"

using namespace DeterministicFiniteAutomaton;

#ifndef NUM_DFA_STATES_PER_BLOCK
#define NUM_DFA_STATES_PER_BLOCK 20
#endif

DFA::DFA(NFA *anNFA) {
  nfa = anNFA;
  allocator = new StateAllocator(nfa);
  nextStateMapping = new NextStateMapping(allocator);

  dfaStartState = allocator->allocateANewState(); // get space for the stateDState
  computeDFAStartState();
  tokensState =  allocator->allocateANewState(); // get space for the tokensDState
};

DFA::~DFA(void) {
  nfa = NULL;  // we do NOT own the NFA.
  if (nextStateMapping) delete nextStateMapping;
  nextStateMapping = NULL;

  dfaStartState      = NULL;
  tokensState       = NULL;
  if (allocator) delete allocator;
  allocator          = NULL;
}

/* Add nfaState to dfaState, following unlabeled arrows. */
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

/* Compute initial state list */
void DFA::computeDFAStartState(void) {
//  allocator->emptyState(dfaStartState);
  addNFAStateToDFAState(dfaStartState, nfa->getNFAStartState());
  nextStateMapping->registerState(dfaStartState);
}

/*
 * Step the NFA from the states in clist
 * past the character c,
 * to create next NFA state set nlist.
 */
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

  State **resultNextState = NULL;
  // now check if we need to store the specific nextDFAState
  if (allocator->isSubStateOf(nextSpecificDFAState, nextGenericDFAState) ||
      allocator->isStateEmpty(nextSpecificDFAState)) {
    // since the specific DState is a subState of the generic
    // and since we have already registered the generic DState
    // we do not need to do anything more
    // this specific DState is no longer needed
    allocator->unallocateState(nextSpecificDFAState);
  } else {
    // the specific DState is NOT a substate of the generic DState
    // so we want to store the specific state as well and return it
    resultNextState =
      nextStateMapping->getNextStateByCharacter(curDFAState, c);
    // merge the generic states into the specific...
    allocator->mergeStateWith(nextSpecificDFAState, nextGenericDFAState);
    // ensure we use the registered DFAState if any...
    *resultNextState =
      nextStateMapping->registerState(nextSpecificDFAState);
    if (*resultNextState != nextSpecificDFAState) {
      // This specific DState is a copy of the already registered DSstate
      // so it is no longer needed
      allocator->unallocateState(nextSpecificDFAState);
    }
  }
  if (allocator->isStateEmpty(nextGenericDFAState)) {
    // the generic state is empty so we do not store it
    allocator->unallocateState(nextGenericDFAState);
  } else {
    // there is a next generic DFAState...
    // SO ...
    // always store the generic (classification based) nextGenericDState
    resultNextState =
      nextStateMapping->getNextStateByClass(curDFAState, classificationSet);
    // ensure we use the registered DFAState if any...
    *resultNextState =
      nextStateMapping->registerState(nextGenericDFAState);
    if (*resultNextState != nextGenericDFAState) {
      // This generic DState is a copy of the already registered DSstate
      // so it is no longer needed
      allocator->unallocateState(nextGenericDFAState);
      // ensure nextGenericDFAState is a valid DState
      // and the one used in the map
      nextGenericDFAState = *resultNextState;
    }
  }
  if (!resultNextState) {
    return NULL;
  }
  return *resultNextState;
}

/* Run DFA to determine whether it matches s. */
NFA::TokenId DFA::getNextTokenId(Utf8Chars *utf8Stream) {
  State *curDFAState, *nextDFAState;

  curDFAState = dfaStartState;
  utf8Char_t curChar = utf8Stream->nextUtf8Char();
  while (curChar.u) {
    State *nextDFAState = NULL;

    State **tryGetDFAState =
      nextStateMapping->tryGetNextStateByCharacter(curDFAState, curChar);
    if (tryGetDFAState && *tryGetDFAState) {
      nextDFAState = *tryGetDFAState;
    } else {
      // try the more generic
      Classifier::classSet_t classificationSet =
        nfa->getClassifier()->getClassSet(curChar);
      tryGetDFAState =
        nextStateMapping->tryGetNextStateByClass(curDFAState,
                                                 classificationSet);
      if (tryGetDFAState && *tryGetDFAState) {
        nextDFAState = *tryGetDFAState;
      } else {
        nextDFAState = computeNextDFAState(curDFAState,
                                           curChar,
                                           classificationSet);
        if (!nextDFAState) {
          utf8Stream->backup();
          NFA::State *tokenState =
            allocator->stateMatchesToken(curDFAState, tokensState);
          if (tokenState) return tokenState->matchData.t;
          return -1;
        }
      }
    }
    curDFAState = nextDFAState;
    curChar = utf8Stream->nextUtf8Char();
  }
  NFA::State *tokenNFAState =
    allocator->stateMatchesToken(curDFAState, tokensState);
  if (tokenNFAState) return tokenNFAState->matchData.t;
  return -1;
}

/*
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the
 * Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall
 * be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
 * KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS
 * OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

