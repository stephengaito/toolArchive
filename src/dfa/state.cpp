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

void StateAllocator::emptyState(State *state) {
  if (!state) return;
  char *curByte = state;
  char *stateEnd = curByte + stateSize;
  for (; curByte < stateEnd; curByte++) {
    *curByte = 0;
  }
}

bool StateAllocator::isStateEmpty(State *state) {
  if (!state) return true;
  char *curByte = state;
  char *stateEnd = curByte + stateSize;
  for (; curByte < stateEnd; curByte++) {
    if (*curByte != 0) return false;
  }
  return true;
}

bool StateAllocator::isSubStateOf(State *state, State *other) {
  // return true if this is a subset of other
  if (!other) return false;
  if (!state) return true;
  char *curByte1 = state;
  char *curByte2 = other;
  char *stateEnd1 = curByte1 + stateSize;
  for (; curByte1 < stateEnd1; curByte1++, curByte2++) {
    if (*curByte1 & ~*curByte2) {
      return false; // curByte1 contains a bit outside of curByte2
    }
  }
  return true;
}

void StateAllocator::mergeStateWith(State *state, State *other) {
  if (!state) return;
  if (!other) return;
  char *curByte1 = state;
  char *curByte2 = other;
  char *stateEnd = curByte1 + stateSize;
  for (; curByte1 < stateEnd; curByte1++, curByte2++) {
    *curByte1 |= *curByte2;
  }
}

void StateAllocator::printStateOnWithMessage(FILE *filePtr,
                                        const char* message,
                                        State *state) {
  fprintf(filePtr, "\n%s [%p](%zu bytes) ", message, state, stateSize);
  for (size_t i = 0; i < stateSize; i++) {
    fprintf(filePtr, " %u ", (int)state[i]&0xFF);
  }
  fprintf(filePtr, "\n");
}

/* Check whether state list contains a match. */
NFA::State *StateAllocator::stateMatchesToken(State *state, State *tokensState) {
  for (size_t i = 0; i < stateSize; i++) {
    if (state[i] & tokensState[i]) {
      for (size_t j = 0; j < 8; j++) {
        if (state[i] & tokensState[i] & (1<<j)) {
          return nfaStateMapping->getNFAStateFor((i*8)+j);
        }
      }
    }
  }
  return NULL;
}

void StateAllocator::copyStateIntoBuffer(State *state,
                                    char *buffer, size_t bufferSize) {
  size_t copySize = stateSize;
  if (bufferSize < stateSize) copySize = bufferSize;
  char *curByte = state;
  char *bufferEnd = buffer + copySize;
  for (; buffer < bufferEnd; buffer++, curByte++) {
    *buffer = *curByte;
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

