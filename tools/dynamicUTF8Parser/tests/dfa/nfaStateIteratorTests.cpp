#include <string.h>
#include <stdio.h>

#include <cUtils/specs/specs.h>

#ifndef protected
#define protected public
#endif

#include "dynUtf8Parser/nfaBuilder.h"
#include <dynUtf8Parser/dfa/dfa.h>

namespace DeterministicFiniteAutomaton {

/// \brief Test the NFAStateIterator class.
describe(DFA_NFAStateIterator) {

  specSize(NFAStateIterator);

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
    NFAStateMapping *mapping = allocator->nfaStateMapping;
    shouldNotBeNULL(mapping);
    State *state = allocator->allocateANewState();
    shouldNotBeNULL((void*)state);
    NFAStateIterator iterator = allocator->newIteratorOn(state);
    shouldBeZero(iterator.curNFAStateNum);
    shouldBeEqual((void*)iterator.curByte, (void*)state);
    shouldBeEqual((void*)iterator.endByte, (void*)(state+allocator->stateSize));
    shouldBeEqual(iterator.curBit, 1);
    // mapping is owned by allocator
    delete allocator;
    delete nfaBuilder;
    delete nfa;
    delete classifier;
  } endIt();

  it("Show that DFA::getNFAStateNumber can deal with lots of NFA::States") {
    Classifier *classifier = new Classifier();
    shouldNotBeNULL(classifier);
    NFA *nfa = new NFA(classifier);
    shouldNotBeNULL(nfa);
    NFABuilder *nfaBuilder = new NFABuilder(nfa);
    shouldNotBeNULL(nfaBuilder);
    nfaBuilder->compileRegularExpressionForTokenId("start", "thisisasimpletest", 1);
    shouldBeEqual(nfa->getNumberStates(), 19);
    StateAllocator *allocator = new StateAllocator(nfa);
    shouldNotBeNULL(allocator);
    shouldBeEqual(allocator->stateSize, 3);
    NFAStateMapping *mapping = allocator->nfaStateMapping;
    shouldNotBeNULL(mapping);
    State *state = allocator->allocateANewState();
    shouldNotBeNULL((void*)state);
    NFA::State *baseState =
      (NFA::State*)nfa->stateAllocator->blocks.getTop();
    shouldNotBeNULL(baseState);
    NFAStateMapping::NFAStateNumber aStateNum =
      mapping->getNFAStateNumber(baseState);
    shouldBeEqual(aStateNum.stateByte, 0);
    shouldBeEqual((uint8_t)aStateNum.stateBit,  (uint8_t)1);
    allocator->setNFAState(state, baseState);
    shouldBeEqual((uint8_t)state[0], (uint8_t)1);
    aStateNum = mapping->getNFAStateNumber(baseState+1);
    shouldBeEqual(aStateNum.stateByte, 0);
    shouldBeEqual((uint8_t)aStateNum.stateBit,  (uint8_t)2);
    allocator->setNFAState(state, baseState+1);
    shouldBeEqual((uint8_t)state[0], (uint8_t)3);
    aStateNum = mapping->getNFAStateNumber(baseState+2);
    shouldBeEqual(aStateNum.stateByte, 0);
    shouldBeEqual((uint8_t)aStateNum.stateBit,  (uint8_t)4);
    allocator->setNFAState(state, baseState+2);
    shouldBeEqual((uint8_t)state[0], (uint8_t)7);
    aStateNum = mapping->getNFAStateNumber(baseState+3);
    shouldBeEqual(aStateNum.stateByte, 0);
    shouldBeEqual((uint8_t)aStateNum.stateBit,  (uint8_t)8);
    allocator->setNFAState(state, baseState+3);
    shouldBeEqual((uint8_t)state[0], (uint8_t)15);
    aStateNum = mapping->getNFAStateNumber(baseState+4);
    shouldBeEqual(aStateNum.stateByte, 0);
    shouldBeEqual((uint8_t)aStateNum.stateBit,  (uint8_t)16);
    allocator->setNFAState(state, baseState+4);
    shouldBeEqual((uint8_t)state[0], (uint8_t)31);
    aStateNum = mapping->getNFAStateNumber(baseState+5);
    shouldBeEqual(aStateNum.stateByte, 0);
    shouldBeEqual((uint8_t)aStateNum.stateBit,  (uint8_t)32);
    allocator->setNFAState(state, baseState+5);
    shouldBeEqual((uint8_t)state[0], (uint8_t)63);
    aStateNum = mapping->getNFAStateNumber(baseState+6);
    shouldBeEqual(aStateNum.stateByte, 0);
    shouldBeEqual((uint8_t)aStateNum.stateBit,  (uint8_t)64);
    allocator->setNFAState(state, baseState+6);
    shouldBeEqual((uint8_t)state[0], (uint8_t)127);
    aStateNum = mapping->getNFAStateNumber(baseState+7);
    shouldBeEqual(aStateNum.stateByte, 0);
    shouldBeEqual((uint8_t)aStateNum.stateBit,  (uint8_t)128);
    allocator->setNFAState(state, baseState+7);
    shouldBeEqual((uint8_t)state[0], (uint8_t)255);
    aStateNum = mapping->getNFAStateNumber(baseState+8);
    shouldBeEqual(aStateNum.stateByte, 1);
    shouldBeEqual((uint8_t)aStateNum.stateBit,  (uint8_t)1);
    allocator->setNFAState(state, baseState+8);
    shouldBeEqual((uint8_t)state[0], (uint8_t)255);
    shouldBeEqual((uint8_t)state[1], (uint8_t)1);
    aStateNum = mapping->getNFAStateNumber(baseState+9);
    shouldBeEqual(aStateNum.stateByte, 1);
    shouldBeEqual((uint8_t)aStateNum.stateBit,  (uint8_t)2);
    allocator->setNFAState(state, baseState+9);
    shouldBeEqual((uint8_t)state[0], (uint8_t)255);
    shouldBeEqual((uint8_t)state[1], (uint8_t)3);
    aStateNum = mapping->getNFAStateNumber(baseState+10);
    shouldBeEqual(aStateNum.stateByte, 1);
    shouldBeEqual((uint8_t)aStateNum.stateBit,  (uint8_t)4);
    allocator->setNFAState(state, baseState+10);
    shouldBeEqual((uint8_t)state[0], (uint8_t)255);
    shouldBeEqual((uint8_t)state[1], (uint8_t)7);
    aStateNum = mapping->getNFAStateNumber(baseState+11);
    shouldBeEqual(aStateNum.stateByte, 1);
    shouldBeEqual((uint8_t)aStateNum.stateBit,  (uint8_t)8);
    allocator->setNFAState(state, baseState+11);
    shouldBeEqual((uint8_t)state[0], (uint8_t)255);
    shouldBeEqual((uint8_t)state[1], (uint8_t)15);
    aStateNum = mapping->getNFAStateNumber(baseState+12);
    shouldBeEqual(aStateNum.stateByte, 1);
    shouldBeEqual((uint8_t)aStateNum.stateBit,  (uint8_t)16);
    allocator->setNFAState(state, baseState+12);
    shouldBeEqual((uint8_t)state[0], (uint8_t)255);
    shouldBeEqual((uint8_t)state[1], (uint8_t)31);
    aStateNum = mapping->getNFAStateNumber(baseState+13);
    shouldBeEqual(aStateNum.stateByte, 1);
    shouldBeEqual((uint8_t)aStateNum.stateBit,  (uint8_t)32);
    allocator->setNFAState(state, baseState+13);
    shouldBeEqual((uint8_t)state[0], (uint8_t)255);
    shouldBeEqual((uint8_t)state[1], (uint8_t)63);
    aStateNum = mapping->getNFAStateNumber(baseState+14);
    shouldBeEqual(aStateNum.stateByte, 1);
    shouldBeEqual((uint8_t)aStateNum.stateBit,  (uint8_t)64);
    allocator->setNFAState(state, baseState+14);
    shouldBeEqual((uint8_t)state[0], (uint8_t)255);
    shouldBeEqual((uint8_t)state[1], (uint8_t)127);
    aStateNum = mapping->getNFAStateNumber(baseState+15);
    shouldBeEqual(aStateNum.stateByte, 1);
    shouldBeEqual((uint8_t)aStateNum.stateBit,  (uint8_t)128);
    allocator->setNFAState(state, baseState+15);
    shouldBeEqual((uint8_t)state[0], (uint8_t)255);
    shouldBeEqual((uint8_t)state[1], (uint8_t)255);
    aStateNum = mapping->getNFAStateNumber(baseState+16);
    shouldBeEqual(aStateNum.stateByte, 2);
    shouldBeEqual((uint8_t)aStateNum.stateBit,  (uint8_t)1);
    allocator->setNFAState(state, baseState+16);
    shouldBeEqual((uint8_t)state[0], (uint8_t)255);
    shouldBeEqual((uint8_t)state[1], (uint8_t)255);
    shouldBeEqual((uint8_t)state[2], (uint8_t)1);
    aStateNum = mapping->getNFAStateNumber(baseState+17);
    shouldBeEqual(aStateNum.stateByte, 2);
    shouldBeEqual((uint8_t)aStateNum.stateBit,  (uint8_t)2);
    allocator->setNFAState(state, baseState+17);
    shouldBeEqual((uint8_t)state[0], (uint8_t)255);
    shouldBeEqual((uint8_t)state[1], (uint8_t)255);
    shouldBeEqual((uint8_t)state[2], (uint8_t)3);
    aStateNum = mapping->getNFAStateNumber(baseState+18);
    shouldBeEqual(aStateNum.stateByte, 2);
    shouldBeEqual((uint8_t)aStateNum.stateBit,  (uint8_t)4);
    allocator->setNFAState(state, baseState+18);
    shouldBeEqual((uint8_t)state[0], (uint8_t)255);
    shouldBeEqual((uint8_t)state[1], (uint8_t)255);
    shouldBeEqual((uint8_t)state[2], (uint8_t)7);
    NFAStateIterator iterator = allocator->newIteratorOn(state);
    for (size_t i = 0; i < 19; i++) {
      shouldBeEqual(iterator.nextState(), baseState+i);
    }
    shouldBeNULL(iterator.nextState());
    allocator->clearNFAState(state, baseState);
    shouldBeEqual((uint8_t)state[0], (uint8_t)254);
    shouldBeEqual((uint8_t)state[1], (uint8_t)255);
    shouldBeEqual((uint8_t)state[2], (uint8_t)7);
    allocator->clearNFAState(state, baseState+1);
    shouldBeEqual((uint8_t)state[0], (uint8_t)252);
    shouldBeEqual((uint8_t)state[1], (uint8_t)255);
    shouldBeEqual((uint8_t)state[2], (uint8_t)7);
    allocator->clearNFAState(state, baseState+2);
    shouldBeEqual((uint8_t)state[0], (uint8_t)248);
    shouldBeEqual((uint8_t)state[1], (uint8_t)255);
    shouldBeEqual((uint8_t)state[2], (uint8_t)7);
    allocator->clearNFAState(state, baseState+3);
    shouldBeEqual((uint8_t)state[0], (uint8_t)240);
    shouldBeEqual((uint8_t)state[1], (uint8_t)255);
    shouldBeEqual((uint8_t)state[2], (uint8_t)7);
    allocator->clearNFAState(state, baseState+4);
    shouldBeEqual((uint8_t)state[0], (uint8_t)224);
    shouldBeEqual((uint8_t)state[1], (uint8_t)255);
    shouldBeEqual((uint8_t)state[2], (uint8_t)7);
    allocator->clearNFAState(state, baseState+5);
    shouldBeEqual((uint8_t)state[0], (uint8_t)192);
    shouldBeEqual((uint8_t)state[1], (uint8_t)255);
    shouldBeEqual((uint8_t)state[2], (uint8_t)7);
    allocator->clearNFAState(state, baseState+6);
    shouldBeEqual((uint8_t)state[0], (uint8_t)128);
    shouldBeEqual((uint8_t)state[1], (uint8_t)255);
    shouldBeEqual((uint8_t)state[2], (uint8_t)7);
    allocator->clearNFAState(state, baseState+7);
    shouldBeEqual((uint8_t)state[0], (uint8_t)0);
    shouldBeEqual((uint8_t)state[1], (uint8_t)255);
    shouldBeEqual((uint8_t)state[2], (uint8_t)7);
    allocator->clearNFAState(state, baseState+8);
    shouldBeEqual((uint8_t)state[0], (uint8_t)0);
    shouldBeEqual((uint8_t)state[1], (uint8_t)254);
    shouldBeEqual((uint8_t)state[2], (uint8_t)7);
    allocator->clearNFAState(state, baseState+9);
    shouldBeEqual((uint8_t)state[0], (uint8_t)0);
    shouldBeEqual((uint8_t)state[1], (uint8_t)252);
    shouldBeEqual((uint8_t)state[2], (uint8_t)7);
    iterator = allocator->newIteratorOn(state);
    for (size_t i = 10; i < 19; i++) {
      shouldBeEqual(iterator.nextState(), baseState+i);
    }
    // mapping is owned by allocator
    delete allocator;
    delete nfaBuilder;
    delete nfa;
    delete classifier;
  } endIt();

} endDescribe(DFA_NFAStateIterator);

}; // namespace DeterministicFiniteAutomaton
