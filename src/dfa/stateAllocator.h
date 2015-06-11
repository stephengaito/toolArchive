#ifndef DFA_STATE_ALLOCATOR_H
#define DFA_STATE_ALLOCATOR_H

#include "dfa/nfaStateIterator.h"

namespace DeterministicFiniteAutomaton {

  /// \brief The DFA::StateAllocator class allocates DFA::State(s) over
  /// a given NFA. It also implements simple methods on the light
  /// weight DFA::State(s) which have been allocated by this allocator.
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
      void unallocateState(State *aState) {
        allocatedUnusedStack.pushItem(aState);
      }

      /// \brief Return a copy of the given state.
      State *clone(State *oldState);

      /// \brief Ensure that all bits in the DFA::State bit set are
      /// turned *off*.
      void emptyState(State *state);

      /// \brief Return true if the DFA::State bit set is empty.
      bool isStateEmpty(State *state);

      /// \brief Return true if the DFA::State bit set d1 is a *subset*
      /// of the DFA::State bit set d2.
      bool isSubStateOf(State *d1, State *d2); // d1 is subsetOf d2

      /// \brief Return the union of the two DFA::State bit sets,
      /// mergeInto and other, into the DFA::State bit set mergeInto.
      void mergeStateWith(State *mergeInto, State *other);

      /// \brief Print the bytes of the DFA::State state on the FILE
      /// filePtr together with the message message.
      void printStateOnWithMessage(FILE *filePtr,
                              const char* message,
                              State *state);

      /// \brief Return true if the DFA::State state contains a token
      /// recognizing NFA::State.
      NFA::State *stateMatchesToken(State *state, State *tokensState);

      /// \brief Copy the DFA::State state into the buffer provided.
      void copyStateIntoBuffer(State *state, char *buffer, size_t bufferSize);

      /// \brief Set the bit corresponding the the NFA::State nfaState
      /// in the DFA::State state's bit set.
      void setNFAState(State *state, NFA::State *nfaState) {
        NFAStateMapping::NFAStateNumber nfaStateNumber =
          nfaStateMapping->getNFAStateNumber(nfaState);
        state[nfaStateNumber.stateByte] |= nfaStateNumber.stateBit;
      };

      /// \brief Clear the bit corresponding the the NFA::State nfaState
      /// in the DFA::State state's bit set.
      void clearNFAState(State *state, NFA::State *nfaState) {
        NFAStateMapping::NFAStateNumber nfaStateNumber =
          nfaStateMapping->getNFAStateNumber(nfaState);
        state[nfaStateNumber.stateByte] &= ~nfaStateNumber.stateBit;
      };

      /// \brief Return the size of any and all DFA::State bit sets
      /// allocated by this allocator.
      size_t getStateSize() {
        return stateSize;
      }

      /// \brief Return the NFA associated to this allocator.
      NFA *getNFA(void) {
        return nfa;
      }

      /// \brief Return an NFAStateIterator for the given state.
      NFAStateIterator newIteratorOn(State *state) {
        return NFAStateIterator(nfaStateMapping, stateSize, state);
      }

      /// \brief Return an pointer to an NFAStateIterator for the given 
      /// state.
      NFAStateIterator *getNewIteratorOn(State *state) {
        return new NFAStateIterator(nfaStateMapping, stateSize, state);
      }

    protected:
      /// \brief The NFA associated to this DFA::StateAllocator.
      NFA *nfa;

      /// \brief The DFA::NFAStateMapping for this collection
      /// of DFA::State(s).
      NFAStateMapping *nfaStateMapping;

      /// \brief The number of bytes in a DFA::State.
      ///
      /// For a given NFA, this is a fixed number, computed when
      /// the DFA::StateAllocator is created.
      size_t stateSize;

      /// \brief A BlockAllocator which allocates new DFA::States.
      BlockAllocator *stateAllocator;

      /// \brief One of three allocated but currently unused DFA::State(s).
      ///
      /// Since this DFA::State is unused it can be re-allocated as needed.
      VarArray<State*> allocatedUnusedStack;

  }; // class StateAllocator
};  // namespace DeterministicFiniteAutomaton

#endif
