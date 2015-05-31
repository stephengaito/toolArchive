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
  printf(  "dfa-nextStateMapping\n");
  printf(  "NextStateMapping = %zu bytes (%zu bits)\n", sizeof(NextStateMapping), sizeof(NextStateMapping)*8);
  printf(  "----------------------------------\n");

  /// \brief Test the NextStateMapping class.
  describe("NextStateMapping", [](){

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
      NextStateMapping *mapping = new NextStateMapping(allocator);
      AssertThat(mapping, Is().Not().EqualTo((void*)0));
      AssertThat(mapping->allocator, Equals(allocator));
      AssertThat(mapping->nextDFAStateMap, Is().Not().EqualTo((void*)0));
      AssertThat(mapping->dfaStateProbeSize,
                 Equals(allocator->stateSize+sizeof(utf8Char_t)));
      AssertThat(mapping->dfaStateProbe, Is().Not().EqualTo((void*)0));
      for (size_t i = 0; i < mapping->dfaStateProbeSize; i++) {
        AssertThat(mapping->dfaStateProbe[i], Equals(0));
      }
      delete mapping;
      delete allocator;
      delete nfa;
      delete classifier;
    });

    it("Should be able to register a State using NextStateMapping::registerDState", [&](){
      Classifier *classifier = new Classifier();
      NFA *nfa = new NFA(classifier);
      NFABuilder *nfaBuilder = new NFABuilder(nfa);
      nfa->appendNFAToStartState(nfaBuilder->compileRegularExpressionForTokenId("(abab|abbb)", 1));
      AssertThat(nfa->getNumberStates(), Is().EqualTo(11));
      StateAllocator *allocator = new StateAllocator(nfa);
      AssertThat(allocator, Is().Not().EqualTo((void*)0));
      AssertThat(allocator->stateSize, Equals(2));
      NextStateMapping *mapping = new NextStateMapping(allocator);
      AssertThat(mapping, Is().Not().EqualTo((void*)0));
      AssertThat(mapping->dfaStateProbe[0], Equals(0));
      AssertThat(mapping->dfaStateProbe[1], Equals(0));
      State *testState = allocator->allocateANewState();
      testState[0] = 255;
      testState[1] = 255;
      mapping->assembleStateProbe(testState);
      AssertThat((uint8_t)mapping->dfaStateProbe[0], Equals(255));
      AssertThat((uint8_t)mapping->dfaStateProbe[1], Equals(255));
      AssertThat((uint8_t)mapping->dfaStateProbe[2], Equals(0));
      State **registeredTryState =
        (State **)hattrie_tryget(mapping->nextDFAStateMap,
                                 testState, allocator->stateSize);
      AssertThat((void*)registeredTryState, Equals((void*)0));
      State *registeredState  = mapping->registerState(testState);
      AssertThat(registeredState, Equals(testState));
      registeredTryState =
        (State **)hattrie_tryget(mapping->nextDFAStateMap,
                                 testState, allocator->stateSize);
      AssertThat((void*)registeredTryState, Is().Not().EqualTo((void*)0));
      AssertThat((void*)*registeredTryState, Equals((void*)testState));
      delete mapping;
      delete allocator;
      delete nfa;
      delete classifier;
    });

    it("Should be able to register a State/character using NextStateMapping::getNextStateByCharacter", [&](){
      Classifier *classifier = new Classifier();
      NFA *nfa = new NFA(classifier);
      NFABuilder *nfaBuilder = new NFABuilder(nfa);
      nfa->appendNFAToStartState(nfaBuilder->compileRegularExpressionForTokenId("(abab|abbb)", 1));
      AssertThat(nfa->getNumberStates(), Is().EqualTo(11));
      StateAllocator *allocator = new StateAllocator(nfa);
      AssertThat(allocator, Is().Not().EqualTo((void*)0));
      AssertThat(allocator->stateSize, Equals(2));
      NextStateMapping *mapping = new NextStateMapping(allocator);
      AssertThat(mapping, Is().Not().EqualTo((void*)0));
      AssertThat(mapping->dfaStateProbe[0], Equals(0));
      AssertThat(mapping->dfaStateProbe[1], Equals(0));
      AssertThat(mapping->dfaStateProbe[2], Equals(0));
      AssertThat(mapping->dfaStateProbe[3], Equals(0));
      State *testState = allocator->allocateANewState();
      testState[0] = 255;
      testState[1] = 255;
      utf8Char_t aChar;
      aChar.u = 0;
      aChar.c[0] = 'a';
      mapping->assembleStateCharacterProbe(testState, aChar);
      AssertThat((uint8_t)mapping->dfaStateProbe[0], Equals(255));
      AssertThat((uint8_t)mapping->dfaStateProbe[1], Equals(255));
      AssertThat((uint8_t)mapping->dfaStateProbe[2], Equals('a'));
      AssertThat((uint8_t)mapping->dfaStateProbe[3], Equals(0));
      State **nextState =
        (State **)hattrie_tryget(mapping->nextDFAStateMap,
                                 mapping->dfaStateProbe,
                                 mapping->dfaStateProbeSize);
      AssertThat((void*)nextState, Equals((void*)0));
      nextState = mapping->tryGetNextStateByCharacter(testState, aChar);
      AssertThat((void**)nextState, Equals((void**)0));
      nextState = mapping->getNextStateByCharacter(testState, aChar);
      AssertThat((void*)nextState, Is().Not().EqualTo((void*)0));
      delete mapping;
      delete allocator;
      delete nfa;
      delete classifier;
    });

    it("Should be able to register a State/class using NextStateMapping::getNextStateByClass", [&](){
      Classifier *classifier = new Classifier();
      NFA *nfa = new NFA(classifier);
      NFABuilder *nfaBuilder = new NFABuilder(nfa);
      nfa->appendNFAToStartState(nfaBuilder->compileRegularExpressionForTokenId("(abab|abbb)", 1));
      AssertThat(nfa->getNumberStates(), Is().EqualTo(11));
      StateAllocator *allocator = new StateAllocator(nfa);
      AssertThat(allocator, Is().Not().EqualTo((void*)0));
      AssertThat(allocator->stateSize, Equals(2));
      NextStateMapping *mapping = new NextStateMapping(allocator);
      AssertThat(mapping, Is().Not().EqualTo((void*)0));
      AssertThat(mapping->dfaStateProbe[0], Equals(0));
      AssertThat(mapping->dfaStateProbe[1], Equals(0));
      AssertThat(mapping->dfaStateProbe[2], Equals(0));
      AssertThat(mapping->dfaStateProbe[3], Equals(0));
      State *testState = allocator->allocateANewState();
      testState[0] = 255;
      testState[1] = 255;
      Classifier::classSet_t aClass;
      aClass = 10;
      mapping->assembleStateClassificationProbe(testState, aClass);
      AssertThat((uint8_t)mapping->dfaStateProbe[0], Equals(255));
      AssertThat((uint8_t)mapping->dfaStateProbe[1], Equals(255));
      AssertThat((uint8_t)mapping->dfaStateProbe[2], Equals(10));
      AssertThat((uint8_t)mapping->dfaStateProbe[3], Equals(0));
      State **nextState =
        (State **)hattrie_tryget(mapping->nextDFAStateMap,
                                 mapping->dfaStateProbe,
                                 mapping->dfaStateProbeSize);
      AssertThat((void*)nextState, Equals((void*)0));
      nextState = mapping->tryGetNextStateByClass(testState, aClass);
      AssertThat((void**)nextState, Equals((void**)0));
      nextState = mapping->getNextStateByClass(testState, aClass);
      AssertThat((void*)nextState, Is().Not().EqualTo((void*)0));
      delete mapping;
      delete allocator;
      delete nfa;
      delete classifier;
    });

  }); // dfa
});

}; // namespace DeterministicFiniteAutomaton
