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

#include "dfa/nextStateMapping.h"

using namespace DeterministicFiniteAutomaton;

#ifndef NUM_DFA_STATES_PER_BLOCK
#define NUM_DFA_STATES_PER_BLOCK 20
#endif

NextStateMapping::NextStateMapping(StateAllocator *anAllocator) {
  allocator = anAllocator;
  dfaStateProbeSize = allocator->getStateSize() + sizeof(utf8Char_t);
  dfaStateProbe = (char*)calloc(dfaStateProbeSize, sizeof(uint8_t));
  nextDFAStateMap   = hattrie_create();
};

NextStateMapping::~NextStateMapping(void) {
  allocator = NULL;
  dfaStateProbeSize = 0;
  if (dfaStateProbe) free(dfaStateProbe);
  dfaStateProbe = NULL;
  if (nextDFAStateMap) hattrie_free(nextDFAStateMap);
  nextDFAStateMap = NULL;
}

void NextStateMapping::assembleStateCharacterProbe(State *state,
                                                      utf8Char_t curChar) {
  assembleStateProbe(state);
  size_t stateSize = allocator->getStateSize();
  for (size_t j = 0; j < sizeof(utf8Char_t); j++) {
    dfaStateProbe[stateSize+j] = curChar.c[j];
  }
}

void NextStateMapping::assembleStateClassificationProbe(State *state,
  Classifier::classSet_t classification) {
  assembleStateProbe(state);
  size_t stateSize = allocator->getStateSize();
  for (size_t j = 0; j < sizeof(Classifier::classSet_t); j++) {
    dfaStateProbe[stateSize+j] = ((uint8_t*)(&classification))[j];
  }
}

State *NextStateMapping::registerState(State *state) {
  assembleStateProbe(state);
  size_t stateSize = allocator->getStateSize();
  State **registeredValue = (State**)hattrie_get(nextDFAStateMap,
                                                 dfaStateProbe,
                                                 stateSize);
  if (!registeredValue) throw LexerException("Hat-Trie failure");
  if (!*registeredValue) *registeredValue = state;
  return *registeredValue;
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

