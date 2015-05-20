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

#include "dfa.h"

DFA::DFA(NFA *anNFA) {
  nfa = anNFA;
  nfaStatePtr2int = hattrie_create();
  dfaStateSize = (nfa->getNumberStates() / 8) + 1;
  dStateVectorSize = 20*dfaStateSize;
  dfaStateProbeSize = dfaStateSize + sizeof(utf8Char_t);
  dfaStateProbe = (char*)calloc(dfaStateProbeSize, sizeof(uint8_t));
  int2nfaStatePtrSize = nfa->getNumberStates();
  int2nfaStatePtr = (NFA::State**)calloc(int2nfaStatePtrSize,
                                         sizeof(NFA::State*));
  numKnownNFAStates = 0;

  nextDFAStateMap   = hattrie_create();

  dStates            = NULL;
  curAllocatedDState = NULL;
  lastDState         = NULL;
  curDStateVector    = 0;
  numDStateVectors   = 0;
  dfaStartState = allocateANewDState(); // get space for the stateDState
  computeDFAStartState();
  tokensDState =  allocateANewDState(); // get space for the tokensDState
  emptyDState(tokensDState);
};

DFA::~DFA(void) {
  nfa = NULL;
  if (nfaStatePtr2int) hattrie_free(nfaStatePtr2int);
  nfaStatePtr2int = NULL;
  if (dfaStateProbe) free(dfaStateProbe);
  dfaStateProbe = NULL;
  dfaStateProbeSize = 0;
  if (int2nfaStatePtr) free(int2nfaStatePtr);
  int2nfaStatePtr = NULL;
  numKnownNFAStates = 0;
  if (nextDFAStateMap) hattrie_free(nextDFAStateMap);
  nextDFAStateMap = NULL;

  if (dStates) {
    for (size_t i = 0; i < numDStateVectors; i++) {
      if (dStates[i]) free(dStates[i]);
      dStates[i] = NULL;
    }
    free(dStates);
  }
  dStates            = NULL;
  dfaStartState      = NULL;
  tokensDState       = NULL;
  curAllocatedDState = NULL;
  lastDState         = NULL;
  curDStateVector    = 0;
  numDStateVectors   = 0;
  allocatedUnusedDState0 = NULL;
  allocatedUnusedDState1 = NULL;
  allocatedUnusedDState2 = NULL;
}

void DFA::emptyDState(DFA::DState *dfaState) {
  if (!dfaState) throw LexerException("invalid DFA state");
  DState *dfaStateEnd = dfaState + dfaStateSize;
  for (; dfaState < dfaStateEnd; dfaState++) {
    *dfaState = 0;
  }
}

bool DFA::isEmptyDState(DFA::DState *dfaState) {
  if (!dfaState) throw LexerException("invalid DFA state");
  DState *dfaStateEnd = dfaState + dfaStateSize;
  for (; dfaState < dfaStateEnd; dfaState++) {
    if (*dfaState != 0) return false;
  }
  return true;
}

void DFA::mergeDStates(DFA::DState *mergeInto, DFA::DState *other) {
  if (!mergeInto) throw LexerException("invalid DFA state");
  if (!other)     throw LexerException("invalid DFA state");
  DState *mergeIntoEnd = mergeInto + dfaStateSize;
  for (; mergeInto < mergeIntoEnd; mergeInto++, other++) {
    *mergeInto |= *other;
  }
}

bool DFA::isSubDState(DFA::DState *d1, DFA::DState *d2) {
  // return true if d1 is a subset of d2
  if (!d1) throw LexerException("invalid DFA state");
  if (!d2) throw LexerException("invalid DFA state");
  DState *d1End = d1 + dfaStateSize;
  for (; d1 < d1End; d1++, d2++) {
    if (*d1 & ~*d2) return false; // d1 contains a bit outside of d2
  }
  return true;
}

/* Check whether state list contains a match. */
bool DFA::matchesToken(DFA::DState *dfaState) {
  for (size_t i = 0; i < dfaStateSize; i++) {
    if (dfaState[i] & tokensDState[i]) return true;
  }
  return false;
}

void DFA::printDState(FILE *filePtr, const char* message, DFA::DState *d) {
  fprintf(filePtr, "\n%s ", message);
  for (size_t i = 0; i < dfaStateSize; i++) {
    fprintf(filePtr, " %u ", (int)d[i]);
  }
  fprintf(filePtr, "\n");
}

DFA::DState *DFA::allocateANewDState(void) {
  DState *newDState = NULL;

  // start by checking if we have any allocated but unused DStates...
  if (allocatedUnusedDState0) {
    newDState = allocatedUnusedDState0;
    allocatedUnusedDState0 = NULL;
    return newDState;
  }
  if (allocatedUnusedDState1) {
    newDState = allocatedUnusedDState1;
    allocatedUnusedDState1 = NULL;
    return newDState;
  }
  if (allocatedUnusedDState2) {
    newDState = allocatedUnusedDState2;
    allocatedUnusedDState2 = NULL;
    return newDState;
  }

  // We have no allocated but unused DStates....
  //
  // So now we check to see if we can allocate a new DState
  // from our current block of DStates...
  if (lastDState <= curAllocatedDState+dfaStateSize) {
    // We have run out of DStates in the current block
    //
    // So we need to first allocate a new block
    if (numDStateVectors <= curDStateVector) {
      // We have run out of block pointers in our DState vector
      //
      // So we allocate a new vector of block pointers
      DState **oldDStates = dStates;
      dStates = (DState**) calloc(numDStateVectors + 10, sizeof(DState*));
      if (oldDStates) {
        memcpy(dStates, oldDStates, numDStateVectors);
      }
      numDStateVectors += 10;
    }
    // We have enough block pointers in our vector of block pointers
    //
    // So now allocate a new block of DStates
    dStates[curDStateVector] = (DState*) calloc(dStateVectorSize,
                                                sizeof(char));
    curAllocatedDState = dStates[curDStateVector];
    lastDState = curAllocatedDState + dStateVectorSize;
    curDStateVector++;
  }
  // We have enough unallocaed DStates to allocate one more
  newDState = curAllocatedDState;
  curAllocatedDState += dfaStateSize;
  return newDState;
}

void DFA::unallocateADState(DFA::DState *aDFAState) {
  if (!allocatedUnusedDState0) {
    allocatedUnusedDState0 = aDFAState;
    return;
  }
  if (!allocatedUnusedDState1) {
    allocatedUnusedDState1 = aDFAState;
    return;
  }
  if (!allocatedUnusedDState2) {
    allocatedUnusedDState2 = aDFAState;
    return;
  }
  // Oops we have allocated too many temporary DStates
  // just quitely drop this one
}

DFA::NFAStateNumber DFA::getNFAStateNumber(NFA::State *nfaState)
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
  // check to see if this NFAState is a token state
  // .... if so record it in the tokensDState
  if (nfaState->matchType == NFA::Token) {
   tokensDState[nfaStateNumber.stateByte] |= nfaStateNumber.stateBit;
  }
  return nfaStateNumber;
}

/* Add nfaState to dfaState, following unlabeled arrows. */
void DFA::addNFAStateToDFAState(DFA::DState *dfaState, NFA::State *nfaState) {
  if (nfaState == NULL) return;
  NFAStateNumber nfaStateNumber = getNFAStateNumber(nfaState);
  dfaState[nfaStateNumber.stateByte] |= nfaStateNumber.stateBit;
  if (nfaState->matchType == NFA::Split) {
    /* follow unlabeled arrows */
    addNFAStateToDFAState(dfaState, nfaState->out);
    addNFAStateToDFAState(dfaState, nfaState->out1);
    return;
  }
}

void DFA::assembleDFAStateProbe(DState *dfaState) {
  for (size_t i = 0; i < dfaStateSize; i++) {
    dfaStateProbe[i] = dfaState[i];
  }
}

void DFA::assembleDFAStateCharacterProbe(DState *dfaState,
                                         utf8Char_t curChar) {
  assembleDFAStateProbe(dfaState);
  for (size_t j = 0; j < sizeof(utf8Char_t); j++) {
    dfaStateProbe[dfaStateSize+j] = curChar.c[j];
  }
}

void DFA::assembleDFAStateClassificationProbe(DState *dfaState,
                                              classSet_t classification) {
  assembleDFAStateProbe(dfaState);
  for (size_t j = 0; j < sizeof(classSet_t); j++) {
    dfaStateProbe[dfaStateSize+j] = ((uint8_t*)(&classification))[j];
  }
}

DFA::DState *DFA::registerDState(DFA::DState *dfaState) {
  DState **registeredValue = (DState**)hattrie_get(nextDFAStateMap,
                                                   dfaState,
                                                   dfaStateSize);
  if (!registeredValue) throw LexerException("Hat-Trie failure");
  if (!*registeredValue) *registeredValue = dfaState;
  return *registeredValue;
}

/* Compute initial state list */
void DFA::computeDFAStartState(void) {
  emptyDState(dfaStartState);
  addNFAStateToDFAState(dfaStartState, nfa->getNFAStartState());
  registerDState(dfaStartState);
}

/*
 * Step the NFA from the states in clist
 * past the character c,
 * to create next NFA state set nlist.
 */
DFA::DState *DFA::computeNextDFAState(DFA::DState *curDFAState,
                                      utf8Char_t c,
                                      classSet_t classificationSet) {
  NFA::State *nfaState;

  DState *nextGenericDFAState;
  nextGenericDFAState = allocateANewDState();
  emptyDState(nextGenericDFAState);

  DState *nextSpecificDFAState;
  nextSpecificDFAState = allocateANewDState();
  emptyDState(nextSpecificDFAState);

  size_t nfaStateNum = 0;
  for (size_t i = 0; i < dfaStateSize; i++) {
    uint8_t curByte = curDFAState[i] & 0xFF;
    for (size_t j = 0; j < 8; j++) {
      if (curByte & 0x01) {
        nfaState = int2nfaStatePtr[nfaStateNum];
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
      curByte >>= 1;
      nfaStateNum++;
    }
  }

  DState **resultNextState = NULL;
  // now check if we need to store the specific nextDFAState
  if (isSubDState(nextSpecificDFAState, nextGenericDFAState) ||
      isEmptyDState(nextSpecificDFAState)) {
    // since the specific DState is a subState of the generic
    // and since we have already registered the generic DState
    // we do not need to do anything more
    // this specific DState is no longer needed
    unallocateADState(nextSpecificDFAState);
  } else {
    // the specific DState is NOT a substate of the generic DState
    // so we want to store the specific state as well and return it
    assembleDFAStateCharacterProbe(curDFAState, c);
    resultNextState = (DState**)hattrie_get(nextDFAStateMap,
                                            dfaStateProbe,
                                            dfaStateProbeSize);
    // merge the generic states into the specific...
    mergeDStates(nextSpecificDFAState, nextGenericDFAState);
    // ensure we use the registered DFAState if any...
    *resultNextState = registerDState(nextSpecificDFAState);
    if (*resultNextState != nextSpecificDFAState) {
      // This specific DState is a copy of the already registered DSstate
      // so it is no longer needed
      unallocateADState(nextSpecificDFAState);
    }
  }
  if (isEmptyDState(nextGenericDFAState)) {
    // the generic state is empty so we do not store it
    unallocateADState(nextGenericDFAState);
  } else {
    // there is a next generic DFAState...
    // SO ...
    // always store the generic (classification based) nextGenericDState
    assembleDFAStateClassificationProbe(curDFAState, classificationSet);
    resultNextState = (DState**)hattrie_get(nextDFAStateMap,
                                            dfaStateProbe,
                                            dfaStateProbeSize);
    // ensure we use the registered DFAState if any...
    *resultNextState = registerDState(nextGenericDFAState);
    if (*resultNextState != nextGenericDFAState) {
      // This generic DState is a copy of the already registered DSstate
      // so it is no longer needed
      unallocateADState(nextGenericDFAState);
      // ensure nextGenericDFAState is a valid DState
      // and the one used in the map
      nextGenericDFAState = *resultNextState;
    }
  }
  if (!resultNextState) return NULL;
  return *resultNextState;
}

/* Run DFA to determine whether it matches s. */
bool DFA::getNextToken(Utf8Chars *utf8Stream) {
  DState *curDFAState, *nextDFAState;

  curDFAState = dfaStartState;
  utf8Char_t curChar = utf8Stream->nextUtf8Char();
  while (curChar.u) {
    assembleDFAStateCharacterProbe(curDFAState, curChar);
    // now probe the nextDFAState hat-trie
    // start with most specific
    DState *nextDFAState = NULL;
    value_t *tryGetDFAState = hattrie_tryget(nextDFAStateMap,
                                             dfaStateProbe,
                                             dfaStateProbeSize);
    if (tryGetDFAState && *tryGetDFAState) {
      nextDFAState = (DState*)*tryGetDFAState;
    } else {
      // try the more generic
      classSet_t classificationSet = nfa->getClassifier()->getClassSet(curChar);
      assembleDFAStateClassificationProbe(curDFAState, classificationSet);
      tryGetDFAState = hattrie_tryget(nextDFAStateMap,
                                      dfaStateProbe,
                                      dfaStateProbeSize);
      if (tryGetDFAState && *tryGetDFAState) {
        nextDFAState = (DState*)*tryGetDFAState;
      } else {
        nextDFAState = computeNextDFAState(curDFAState,
                                           curChar,
                                           classificationSet);
        if (!nextDFAState) return false;
      }
    }
    curDFAState = nextDFAState;
    curChar = utf8Stream->nextUtf8Char();
  }
  return matchesToken(curDFAState);
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

