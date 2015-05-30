#ifndef DFA_NFA_STATE_ITERATOR_H
#define DFA_NFA_STATE_ITERATOR_H

#include "dfa/nfaStateMapping.h"

namespace DeterministicFiniteAutomaton {

  class NFAStateIterator {

    public:
      ~NFAStateIterator(void) {
        curByte = NULL;
        endByte = NULL;
        curBit  = 0;
        curNFAStateNum = 0;
        nfaStateMapping = NULL;
      };

      NFA::State *nextState(void) {
        while (curByte < endByte) {
          while (curBit < 256) {
            if (*curByte & curBit) {
              NFA::State *nfaState =
                nfaStateMapping->getNFAStateFor(curNFAStateNum);
              curBit <<= 1;
              curNFAStateNum++;
              return nfaState;
            } else {
              curBit <<= 1;
              curNFAStateNum++;
            }
          }
          curBit = 1;
          curByte++;
        }
        return NULL;
      }

    private:
      NFAStateIterator(NFAStateMapping *aMapping,
                       size_t stateSize,
                       State *state) {
        curByte = state;
        endByte = curByte + stateSize;
        curBit  = 1;
        curNFAStateNum = 0;
        nfaStateMapping = aMapping;
      }

      NFAStateMapping *nfaStateMapping;
      size_t   curNFAStateNum;
      char    *curByte;
      char    *endByte;
      size_t  curBit;

      friend class StateAllocator;

  };

}; // namespace DeterministicFiniteAutomaton

#endif
