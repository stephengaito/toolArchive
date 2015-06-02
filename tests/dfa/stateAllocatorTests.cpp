#include <bandit/bandit.h>
using namespace bandit;

#include <string.h>
#include <stdio.h>

#ifndef private
#define private public
#endif

#include "nfaBuilder.h"
#include <dfa/dfa.h>

namespace DeterministicFiniteAutomaton {

go_bandit([](){

  printf("\n----------------------------------\n");
  printf(  "dfa-stateAllocator\n");
  printf(  "StateAllocator = %zu bytes (%zu bits)\n", sizeof(StateAllocator), sizeof(StateAllocator)*8);
  printf(  "----------------------------------\n");

  /// \brief Test the StateAllocator class.
  describe("StateAllocator", [](){

    /// Show that we can create an appropriately allocated DFA
    /// from a given NFA.
    it("Should have correct sizes and pointers setup", [&](){
      Classifier *classifier = new Classifier();
      NFA *nfa = new NFA(classifier);
      NFABuilder *nfaBuilder = new NFABuilder(nfa);
      nfaBuilder->compileRegularExpressionForTokenId("start", "(abab|abbb)", 1);
      AssertThat(nfa->getNumberStates(), Is().EqualTo(11));
      StateAllocator *allocator = new StateAllocator(nfa);
      AssertThat(allocator, Is().Not().EqualTo((void*)0));
      AssertThat(allocator->nfa, Equals(nfa));
      AssertThat(allocator->nfaStateMapping, Is().Not().EqualTo((void*)0));
      AssertThat(allocator->stateSize, Is().EqualTo(2)); // at most 16 NFA state bits
      AssertThat(allocator->stateAllocator, Is().Not().EqualTo((void*)0));
      AssertThat(allocator->allocatedUnusedStack.getNumItems(), Equals(0));
      delete allocator;
      delete nfa;
      delete classifier;
    });

    /// Show that we can allocate and unallocate States, to/from
    /// the three allocatedUnusedState{0|1|2}.
    it("Allocate and unallocate States", [&](){
      Classifier *classifier = new Classifier();
      NFA *nfa = new NFA(classifier);
      NFABuilder *nfaBuilder = new NFABuilder(nfa);
      nfaBuilder->compileRegularExpressionForTokenId("start", "(abab|abbb)", 1);
      AssertThat(nfa->getNumberStates(), Is().EqualTo(11));
      StateAllocator *allocator = new StateAllocator(nfa);
      AssertThat(allocator->allocatedUnusedStack.getNumItems(), Equals(0));
      State *aDState0 = allocator->allocateANewState();
      State *aDState1 = allocator->allocateANewState();
      State *aDState2 = allocator->allocateANewState();
      State *aDState3 = allocator->allocateANewState();
      AssertThat(aDState0, Is().Not().EqualTo(aDState1));
      AssertThat(aDState0, Is().Not().EqualTo(aDState2));
      AssertThat(aDState1, Is().Not().EqualTo(aDState2));
      AssertThat(allocator->allocatedUnusedStack.getNumItems(), Equals(0));
      allocator->unallocateState(aDState0);
      AssertThat(allocator->allocatedUnusedStack.getNumItems(), Equals(1));
      AssertThat(allocator->allocatedUnusedStack.getItem(0, NULL),
        Equals(aDState0));
      allocator->unallocateState(aDState1);
      AssertThat(allocator->allocatedUnusedStack.getNumItems(), Equals(2));
      AssertThat(allocator->allocatedUnusedStack.getItem(0, NULL),
        Equals(aDState0));
      AssertThat(allocator->allocatedUnusedStack.getItem(1, NULL),
        Equals(aDState1));
      allocator->unallocateState(aDState2);
      AssertThat(allocator->allocatedUnusedStack.getNumItems(), Equals(3));
      AssertThat(allocator->allocatedUnusedStack.getItem(0, NULL),
        Equals(aDState0));
      AssertThat(allocator->allocatedUnusedStack.getItem(1, NULL),
        Equals(aDState1));
      AssertThat(allocator->allocatedUnusedStack.getItem(2, NULL),
        Equals(aDState2));
      allocator->unallocateState(aDState3);
      AssertThat(allocator->allocatedUnusedStack.getNumItems(), Equals(4));
      AssertThat(allocator->allocatedUnusedStack.getItem(0, NULL),
        Equals(aDState0));
      AssertThat(allocator->allocatedUnusedStack.getItem(1, NULL),
        Equals(aDState1));
      AssertThat(allocator->allocatedUnusedStack.getItem(2, NULL),
        Equals(aDState2));
      AssertThat(allocator->allocatedUnusedStack.getItem(3, NULL),
        Equals(aDState3));
      State *aNewDState0 = allocator->allocateANewState();
      AssertThat(aDState3, Is().EqualTo((void*)aNewDState0));
      AssertThat(allocator->allocatedUnusedStack.getNumItems(), Equals(3));
      AssertThat(allocator->allocatedUnusedStack.getItem(0, NULL),
        Equals(aDState0));
      AssertThat(allocator->allocatedUnusedStack.getItem(1, NULL),
        Equals(aDState1));
      AssertThat(allocator->allocatedUnusedStack.getItem(2, NULL),
        Equals(aDState2));
      AssertThat(allocator->allocatedUnusedStack.getItem(3, NULL),
        Equals((void*)0));
      allocator->unallocateState(aNewDState0);
      AssertThat(allocator->allocatedUnusedStack.getNumItems(), Equals(4));
      AssertThat(allocator->allocatedUnusedStack.getItem(0, NULL),
        Equals(aDState0));
      AssertThat(allocator->allocatedUnusedStack.getItem(1, NULL),
        Equals(aDState1));
      AssertThat(allocator->allocatedUnusedStack.getItem(2, NULL),
        Equals(aDState2));
      AssertThat(allocator->allocatedUnusedStack.getItem(3, NULL),
        Equals(aNewDState0));
    });

    it("Allocate and unallocate lots of states", [](){
      State *someNewDStates[100];
      Classifier *classifier = new Classifier();
      NFA *nfa = new NFA(classifier);
      NFABuilder *nfaBuilder = new NFABuilder(nfa);
      nfaBuilder->compileRegularExpressionForTokenId("start", "(abab|abbb)", 1);
      AssertThat(nfa->getNumberStates(), Is().EqualTo(11));
      StateAllocator *allocator = new StateAllocator(nfa);
      AssertThat(allocator->allocatedUnusedStack.getNumItems(), Equals(0));
      for (size_t i = 0; i < 100; i++) {
        someNewDStates[i] = allocator->allocateANewState();
        AssertThat(someNewDStates, Is().Not().EqualTo((void*)0));
      }
      for (size_t i = 0; i < 100; i++) {
        allocator->unallocateState(someNewDStates[i]);
        AssertThat(allocator->allocatedUnusedStack.getNumItems(), Equals(i+1));
        AssertThat(allocator->allocatedUnusedStack.getItem(i, NULL),
          Equals(someNewDStates[i]));
      }
      for (size_t i = 100; 0 < i; i--) {
        State *aState = allocator->allocateANewState();
        AssertThat(aState, Equals(someNewDStates[i-1]));
        AssertThat(allocator->allocatedUnusedStack.getNumItems(), Equals(i-1));
      }
      delete allocator;
      delete nfa;
      delete classifier;
    });

  }); // dfa
});

}; // namespace DeterministicFiniteAutomaton
