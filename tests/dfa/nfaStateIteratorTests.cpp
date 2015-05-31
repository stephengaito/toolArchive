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
  printf(  "dfa-nfaStateIterator\n");
  printf(  "NFAStateIterator = %zu bytes (%zu bits)\n", sizeof(NFAStateIterator), sizeof(NFAStateIterator)*8);
  printf(  "----------------------------------\n");

  /// \brief Test the NFAStateIterator class.
  describe("NFAStateIterator", [](){

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
      NFAStateMapping *mapping = allocator->nfaStateMapping;
      AssertThat(mapping, Is().Not().EqualTo((void*)0));
      State *state = allocator->allocateANewState();
      AssertThat(state, Is().Not().EqualTo((void*)0));
      NFAStateIterator iterator = allocator->newIteratorOn(state);
      AssertThat(iterator.curNFAStateNum, Equals(0));
      AssertThat(iterator.curByte, Equals(state));
      AssertThat(iterator.endByte, Equals(state+allocator->stateSize));
      AssertThat(iterator.curBit, Equals(1));
      // mapping is owned by allocator
      delete allocator;
      delete nfa;
      delete classifier;
    });

    it("Show that DFA::getNFAStateNumber can deal with lots of NFA::States", [&](){
      Classifier *classifier = new Classifier();
      NFA *nfa = new NFA(classifier);
      NFABuilder *nfaBuilder = new NFABuilder(nfa);
      nfa->appendNFAToStartState(nfaBuilder->compileRegularExpressionForTokenId("thisisasimpletest", 1));
      AssertThat(nfa->getNumberStates(), Is().EqualTo(19));
      StateAllocator *allocator = new StateAllocator(nfa);
      AssertThat(allocator, Is().Not().EqualTo((void*)0));
      AssertThat(allocator->stateSize, Equals(3));
      NFAStateMapping *mapping = allocator->nfaStateMapping;
      AssertThat(mapping, Is().Not().EqualTo((void*)0));
      State *state = allocator->allocateANewState();
      AssertThat(state, Is().Not().EqualTo((void*)0));
      NFA::State *baseState =
       (NFA::State*)nfa->stateAllocator->blocks[nfa->stateAllocator->nextBlock - 1];
      NFAStateMapping::NFAStateNumber aStateNum =
        mapping->getNFAStateNumber(baseState);
      AssertThat(aStateNum.stateByte, Is().EqualTo(0));
      AssertThat((uint8_t)aStateNum.stateBit,  Is().EqualTo(1));
      allocator->setNFAState(state, baseState);
      AssertThat((uint8_t)state[0], Equals(1));
      aStateNum = mapping->getNFAStateNumber(baseState+1);
      AssertThat(aStateNum.stateByte, Is().EqualTo(0));
      AssertThat((uint8_t)aStateNum.stateBit,  Is().EqualTo(2));
      allocator->setNFAState(state, baseState+1);
      AssertThat((uint8_t)state[0], Equals(3));
      aStateNum = mapping->getNFAStateNumber(baseState+2);
      AssertThat(aStateNum.stateByte, Is().EqualTo(0));
      AssertThat((uint8_t)aStateNum.stateBit,  Is().EqualTo(4));
      allocator->setNFAState(state, baseState+2);
      AssertThat((uint8_t)state[0], Equals(7));
      aStateNum = mapping->getNFAStateNumber(baseState+3);
      AssertThat(aStateNum.stateByte, Is().EqualTo(0));
      AssertThat((uint8_t)aStateNum.stateBit,  Is().EqualTo(8));
      allocator->setNFAState(state, baseState+3);
      AssertThat((uint8_t)state[0], Equals(15));
      aStateNum = mapping->getNFAStateNumber(baseState+4);
      AssertThat(aStateNum.stateByte, Is().EqualTo(0));
      AssertThat((uint8_t)aStateNum.stateBit,  Is().EqualTo(16));
      allocator->setNFAState(state, baseState+4);
      AssertThat((uint8_t)state[0], Equals(31));
      aStateNum = mapping->getNFAStateNumber(baseState+5);
      AssertThat(aStateNum.stateByte, Is().EqualTo(0));
      AssertThat((uint8_t)aStateNum.stateBit,  Is().EqualTo(32));
      allocator->setNFAState(state, baseState+5);
      AssertThat((uint8_t)state[0], Equals(63));
      aStateNum = mapping->getNFAStateNumber(baseState+6);
      AssertThat(aStateNum.stateByte, Is().EqualTo(0));
      AssertThat((uint8_t)aStateNum.stateBit,  Is().EqualTo(64));
      allocator->setNFAState(state, baseState+6);
      AssertThat((uint8_t)state[0], Equals(127));
      aStateNum = mapping->getNFAStateNumber(baseState+7);
      AssertThat(aStateNum.stateByte, Is().EqualTo(0));
      AssertThat((uint8_t)aStateNum.stateBit,  Is().EqualTo(128));
      allocator->setNFAState(state, baseState+7);
      AssertThat((uint8_t)state[0], Equals(255));
      aStateNum = mapping->getNFAStateNumber(baseState+8);
      AssertThat(aStateNum.stateByte, Is().EqualTo(1));
      AssertThat((uint8_t)aStateNum.stateBit,  Is().EqualTo(1));
      allocator->setNFAState(state, baseState+8);
      AssertThat((uint8_t)state[0], Equals(255));
      AssertThat((uint8_t)state[1], Equals(1));
      aStateNum = mapping->getNFAStateNumber(baseState+9);
      AssertThat(aStateNum.stateByte, Is().EqualTo(1));
      AssertThat((uint8_t)aStateNum.stateBit,  Is().EqualTo(2));
      allocator->setNFAState(state, baseState+9);
      AssertThat((uint8_t)state[0], Equals(255));
      AssertThat((uint8_t)state[1], Equals(3));
      aStateNum = mapping->getNFAStateNumber(baseState+10);
      AssertThat(aStateNum.stateByte, Is().EqualTo(1));
      AssertThat((uint8_t)aStateNum.stateBit,  Is().EqualTo(4));
      allocator->setNFAState(state, baseState+10);
      AssertThat((uint8_t)state[0], Equals(255));
      AssertThat((uint8_t)state[1], Equals(7));
      aStateNum = mapping->getNFAStateNumber(baseState+11);
      AssertThat(aStateNum.stateByte, Is().EqualTo(1));
      AssertThat((uint8_t)aStateNum.stateBit,  Is().EqualTo(8));
      allocator->setNFAState(state, baseState+11);
      AssertThat((uint8_t)state[0], Equals(255));
      AssertThat((uint8_t)state[1], Equals(15));
      aStateNum = mapping->getNFAStateNumber(baseState+12);
      AssertThat(aStateNum.stateByte, Is().EqualTo(1));
      AssertThat((uint8_t)aStateNum.stateBit,  Is().EqualTo(16));
      allocator->setNFAState(state, baseState+12);
      AssertThat((uint8_t)state[0], Equals(255));
      AssertThat((uint8_t)state[1], Equals(31));
      aStateNum = mapping->getNFAStateNumber(baseState+13);
      AssertThat(aStateNum.stateByte, Is().EqualTo(1));
      AssertThat((uint8_t)aStateNum.stateBit,  Is().EqualTo(32));
      allocator->setNFAState(state, baseState+13);
      AssertThat((uint8_t)state[0], Equals(255));
      AssertThat((uint8_t)state[1], Equals(63));
      aStateNum = mapping->getNFAStateNumber(baseState+14);
      AssertThat(aStateNum.stateByte, Is().EqualTo(1));
      AssertThat((uint8_t)aStateNum.stateBit,  Is().EqualTo(64));
      allocator->setNFAState(state, baseState+14);
      AssertThat((uint8_t)state[0], Equals(255));
      AssertThat((uint8_t)state[1], Equals(127));
      aStateNum = mapping->getNFAStateNumber(baseState+15);
      AssertThat(aStateNum.stateByte, Is().EqualTo(1));
      AssertThat((uint8_t)aStateNum.stateBit,  Is().EqualTo(128));
      allocator->setNFAState(state, baseState+15);
      AssertThat((uint8_t)state[0], Equals(255));
      AssertThat((uint8_t)state[1], Equals(255));
      aStateNum = mapping->getNFAStateNumber(baseState+16);
      AssertThat(aStateNum.stateByte, Is().EqualTo(2));
      AssertThat((uint8_t)aStateNum.stateBit,  Is().EqualTo(1));
      allocator->setNFAState(state, baseState+16);
      AssertThat((uint8_t)state[0], Equals(255));
      AssertThat((uint8_t)state[1], Equals(255));
      AssertThat((uint8_t)state[2], Equals(1));
      aStateNum = mapping->getNFAStateNumber(baseState+17);
      AssertThat(aStateNum.stateByte, Is().EqualTo(2));
      AssertThat((uint8_t)aStateNum.stateBit,  Is().EqualTo(2));
      allocator->setNFAState(state, baseState+17);
      AssertThat((uint8_t)state[0], Equals(255));
      AssertThat((uint8_t)state[1], Equals(255));
      AssertThat((uint8_t)state[2], Equals(3));
      aStateNum = mapping->getNFAStateNumber(baseState+18);
      AssertThat(aStateNum.stateByte, Is().EqualTo(2));
      AssertThat((uint8_t)aStateNum.stateBit,  Is().EqualTo(4));
      allocator->setNFAState(state, baseState+18);
      AssertThat((uint8_t)state[0], Equals(255));
      AssertThat((uint8_t)state[1], Equals(255));
      AssertThat((uint8_t)state[2], Equals(7));
      NFAStateIterator iterator = allocator->newIteratorOn(state);
      for (size_t i = 0; i < 19; i++) {
        AssertThat(iterator.nextState(), Equals(baseState+i));
      }
      AssertThat(iterator.nextState(), Equals((void*)0));
      // mapping is owned by allocator
      delete allocator;
      delete nfa;
      delete classifier;
    });

  }); // dfa
});

}; // namespace DeterministicFiniteAutomaton
