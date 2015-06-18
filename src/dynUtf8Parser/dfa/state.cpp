#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "dynUtf8Parser/dfa/stateAllocator.h"

using namespace DeterministicFiniteAutomaton;

#ifndef NUM_DFA_STATES_PER_BLOCK
#define NUM_DFA_STATES_PER_BLOCK 20
#endif

State *StateAllocator::clone(State *oldState) {
  State *newState = allocateANewState();
  if (!oldState) return newState;
  char *curByte = newState;
  char *oldByte = oldState;
  char *stateEnd = curByte + stateSize;
  for (; curByte < stateEnd; curByte++, oldByte++) {
    *curByte = *oldByte;
  }
  return newState;
}

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
