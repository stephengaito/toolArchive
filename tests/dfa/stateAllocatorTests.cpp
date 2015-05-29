#include <bandit/bandit.h>
using namespace bandit;

#include <string.h>
#include <stdio.h>

#ifndef private
#define private public
#endif

#include "nfaBuilder.h"
#include <dfa/dfa.h>

using namespace DeterministicFiniteAutomaton;

go_bandit([](){

  printf("\n----------------------------------\n");
  printf(  "dfa-stateAllocator\n");
  printf(  "StateAllocator = %zu bytes (%zu bits)\n", sizeof(StateAllocator), sizeof(StateAllocator)*8);
  printf(  "----------------------------------\n");

  /// \brief Test the ability of a given DFA class to compile, on the fly,
  /// a DFA corresponding to a given NFA.
  describe("DFA", [](){

    /// Show that we can create an appropriately allocated DFA
    /// from a given NFA.
    it("Should have correct sizes and pointers setup", [&](){
      Classifier *classifier = new Classifier();
      NFA *nfa = new NFA(classifier);
      NFABuilder *nfaBuilder = new NFABuilder(nfa);
      nfa->appendNFAToStartState(nfaBuilder->compileRegularExpressionForTokenId("(abab|abbb)", 1));
      AssertThat(nfa->getNumberStates(), Is().EqualTo(11));
      StateAllocator *allocator = new StateAllocator(nfa);
      AssertThat(allocator, Is().Not().EqualTo((void*)0));
      AssertThat(allocator->nfa, Equals(nfa));
      AssertThat(allocator->nfaStateMapping, Is().Not().EqualTo((void*)0));
      AssertThat(allocator->stateSize, Is().EqualTo(2)); // at most 16 NFA state bits
      AssertThat(allocator->stateAllocator, Is().Not().EqualTo((void*)0));
      AssertThat(allocator->allocatedUnusedState0, Is().EqualTo((void*)0));
      AssertThat(allocator->allocatedUnusedState1, Is().EqualTo((void*)0));
      AssertThat(allocator->allocatedUnusedState2, Is().EqualTo((void*)0));
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
      nfa->appendNFAToStartState(nfaBuilder->compileRegularExpressionForTokenId("(abab|abbb)", 1));
      AssertThat(nfa->getNumberStates(), Is().EqualTo(11));
      StateAllocator *allocator = new StateAllocator(nfa);
      AssertThat(allocator->allocatedUnusedState0, Is().EqualTo((void*)0));
      AssertThat(allocator->allocatedUnusedState1, Is().EqualTo((void*)0));
      AssertThat(allocator->allocatedUnusedState2, Is().EqualTo((void*)0));
      State *aDState0 = allocator->allocateANewState();
      State *aDState1 = allocator->allocateANewState();
      State *aDState2 = allocator->allocateANewState();
      State *aDState3 = allocator->allocateANewState();
      AssertThat(aDState0, Is().Not().EqualTo(aDState1));
      AssertThat(aDState0, Is().Not().EqualTo(aDState2));
      AssertThat(aDState1, Is().Not().EqualTo(aDState2));
      AssertThat(allocator->allocatedUnusedState0, Is().EqualTo((void*)0));
      AssertThat(allocator->allocatedUnusedState1, Is().EqualTo((void*)0));
      AssertThat(allocator->allocatedUnusedState2, Is().EqualTo((void*)0));
      allocator->unallocateState(aDState0);
      AssertThat(allocator->allocatedUnusedState0, Is().EqualTo(aDState0));
      AssertThat(allocator->allocatedUnusedState1, Is().EqualTo((void*)0));
      AssertThat(allocator->allocatedUnusedState2, Is().EqualTo((void*)0));
      allocator->unallocateState(aDState1);
      AssertThat(allocator->allocatedUnusedState0, Is().EqualTo(aDState0));
      AssertThat(allocator->allocatedUnusedState1, Is().EqualTo(aDState1));
      AssertThat(allocator->allocatedUnusedState2, Is().EqualTo((void*)0));
      allocator->unallocateState(aDState2);
      AssertThat(allocator->allocatedUnusedState0, Is().EqualTo(aDState0));
      AssertThat(allocator->allocatedUnusedState1, Is().EqualTo(aDState1));
      AssertThat(allocator->allocatedUnusedState2, Is().EqualTo(aDState2));
      allocator->unallocateState(aDState3); // quietly dropped aDState3 ;-(
      AssertThat(allocator->allocatedUnusedState0, Is().EqualTo(aDState0));
      AssertThat(allocator->allocatedUnusedState1, Is().EqualTo(aDState1));
      AssertThat(allocator->allocatedUnusedState2, Is().EqualTo(aDState2));
      State *aNewDState0 = allocator->allocateANewState();
      AssertThat(aDState0, Is().EqualTo(aNewDState0));
      AssertThat(allocator->allocatedUnusedState0, Is().EqualTo((void*)0));
      AssertThat(allocator->allocatedUnusedState1, Is().EqualTo(aDState1));
      AssertThat(allocator->allocatedUnusedState2, Is().EqualTo(aDState2));
      allocator->unallocateState(aNewDState0);
      AssertThat(allocator->allocatedUnusedState0, Is().EqualTo(aNewDState0));
      AssertThat(allocator->allocatedUnusedState1, Is().EqualTo(aDState1));
      AssertThat(allocator->allocatedUnusedState2, Is().EqualTo(aDState2));
      for (size_t i = 0; i < 100; i++) {
        State *someNewDStates = allocator->allocateANewState();
        AssertThat(someNewDStates, Is().Not().EqualTo((void*)0));
      }
      delete allocator;
      delete nfa;
      delete classifier;
    });

  }); // dfa
});
