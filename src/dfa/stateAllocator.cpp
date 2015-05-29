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

#include "dfa/stateAllocator.h"

using namespace DeterministicFiniteAutomaton;

#ifndef NUM_DFA_STATES_PER_BLOCK
#define NUM_DFA_STATES_PER_BLOCK 20
#endif

StateAllocator::StateAllocator(NFA *anNFA) {
  nfa = anNFA;
  nfaStateMapping = new NFAStateMapping(this);
  stateSize = (nfa->getNumberStates() / 8) + 1;
  allocatedUnusedState0 = NULL;
  allocatedUnusedState1 = NULL;
  allocatedUnusedState2 = NULL;
  stateAllocator = new BlockAllocator(NUM_DFA_STATES_PER_BLOCK*stateSize);
};

StateAllocator::~StateAllocator(void) {
  nfa = NULL;

  if (nfaStateMapping) delete nfaStateMapping;
  nfaStateMapping = NULL;

  stateSize = 0;
  if (stateAllocator) delete stateAllocator;
  stateAllocator    = NULL;
  allocatedUnusedState0 = NULL;
  allocatedUnusedState1 = NULL;
  allocatedUnusedState2 = NULL;
}

State *StateAllocator::allocateANewState(void) {
  State *newState = NULL;

  // start by checking if we have any allocated but unused DStates...
  if (allocatedUnusedState0) {
    newState = allocatedUnusedState0;
    allocatedUnusedState0 = NULL;
    emptyState(newState);
    return newState;
  }
  if (allocatedUnusedState1) {
    newState = allocatedUnusedState1;
    allocatedUnusedState1 = NULL;
    emptyState(newState);
    return newState;
  }
  if (allocatedUnusedState2) {
    newState = allocatedUnusedState2;
    allocatedUnusedState2 = NULL;
    emptyState(newState);
    return newState;
  }

  // We have no allocated but unused DStates....
  newState = (State*)stateAllocator->allocateNewStructure(stateSize);
  emptyState(newState);
  return newState;
}

void StateAllocator::unallocateState(State *aState) {
  if (!allocatedUnusedState0) {
    allocatedUnusedState0 = aState;
    return;
  }
  if (!allocatedUnusedState1) {
    allocatedUnusedState1 = aState;
    return;
  }
  if (!allocatedUnusedState2) {
    allocatedUnusedState2 = aState;
    return;
  }
  // Oops we have allocated too many temporary DStates
  // just quietly drop this one
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

