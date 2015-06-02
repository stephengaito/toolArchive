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
  numKnownStates = 0;
  utf8Classifier = aUTF8Classifier;
}

NFA::~NFA(void) {
  if (stateAllocator) delete stateAllocator;
  stateAllocator = NULL;
  if (startStateIds) hattrie_free(startStateIds);
  startStateIds  = NULL;
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

void NFA::appendNFAToStartState(const char *startStateName,
                                NFA::State *baseSplitState) {
  StartStateId startStateId = findStartStateId(startStateName);
  ASSERT(startStateId < startState.getNumItems()); // Corrupted startStateIds Hat-Trie
  MatchData nulMatchData;
  nulMatchData.c.u = 0;
  if (!startState.getItem(startStateId, NULL)) {
    startState.setItem(startStateId, baseSplitState);
  } else {
    State *nextState = startState.getItem(startStateId, NULL);
    while (nextState->out1) nextState = nextState->out1;
    nextState->out1 = baseSplitState;
  }
}

void NFA::printStateOnWithMessage(FILE *filePtr,
                                  const char *message,
                                  NFA::State *state) {
  fprintf(filePtr, "%s (%p)\n", message, state);
  fprintf(filePtr, "  MatchType: %u\n", state->matchType);
  fprintf(filePtr, "  MatchData: %lu\n", state->matchData.c.u);
  fprintf(filePtr, "  Out: %p\n", state->out);
  fprintf(filePtr, "  Out1: %p\n", state->out1);
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

