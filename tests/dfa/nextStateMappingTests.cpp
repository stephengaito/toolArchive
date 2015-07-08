#include <string.h>
#include <stdio.h>

#include <cUtils/specs/specs.h>

#ifndef protected
#define protected public
#endif

#include "dynUtf8Parser/nfaBuilder.h"
#include <dynUtf8Parser/dfa/dfa.h>

namespace DeterministicFiniteAutomaton {

/// \brief Test the NextStateMapping class.
describe(DFA_NextStateMapping) {

  specSize(NextStateMapping);

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
    NextStateMapping *mapping = new NextStateMapping(allocator);
    shouldNotBeNULL(mapping);
    shouldBeEqual(mapping->allocator, allocator);
    shouldNotBeNULL(mapping->nextDFAStateMap);
    shouldBeEqual(mapping->dfaStateProbeSize,
               (allocator->stateSize+sizeof(utf8Char_t)));
    shouldNotBeNULL((void*)mapping->dfaStateProbe);
    for (size_t i = 0; i < mapping->dfaStateProbeSize; i++) {
      shouldBeZero(mapping->dfaStateProbe[i]);
    }
    delete mapping;
    delete allocator;
    delete nfaBuilder;
    delete nfa;
    delete classifier;
  } endIt();

  it("Should be able to register a State using",
     "NextStateMapping::registerDState") {
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
    shouldBeEqual(allocator->stateSize, 2);
    NextStateMapping *mapping = new NextStateMapping(allocator);
    shouldNotBeNULL(mapping);
    shouldBeZero(mapping->dfaStateProbe[0]);
    shouldBeZero(mapping->dfaStateProbe[1]);
    State *testState = allocator->allocateANewState();
    testState[0] = 255;
    testState[1] = 255;
    mapping->assembleStateProbe(testState);
    shouldBeEqual((uint8_t)mapping->dfaStateProbe[0], (uint8_t)255);
    shouldBeEqual((uint8_t)mapping->dfaStateProbe[1], (uint8_t)255);
    shouldBeEqual((uint8_t)mapping->dfaStateProbe[2], (uint8_t)0);
    State **registeredTryState =
      (State **)hattrie_tryget(mapping->nextDFAStateMap,
                               testState, allocator->stateSize);
    shouldBeNULL((void*)registeredTryState);
    State *registeredState  = mapping->registerState(testState);
    shouldBeEqual(registeredState, testState);
    registeredTryState =
      (State **)hattrie_tryget(mapping->nextDFAStateMap,
                               testState, allocator->stateSize);
    shouldNotBeNULL((void*)registeredTryState);
    shouldBeEqual((void*)*registeredTryState, (void*)testState);
    delete mapping;
    delete allocator;
    delete nfaBuilder;
    delete nfa;
    delete classifier;
  } endIt();

  it("Should be able to register a State/character using",
     "NextStateMapping::getNextStateByCharacter") {
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
    shouldBeEqual(allocator->stateSize, 2);
    NextStateMapping *mapping = new NextStateMapping(allocator);
    shouldNotBeNULL(mapping);
    shouldBeZero(mapping->dfaStateProbe[0]);
    shouldBeZero(mapping->dfaStateProbe[1]);
    shouldBeZero(mapping->dfaStateProbe[2]);
    shouldBeZero(mapping->dfaStateProbe[3]);
    State *testState = allocator->allocateANewState();
    testState[0] = 255;
    testState[1] = 255;
    utf8Char_t aChar;
    aChar.u = 0;
    aChar.c[0] = 'a';
    mapping->assembleStateCharacterProbe(testState, aChar);
    shouldBeEqual((uint8_t)mapping->dfaStateProbe[0], (uint8_t)255);
    shouldBeEqual((uint8_t)mapping->dfaStateProbe[1], (uint8_t)255);
    shouldBeEqual((uint8_t)mapping->dfaStateProbe[2], (uint8_t)'a');
    shouldBeEqual((uint8_t)mapping->dfaStateProbe[3], (uint8_t)0);
    State **nextState =
      (State **)hattrie_tryget(mapping->nextDFAStateMap,
                               mapping->dfaStateProbe,
                               mapping->dfaStateProbeSize);
    shouldBeNULL((void*)nextState);
    nextState = mapping->tryGetNextStateByCharacter(testState, aChar);
    shouldBeNULL((void**)nextState);
    nextState = mapping->getNextStateByCharacter(testState, aChar);
    shouldNotBeNULL((void*)nextState);
    delete mapping;
    delete allocator;
    delete nfaBuilder;
    delete nfa;
    delete classifier;
  } endIt();

  it("Should be able to register a State/class using",
     "NextStateMapping::getNextStateByClass") {
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
    shouldBeEqual(allocator->stateSize, 2);
    NextStateMapping *mapping = new NextStateMapping(allocator);
    shouldNotBeNULL(mapping);
    shouldBeZero(mapping->dfaStateProbe[0]);
    shouldBeZero(mapping->dfaStateProbe[1]);
    shouldBeZero(mapping->dfaStateProbe[2]);
    shouldBeZero(mapping->dfaStateProbe[3]);
    State *testState = allocator->allocateANewState();
    testState[0] = 255;
    testState[1] = 255;
    Classifier::classSet_t aClass;
    aClass = 10;
    mapping->assembleStateClassificationProbe(testState, aClass);
    shouldBeEqual((uint8_t)mapping->dfaStateProbe[0], (uint8_t)255);
    shouldBeEqual((uint8_t)mapping->dfaStateProbe[1], (uint8_t)255);
    shouldBeEqual((uint8_t)mapping->dfaStateProbe[2], (uint8_t)10);
    shouldBeEqual((uint8_t)mapping->dfaStateProbe[3], (uint8_t)0);
    State **nextState =
      (State **)hattrie_tryget(mapping->nextDFAStateMap,
                               mapping->dfaStateProbe,
                               mapping->dfaStateProbeSize);
    shouldBeNULL((void*)nextState);
    nextState = mapping->tryGetNextStateByClass(testState, aClass);
    shouldBeNULL((void**)nextState);
    nextState = mapping->getNextStateByClass(testState, aClass);
    shouldNotBeNULL((void*)nextState);
    delete mapping;
    delete allocator;
    delete nfaBuilder;
    delete nfa;
    delete classifier;
  } endIt();

} endDescribe(DFA_NextStateMapping);

}; // namespace DeterministicFiniteAutomaton
