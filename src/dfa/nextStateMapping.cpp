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
  if (!registeredValue) throw ParserException("Hat-Trie failure");
  if (!*registeredValue) *registeredValue = state;
  return *registeredValue;
}

