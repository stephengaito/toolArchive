#ifndef DFA_STATE_ALLOCATOR_H
#define DFA_STATE_ALLOCATOR_H

#include "dfa/nfaStateIterator.h"

namespace DeterministicFiniteAutomaton {

  /// \brief The DFA::StateAllocator class allocates DFA::State(s) over
  /// a given NFA.
  class StateAllocator {
    public:

      /// \brief Create a DFA::StateAllocator object corresponding to a
      /// given NFA.
      StateAllocator(NFA *anNFA);

      /// \brief Destroy the DFA::StateAllocator object.
      ~StateAllocator(void);

      /// \brief Allocate a DFA::State.
      ///
      /// This DFA::State may come from either one of the allocated but
      /// but unused DFA::State(s) *or* from one of the currently unallocated
      /// DFA::State(s) in the current block of DFA::State(s).
      State *allocateANewState(void);

      /// \brief Mark an allocated DFA::State as unused and store it in
      /// one of the allocated but unused DFA::State(s).
      void unallocateState(State *aState);

      /// \brief Ensure that all bits in the DFA::DState bit set are
      /// turned *off*.
      void emptyState(State *state);

      /// \brief Return true if the DFA::DState bit set is empty.
      bool isStateEmpty(State *state);

      /// \brief Return true if the DFA::DState bit set this is a *subset*
      /// of the DFA::DState bit set d2.
      bool isSubStateOf(State *d1, State *d2); // d1 is subsetOf d2

      /// \brief Return the union of the two DFA::DState bit sets,
      /// mergeInto and other, in the DFA::DState bit set mergeInto.
      void mergeStateWith(State *mergeInto, State *other);

      /// \brief Print the bytes of the DFA::DState d on the FILE filePtr
      /// together with the message message.
      void printStateOnWithMessage(FILE *filePtr,
                              const char* message,
                              State *state);

      /// \brief Return true if the DFA::DState contains a token
      /// recognizing NFA::State.
      NFA::State *stateMatchesToken(State *state, State *tokensState);

      void copyStateIntoBuffer(State *state, char *buffer, size_t bufferSize);

      void setNFAState(State *state, NFA::State *nfaState) {
        NFAStateMapping::NFAStateNumber nfaStateNumber =
          nfaStateMapping->getNFAStateNumber(nfaState);
        state[nfaStateNumber.stateByte] |= nfaStateNumber.stateBit;
      };

      size_t getStateSize() {
        return stateSize;
      }

      NFA *getNFA(void) {
        return nfa;
      }

      NFAStateIterator newIteratorOn(State *state) {
        return NFAStateIterator(nfaStateMapping, stateSize, state);
      }

    private:
      /// \brief The NFA associated to this DFA::StateAllocator.
      NFA *nfa;

      /// \brief The DFA::NFAStateMapping for this collection
      /// of DFA::States.
      NFAStateMapping *nfaStateMapping;

      /// \brief The number of bytes in a DFA::DState.
      ///
      /// For a given NFA, this is a fixed number, computed when
      /// the DFA::StateAllocator is created.
      size_t stateSize;

      /// \brief A BlockAllocator which allocates new DStates.
      BlockAllocator *stateAllocator;

      /// \brief One of three allocated but currently unused DFA::State(s).
      ///
      /// Since this DFA::State is unused it can be re-allocated as needed.
      State *allocatedUnusedState0;

      /// \brief One of three allocated but currently unused DFA::State(s).
      ///
      /// Since this DFA::State is unused it can be re-allocated as needed.
      State *allocatedUnusedState1;

      /// \brief One of three allocated but currently unused DFA::State(s).
      ///
      /// Since this DFA::State is unused it can be re-allocated as needed.
      State *allocatedUnusedState2;

  }; // class StateAllocator
};  // namespace DeterministicFiniteAutomaton

#endif
