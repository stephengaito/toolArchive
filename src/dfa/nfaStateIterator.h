#ifndef DFA_NFA_STATE_ITERATOR_H
#define DFA_NFA_STATE_ITERATOR_H

#include "dfa/nfaStateMapping.h"

namespace DeterministicFiniteAutomaton {

  /// \brief The NFAStateIterator class encapsulates the state required
  /// to iterate over the NFA states in a DFA::State bit set.
  class NFAStateIterator {

    public:

      /// \brief Destroy an NFAStateIterator object.
      ~NFAStateIterator(void) {
        curByte = NULL;
        endByte = NULL;
        curBit  = 0;
        curNFAStateNum = 0;
        nfaStateMapping = NULL;
      };

      /// \brief Return the next NFA::State in the DFA::State bit set.
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

      /// \brief ALlow a StateAllocator direct access to the private
      /// constructor method of an NFAStateIterator.
      friend class StateAllocator;

      /// \brief Create a new NFAStateIterator for a given
      /// NFAStateMapping and a particular DFA::State bit set.
      ///
      /// This method can only be invoked by a StateAllocator, which is
      /// the only object which has all of the information to
      /// successfully create an NFAStateIterator.
      NFAStateIterator(NFAStateMapping *aMapping,
                       size_t stateSize,
                       State *state) {
        curByte = state;
        endByte = curByte + stateSize;
        curBit  = 1;
        curNFAStateNum = 0;
        nfaStateMapping = aMapping;
      }

      /// \brief The NFAStateMapping which is used by the iterator to
      /// map from a bit in a DFA::State bit set back to the NFA::State
      /// that bit represents.
      NFAStateMapping *nfaStateMapping;

      /// \brief The index into NFAStateMapping's int2nfaStatePtr
      /// mapping for the current bit in the DFA::State bit set.
      size_t   curNFAStateNum;

      /// \brief A pointer to the current byte in the DFA::State bit
      /// set (which is an array of bytes).
      char    *curByte;

      /// \brief A pointer to the end of the DFA::State bit sets array
      /// of bytes.
      char    *endByte;

      /// \brief The current bit in the current byte in the DFA::State
      /// bit set.
      size_t  curBit;

  };

}; // namespace DeterministicFiniteAutomaton

#endif
