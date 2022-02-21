#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "dynUtf8Parser/nfaBuilder.h"

#ifndef NUM_NFA_STATES_PER_BLOCK
#define NUM_NFA_STATES_PER_BLOCK 20
#endif

NFA::NFA(Classifier *aUTF8Classifier) {
  stateAllocator = new BlockAllocator(NUM_NFA_STATES_PER_BLOCK*sizeof(State));
  startStateIds  = hattrie_create();
  numKnownStates = 0;
  utf8Classifier = aUTF8Classifier;
}

NFA::~NFA(void) {
  for (size_t i = 0; i < startState.getNumItems(); i++) {
    deleteState(startState.getItem(i, NULL));
  }
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
                          NFA::State *out1,
                          const char *aMessage) {
  State *newState =
    (State*)stateAllocator->allocateNewStructure(sizeof(State));
  numKnownStates++;
  newState->matchType = aMatchType;
  newState->matchData = someMatchData;
  newState->out       = out;
  newState->out1      = out1;
  newState->message   = strdup(aMessage);
  return newState;
}

void NFA::deleteState(NFA::State *aState) {
  if (!aState) return;
  NFA::State *oldOut = aState->out;
  aState->out = NULL;
  NFA::State *oldOut1 = aState->out1;
  aState->out1 = NULL;
  if (oldOut)  deleteState(oldOut);
  if (oldOut1) deleteState(oldOut1);
  if (aState->message) free((void*)(aState->message));
  aState->message = NULL;
}

void NFA::appendNFAToStartState(const char *startStateName,
                                NFA::State *baseSplitState) {
  StartStateId startStateId = findStartStateId(startStateName);
  ASSERT(startStateId < startState.getNumItems()); // Corrupted startStateIds Hat-Trie
  MatchData nulMatchData;
  nulMatchData.c.u = 0;
  if (!startState.getItem(startStateId, NULL)) {
    startState.setItem(startStateId, baseSplitState);
    char message[strlen(baseSplitState->message)+10];
    strcpy(message, startStateName);
    strcat(message, "[0]");
    if (baseSplitState->message) free((void*)baseSplitState->message);
    baseSplitState->message = strdup(message);
  } else {
    State *nextState = startState.getItem(startStateId, NULL);
    size_t numStartStates = 1;
    while (nextState->out1) {
      nextState = nextState->out1;
      numStartStates++;
    }
    nextState->out1 = baseSplitState;
    char message[strlen(startStateName)+50];
    sprintf(message, "%s[%zu]", startStateName, numStartStates);
    if (baseSplitState->message) free((void*)baseSplitState->message);
    baseSplitState->message = strdup(message);
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
  fprintf(filePtr, "  message: %s\n", state->message);
}
