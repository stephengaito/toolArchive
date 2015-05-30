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

NFAStateMapping::NFAStateMapping(StateAllocator *anAllocator) {
  allocator = anAllocator;
  nfaStatePtr2int = hattrie_create();
  int2nfaStatePtrSize = allocator->getNFA()->getNumberStates();
  int2nfaStatePtr = (NFA::State**)calloc(int2nfaStatePtrSize,
                                         sizeof(NFA::State*));
  numKnownNFAStates = 0;
};

NFAStateMapping::~NFAStateMapping(void) {
  allocator = NULL;
  if (nfaStatePtr2int) hattrie_free(nfaStatePtr2int);
  nfaStatePtr2int = NULL;
  if (int2nfaStatePtr) free(int2nfaStatePtr);
  int2nfaStatePtr = NULL;
  numKnownNFAStates = 0;
}

NFAStateMapping::NFAStateNumber NFAStateMapping::getNFAStateNumber(NFA::State *nfaState)
  throw (LexerException) {
  NFAStateNumber nfaStateNumber;
  nfaStateNumber.stateByte = 0;
  nfaStateNumber.stateBit  = 0;
  if (!nfaState) return nfaStateNumber;
  long long tmpNFAState = (long long)nfaState;
  char nfaStatePtr[sizeof(NFA::State*)];
  for (size_t i = 0; i < sizeof(NFA::State*); i++) {
    nfaStatePtr[i] = tmpNFAState & 0xFF;
    tmpNFAState >>=8;
  }
  value_t *nfaStateIntPtr = hattrie_get(nfaStatePtr2int,
                                        nfaStatePtr,
                                        sizeof(NFA::State*));
  if (!nfaStateIntPtr) throw LexerException("corrupted HAT-Trie nfaStatePtr2int");
  if (!*nfaStateIntPtr) {
    if (int2nfaStatePtrSize <= numKnownNFAStates) {
      throw LexerException("could not getNFAStateNumber too few nfaStateInts");
    }
    // this NFA::State needs to be added to our mapping
    int2nfaStatePtr[numKnownNFAStates] = nfaState;
    numKnownNFAStates++;
    *nfaStateIntPtr = numKnownNFAStates;
  }
  nfaStateNumber.stateByte = (*nfaStateIntPtr - 1) / 8;
  nfaStateNumber.stateBit  = 1 << ((*nfaStateIntPtr - 1) % 8);
  return nfaStateNumber;
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

