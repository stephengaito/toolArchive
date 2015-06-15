#ifndef DFA_NEXT_STATE_MAPPING_H
#define DFA_NEXT_STATE_MAPPING_H

#include "dfa/stateAllocator.h"

namespace DeterministicFiniteAutomaton {

  /// \brief The NextStateMapping class is used to implement the next state
  /// mapping which is the heart of the DFA interpreter for a given NFA.
  ///
  /// The NextStateMapping class ues the [Hat-Trie
  /// library](https://github.com/dcjones/hat-trie).
  class NextStateMapping {
    public:

      /// \brief Create a NextStateMapping object corresponding to a
      /// given collection of DFA::States for an NFA.
      ///
      /// The StateAllocator, is associated to a specific NFA.
      NextStateMapping(StateAllocator *anAllocator);

      /// \brief Destroy the NextStateMapping object.
      ~NextStateMapping(void);

      /// \brief Register the DFA::State to ensure all DFA::State bit
      /// sets use the *same* in memory object.
      ///
      /// This registration process makes use of the nextDFAStateMap.
      State *registerState(State *state);

      /// \brief Using the current DFA::State and character, get the
      /// next DFA::State (if known) from the nextDFAStateMap. If no
      /// next DFA::State exits, register it with the nextDFAStateMap.
      State **getNextStateByCharacter(State *curState, utf8Char_t c) {
        assembleStateCharacterProbe(curState, c);
        return (State**)hattrie_get(nextDFAStateMap,
                                    dfaStateProbe,
                                    dfaStateProbeSize);
      }

      /// \brief Using the current DFA::State and character, get the
      /// next DFA::State (if known) from the nextDFAStateMap. If no
      /// next DFA::State exists, *do* *not* register it with the
      /// nextDFAStateMap.
      State **tryGetNextStateByCharacter(State *curState, utf8Char_t c) {
        assembleStateCharacterProbe(curState, c);
        return (State**)hattrie_tryget(nextDFAStateMap,
                                       dfaStateProbe,
                                       dfaStateProbeSize);
      }

      /// \brief Using the current DFA::State and classification, get
      /// the next DFA::State (if known) from the nextDFAStateMap. If
      /// no next DFA::State exits, register it with the
      /// nextDFAStateMap.
      State **getNextStateByClass(State *curState,
                                  Classifier::classSet_t classSet) {
        assembleStateClassificationProbe(curState, classSet);
        return (State**)hattrie_get(nextDFAStateMap,
                                    dfaStateProbe,
                                    dfaStateProbeSize);
      }

      /// \brief Using the current DFA::State and classification, get
      /// the next DFA::State (if known) from the nextDFAStateMap. If
      /// no next DFA::State exists, *do* *not* register it with the
      /// nextDFAStateMap.
      State **tryGetNextStateByClass(State *curState,
                                  Classifier::classSet_t classSet) {
        assembleStateClassificationProbe(curState, classSet);
        return (State**)hattrie_tryget(nextDFAStateMap,
                                       dfaStateProbe,
                                       dfaStateProbeSize);
      }

    protected:

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

      /// \brief The Hat-Trie based next DFA::State mapping.
      ///
      /// This mapping is used both to register the known DFA::State(s),
      /// as well as record any successor DFA::State(s) for a given
      /// DFA::State + {character | classSet_t} combination.
      hattrie_t   *nextDFAStateMap;

      /// \brief The maximum size of a probe into the nextDFAStateMap
      /// mapping.
      size_t dfaStateProbeSize;

      /// \brief Some (central-per-DFA) storage for probing the
      /// nextDFAStateMap.
      char  *dfaStateProbe;

  }; // class NextStateMapping
};  // namespace DeterministicFiniteAutomaton

#endif
