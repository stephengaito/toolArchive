#include <string.h>
#include <stdio.h>

#include <cUtils/specs/specs.h>

#ifndef protected
#define protected public
#endif

#include "dynUtf8Parser/nfaBuilder.h"
#include <dynUtf8Parser/dfa/dfa.h>

namespace DeterministicFiniteAutomaton {

/// \brief Test the StateAllocator class.
describe(StateAllocator) {

  specSize(StateAllocator);

  /// Show that we can create an appropriately allocated DFA
  /// from a given NFA.
  it("Should have correct sizes and pointers setup") {
    Classifier *classifier = new Classifier();
    shouldNotBeNULL(classifier);
    NFA *nfa = new NFA(classifier);
    shouldNotBeNULL(nfa);
    NFABuilder *nfaBuilder = new NFABuilder(nfa);
    shouldNotBeNULL(nfaBuilder);
    nfaBuilder->compileRegularExpressionForTokenId("start", "(abab|abbb)", 1);
    shouldBeEqual(nfa->getNumberStates(), 11);
    StateAllocator *allocator = new StateAllocator(nfa);
    shouldNotBeNULL(allocator);
    shouldBeEqual(allocator->nfa, nfa);
    shouldNotBeNULL(allocator->nfaStateMapping);
    shouldBeEqual(allocator->stateSize, 2); // at most 16 NFA state bits
    shouldNotBeNULL(allocator->stateAllocator);
    shouldBeZero(allocator->allocatedUnusedStack.getNumItems());
    delete allocator;
    delete nfaBuilder;
    delete nfa;
    delete classifier;
  } endIt();

  /// Show that we can allocate and unallocate States, to/from
  /// the three allocatedUnusedState{0|1|2}.
  it("Allocate and unallocate States") {
    Classifier *classifier = new Classifier();
    shouldNotBeNULL(classifier);
    NFA *nfa = new NFA(classifier);
    shouldNotBeNULL(nfa);
    NFABuilder *nfaBuilder = new NFABuilder(nfa);
    shouldNotBeNULL(nfaBuilder);
    nfaBuilder->compileRegularExpressionForTokenId("start", "(abab|abbb)", 1);
    shouldBeEqual(nfa->getNumberStates(), 11);
    StateAllocator *allocator = new StateAllocator(nfa);
    shouldNotBeNULL(allocator);
    shouldBeZero(allocator->allocatedUnusedStack.getNumItems());
    State *aDState0 = allocator->allocateANewState();
    State *aDState1 = allocator->allocateANewState();
    State *aDState2 = allocator->allocateANewState();
    State *aDState3 = allocator->allocateANewState();
    shouldNotBeNULL((void*)aDState0);
    shouldNotBeNULL((void*)aDState1);
    shouldNotBeNULL((void*)aDState2);
    shouldNotBeNULL((void*)aDState3);
    shouldNotBeEqual((void*)aDState0, (void*)aDState1);
    shouldNotBeEqual((void*)aDState0, (void*)aDState2);
    shouldNotBeEqual((void*)aDState1, (void*)aDState2);
    shouldBeZero(allocator->allocatedUnusedStack.getNumItems());
    allocator->unallocateState(aDState0);
    shouldBeEqual(allocator->allocatedUnusedStack.getNumItems(), 1);
    shouldBeEqual((void*)allocator->allocatedUnusedStack.getItem(0, NULL),
      (void*)aDState0);
    allocator->unallocateState(aDState1);
    shouldBeEqual(allocator->allocatedUnusedStack.getNumItems(), 2);
    shouldBeEqual((void*)allocator->allocatedUnusedStack.getItem(0, NULL),
      (void*)aDState0);
    shouldBeEqual((void*)allocator->allocatedUnusedStack.getItem(1, NULL),
      (void*)aDState1);
    allocator->unallocateState(aDState2);
    shouldBeEqual(allocator->allocatedUnusedStack.getNumItems(), 3);
    shouldBeEqual((void*)allocator->allocatedUnusedStack.getItem(0, NULL),
      (void*)aDState0);
    shouldBeEqual((void*)allocator->allocatedUnusedStack.getItem(1, NULL),
      (void*)aDState1);
    shouldBeEqual((void*)allocator->allocatedUnusedStack.getItem(2, NULL),
      (void*)aDState2);
    allocator->unallocateState(aDState3);
    shouldBeEqual(allocator->allocatedUnusedStack.getNumItems(), 4);
    shouldBeEqual((void*)allocator->allocatedUnusedStack.getItem(0, NULL),
      (void*)aDState0);
    shouldBeEqual((void*)allocator->allocatedUnusedStack.getItem(1, NULL),
      (void*)aDState1);
    shouldBeEqual((void*)allocator->allocatedUnusedStack.getItem(2, NULL),
      (void*)aDState2);
    shouldBeEqual((void*)allocator->allocatedUnusedStack.getItem(3, NULL),
      (void*)aDState3);
    State *aNewDState0 = allocator->allocateANewState();
    shouldBeEqual((void*)aDState3, (void*)aNewDState0);
    shouldBeEqual(allocator->allocatedUnusedStack.getNumItems(), 3);
    shouldBeEqual((void*)allocator->allocatedUnusedStack.getItem(0, NULL),
      (void*)aDState0);
    shouldBeEqual((void*)allocator->allocatedUnusedStack.getItem(1, NULL),
      (void*)aDState1);
    shouldBeEqual((void*)allocator->allocatedUnusedStack.getItem(2, NULL),
      (void*)aDState2);
    shouldBeNULL((void*)allocator->allocatedUnusedStack.getItem(3, NULL));
    allocator->unallocateState(aNewDState0);
    shouldBeEqual(allocator->allocatedUnusedStack.getNumItems(), 4);
    shouldBeEqual((void*)allocator->allocatedUnusedStack.getItem(0, NULL),
      (void*)aDState0);
    shouldBeEqual((void*)allocator->allocatedUnusedStack.getItem(1, NULL),
      (void*)aDState1);
    shouldBeEqual((void*)allocator->allocatedUnusedStack.getItem(2, NULL),
      (void*)aDState2);
    shouldBeEqual((void*)allocator->allocatedUnusedStack.getItem(3, NULL),
      (void*)aNewDState0);
    delete allocator;
    delete nfaBuilder;
    delete nfa;
    delete classifier;
  } endIt();

  it("Allocate and unallocate lots of states") {
    State *someNewDStates[100];
    Classifier *classifier = new Classifier();
    shouldNotBeNULL(classifier);
    NFA *nfa = new NFA(classifier);
    shouldNotBeNULL(nfa);
    NFABuilder *nfaBuilder = new NFABuilder(nfa);
    shouldNotBeNULL(nfaBuilder);
    nfaBuilder->compileRegularExpressionForTokenId("start", "(abab|abbb)", 1);
    shouldBeEqual(nfa->getNumberStates(), 11);
    StateAllocator *allocator = new StateAllocator(nfa);
    shouldNotBeNULL(allocator);
    shouldBeZero(allocator->allocatedUnusedStack.getNumItems());
    for (size_t i = 0; i < 100; i++) {
      someNewDStates[i] = allocator->allocateANewState();
      shouldNotBeNULL(someNewDStates);
    }
    for (size_t i = 0; i < 100; i++) {
      allocator->unallocateState(someNewDStates[i]);
      shouldBeEqual(allocator->allocatedUnusedStack.getNumItems(), i+1);
      shouldBeEqual((void*)allocator->allocatedUnusedStack.getItem(i, NULL),
        (void*)someNewDStates[i]);
    }
    for (size_t i = 100; 0 < i; i--) {
      State *aState = allocator->allocateANewState();
      shouldBeEqual((void*)aState, (void*)someNewDStates[i-1]);
      shouldBeEqual(allocator->allocatedUnusedStack.getNumItems(), i-1);
    }
    delete allocator;
    delete nfaBuilder;
    delete nfa;
    delete classifier;
  } endIt();

} endDescribe(StateAllocator);

}; // namespace DeterministicFiniteAutomaton
