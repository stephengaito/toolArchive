#ifndef DFA_NEXT_STATE_MAPPING_H
#define DFA_NEXT_STATE_MAPPING_H

#include "dfa/stateAllocator.h"

namespace DeterministicFiniteAutomaton {

  /// \brief The NextStateMapping class is used to implement the next state
  /// mapping which is the heart of the DFA interpreter for a given NFA.
  class NextStateMapping {
    public:

      /// \brief Create a DFA object corresponding to a given NFA.
      NextStateMapping(StateAllocator *anAllocator);

      /// \brief Destroy the DFA object.
      ~NextStateMapping(void);

      /// \brief Register the DFA::DState to ensure all DFA::DState bit
      /// sets use the *same* in memory object.
      ///
      /// This registration process makes use of the nextDFAStateMap.
      State *registerState(State *state);

      State **getNextStateByCharacter(State *curState, utf8Char_t c) {
        assembleStateCharacterProbe(curState, c);
        return (State**)hattrie_get(nextDFAStateMap,
                                    dfaStateProbe,
                                    dfaStateProbeSize);
      }

      State **tryGetNextStateByCharacter(State *curState, utf8Char_t c) {
        assembleStateCharacterProbe(curState, c);
        return (State**)hattrie_tryget(nextDFAStateMap,
                                       dfaStateProbe,
                                       dfaStateProbeSize);
      }

      State **getNextStateByClass(State *curState,
                                  Classifier::classSet_t classSet) {
        assembleStateClassificationProbe(curState, classSet);
        return (State**)hattrie_get(nextDFAStateMap,
                                    dfaStateProbe,
                                    dfaStateProbeSize);
      }

      State **tryGetNextStateByClass(State *curState,
                                  Classifier::classSet_t classSet) {
        assembleStateClassificationProbe(curState, classSet);
        return (State**)hattrie_tryget(nextDFAStateMap,
                                       dfaStateProbe,
                                       dfaStateProbeSize);
      }

    private:

      /// \brief Copy the DFA::DState bytes into the dfaStateProbe array.
      void assembleStateProbe(State *state) {
        allocator->copyStateIntoBuffer(state, dfaStateProbe, dfaStateProbeSize);
      }

      /// \brief Copy the DFA::DState bytes followed by the bytes in
      /// the utf8Char_t structure into the dfaStateProbe array.
      void assembleStateCharacterProbe(State *dfaState,
                                          utf8Char_t curChar);

      /// \brief Copy the DFA::DState bytes followed by the bytes in
      /// the Classifier::classSet_t structure into the dfaStateProbe
      /// array.
      void assembleStateClassificationProbe(State *dfaState,
        Classifier::classSet_t classification);

      /// \brief The DFA::StateAllocator for this NextStateMapping.
      ///
      /// This NextStateMapping maps DFA::State/character/classSet_t
      /// patterns to their associated *next* DFA::states.  All such
      /// DFA::States are allocated by this DFA::StateAllocator.
      StateAllocator *allocator;

      /// \brief The Hat-Trie based next DFA::DState mapping.
      ///
      /// This mapping is used both to register the known DFA::DState(s),
      /// as well as record any successor DFA::DState(s) for a given
      /// DFA::DState + {character | classSet_t} combination.
      hattrie_t   *nextDFAStateMap;

      /// \brief The maximum size of a probe into the nextDFAStateMap
      /// mapping.
      size_t dfaStateProbeSize;

      /// \brief Some (central-pre-DFA) storage for probing the
      /// nextDFAStateMap.
      char  *dfaStateProbe;

  }; // class NextStateMapping
};  // namespace DeterministicFiniteAutomaton

#endif
