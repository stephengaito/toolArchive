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

#include "nfaBuilder.h"

#ifndef NUM_NFA_STATES_PER_BLOCK
#define NUM_NFA_STATES_PER_BLOCK 20
#endif

#ifndef START_STATE_IDS_ARRAY_INCREMENT
#define START_STATE_IDS_ARRAY_INCREMENT 10
#endif

NFA::NFA(Classifier *aUTF8Classifier) {
  stateAllocator = new BlockAllocator(NUM_NFA_STATES_PER_BLOCK*sizeof(State));
  startStateIds  = hattrie_create();
  startState     = NULL;
  nextStartState = 0;
  numStartStates = 0;
  numKnownStates = 0;
  utf8Classifier = aUTF8Classifier;
}

NFA::~NFA(void) {
  if (stateAllocator) delete stateAllocator;
  stateAllocator = NULL;
  if (startStateIds) hattrie_free(startStateIds);
  startStateIds  = NULL;
  if (startState) free(startState);
  startState     = NULL;
  nextStartState = 0;
  numStartStates = 0;
  numKnownStates = 0;
  utf8Classifier = NULL; // classifier is not "owned" by the NFA instance
}

NFA::State *NFA::addState(NFA::MatchType aMatchType,
                          NFA::MatchData someMatchData,
                          NFA::State *out,
                          NFA::State *out1) {
  State *newState =
    (State*)stateAllocator->allocateNewStructure(sizeof(State));
  numKnownStates++;
  newState->matchType = aMatchType;
  newState->matchData = someMatchData;
  newState->out       = out;
  newState->out1      = out1;
  return newState;
}

void NFA::registerStartState(const char *startStateName) {
  StartStateId *startStateId = hattrie_get(startStateIds,
                                           startStateName,
                                           strlen(startStateName));
  ASSERT(startStateId); // corrupted startStateIds Hat-Trie
  if (!*startStateId) {
    // we need to allocate a new startStateId
    if (numStartStates <= nextStartState) {
      // we need to allocate a larger array of start state ids
      State **oldStartStates = startState;
      startState =
        (State**) calloc(numStartStates + START_STATE_IDS_ARRAY_INCREMENT,
                         sizeof(State*));
      if (oldStartStates) {
        memcpy(startState, oldStartStates, numStartStates);
        free(oldStartStates);
      }
      numStartStates += START_STATE_IDS_ARRAY_INCREMENT;
    }
    // to be able to interact with the above check for a newly
    // allocated startStateName Hat-Trie, the *startStateId
    // MUST be one-relative rather than the usual zero-relative.
    *startStateId = nextStartState + 1;
    startState[*startStateId - 1] = NULL;
    nextStartState++;
  }
}

void NFA::appendNFAToStartState(const char *startStateName,
                                NFA::State *baseSplitState) {
  StartStateId startStateId = findStartStateId(startStateName);
  ASSERT(startStateId < numStartStates); // Corrupted startStateIds Hat-Trie
  MatchData nulMatchData;
  nulMatchData.c.u = 0;
  if (!startState[startStateId]) {
    startState[startStateId] = baseSplitState;
  } else {
    State *nextState = startState[startStateId];
    while (nextState->out1) nextState = nextState->out1;
    nextState->out1 = baseSplitState;
  }
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

