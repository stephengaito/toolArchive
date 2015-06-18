#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "dynUtf8Parser/dfa/stateAllocator.h"

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
  throw (ParserException) {
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
  if (!nfaStateIntPtr) throw ParserException("corrupted HAT-Trie nfaStatePtr2int");
  if (!*nfaStateIntPtr) {
    if (int2nfaStatePtrSize <= numKnownNFAStates) {
      throw ParserException("could not getNFAStateNumber too few nfaStateInts");
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
