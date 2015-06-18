#include <bandit/bandit.h>
using namespace bandit;

#include <string.h>
#include <stdio.h>

#ifndef protected
#define protected public
#endif

#include "dynUtf8Parser/nfaBuilder.h"
#include <dynUtf8Parser/dfa/dfa.h>

namespace DeterministicFiniteAutomaton {

go_bandit([](){

  printf("\n----------------------------------\n");
  printf(  "dfa-nfaStateMapping\n");
  printf(  "NFAStateMapping = %zu bytes (%zu bits)\n", sizeof(NFAStateMapping), sizeof(NFAStateMapping)*8);
  printf(  "----------------------------------\n");

  /// \brief Test the NFAStateMapping class.
  describe("NFAStateMapping", [](){

    /// Show that we can create an appropriately allocated DFA
    /// from a given NFA.
    it("Should have correct sizes and pointers setup", [&](){
      Classifier *classifier = new Classifier();
      NFA *nfa = new NFA(classifier);
      NFABuilder *nfaBuilder = new NFABuilder(nfa);
      nfaBuilder->compileRegularExpressionForTokenId("start", "(abab|abbb)", 1);
      AssertThat(nfa->getNumberStates(), Is().EqualTo(11));
      StateAllocator *allocator = new StateAllocator(nfa);
      NFAStateMapping *stateMapping = allocator->nfaStateMapping;
      AssertThat(stateMapping, Is().Not().EqualTo((void*)0));
      AssertThat(stateMapping->allocator, Equals(allocator));
      AssertThat(stateMapping->nfaStatePtr2int, Is().Not().EqualTo((void*)0));
      AssertThat(stateMapping->int2nfaStatePtr, Is().Not().EqualTo((NFA::State**)0));
      AssertThat(stateMapping->numKnownNFAStates, Equals(0));
      // stateMapper is owned by allocator
      delete allocator;
      delete nfa;
      delete classifier;
    });

    /// Show that NFAStateMapping::getNFAStateNumber computes correct
    /// NFAStateMapping::NFAStateNumber(s). In particular we need to
    /// show that we can deal with *more* than 8 NFA::States (i.e. more
    /// than the first byte in the NFAStateNumber/DFA::DState bit set).
    it("Should compute correct NFAStateNumbers using getNFAStateNumber", [&](){
      Classifier *classifier = new Classifier();
      NFA *nfa = new NFA(classifier);
      NFABuilder *nfaBuilder = new NFABuilder(nfa);
      nfaBuilder->compileRegularExpressionForTokenId("start", "thisisasimpletest", 1);
      AssertThat(nfa->getNumberStates(), Is().EqualTo(19));
      StateAllocator *allocator = new StateAllocator(nfa);
      AssertThat(allocator, Is().Not().EqualTo((void*)0));
      NFAStateMapping *mapping = allocator->nfaStateMapping;
      AssertThat(mapping, Is().Not().EqualTo((void*)0));
      AssertThat(mapping->numKnownNFAStates, Is().EqualTo(0));
      NFA::State *nfaStartState = nfa->getStartState("start");
      AssertThat(nfaStartState, Is().Not().EqualTo((void*)0));
      mapping->getNFAStateNumber(nfaStartState);
      AssertThat(mapping->int2nfaStatePtr[0], Is().EqualTo(nfaStartState));
      long long tmpNFAState = (long long)nfaStartState;
      char nfaStatePtr[sizeof(NFA::State*)];
      for (size_t i = 0; i < sizeof(NFA::State*); i++) {
        nfaStatePtr[i] = tmpNFAState & 0xFF;
        tmpNFAState >>=8;
      }
      AssertThat(*hattrie_get(mapping->nfaStatePtr2int, nfaStatePtr, sizeof(NFA::State*)), Is().EqualTo(1));
      AssertThat(nfaStartState->out, Is().Not().EqualTo((void*)0));
      mapping->getNFAStateNumber(nfaStartState->out);
      AssertThat(mapping->int2nfaStatePtr[1], Is().EqualTo(nfaStartState->out));
      tmpNFAState = (long long)(nfaStartState->out);
      for (size_t i = 0; i < sizeof(NFA::State*); i++) {
        nfaStatePtr[i] = tmpNFAState & 0xFF;
        tmpNFAState >>=8;
      }
      AssertThat(*hattrie_get(mapping->nfaStatePtr2int, nfaStatePtr, sizeof(NFA::State*)), Is().EqualTo(2));

      AssertThat(mapping->int2nfaStatePtr[2], Is().EqualTo((void*)0));
      AssertThat(mapping->numKnownNFAStates, Is().EqualTo(2));
      NFAStateMapping::NFAStateNumber aStateNum = mapping->getNFAStateNumber(nfaStartState);
      AssertThat(mapping->numKnownNFAStates, Is().EqualTo(2));
      AssertThat(aStateNum.stateByte, Is().EqualTo(0));
      AssertThat((int)aStateNum.stateBit, Is().EqualTo(1));
      aStateNum = mapping->getNFAStateNumber(nfaStartState->out);
      AssertThat(mapping->numKnownNFAStates, Is().EqualTo(2));
      AssertThat(aStateNum.stateByte, Is().EqualTo(0));
      AssertThat((int)aStateNum.stateBit, Is().EqualTo(2));
      NFA::State *nextState = nfaStartState->out;
      AssertThat(nextState, Is().Not().EqualTo((void*)0));
      aStateNum = mapping->getNFAStateNumber(nextState->out);
      AssertThat(mapping->numKnownNFAStates, Is().EqualTo(3));
      AssertThat(aStateNum.stateByte, Is().EqualTo(0));
      AssertThat((int)aStateNum.stateBit, Is().EqualTo(4));
      nextState = nextState->out;
      AssertThat(nextState, Is().Not().EqualTo((void*)0));
      aStateNum = mapping->getNFAStateNumber(nextState->out);
      AssertThat(mapping->numKnownNFAStates, Is().EqualTo(4));
      AssertThat(aStateNum.stateByte, Is().EqualTo(0));
      AssertThat((int)aStateNum.stateBit, Is().EqualTo(8));
      // stateMapper is owned by allocator
      delete allocator;
      delete nfa;
      delete classifier;
    });

    it("Show that DFA::getNFAStateNumber can deal with lots of NFA::States", [&](){
      Classifier *classifier = new Classifier();
      NFA *nfa = new NFA(classifier);
      NFABuilder *nfaBuilder = new NFABuilder(nfa);
      nfaBuilder->compileRegularExpressionForTokenId("start", "thisisasimpletest", 1);
      AssertThat(nfa->getNumberStates(), Is().EqualTo(19));
      StateAllocator *allocator = new StateAllocator(nfa);
      AssertThat(allocator, Is().Not().EqualTo((void*)0));
      NFAStateMapping *mapping = allocator->nfaStateMapping;
      AssertThat(mapping, Is().Not().EqualTo((void*)0));
      NFA::State *baseState =
        (NFA::State*)nfa->stateAllocator->blocks.getTop();
      NFAStateMapping::NFAStateNumber aStateNum =
         mapping->getNFAStateNumber(baseState);
      AssertThat(aStateNum.stateByte, Is().EqualTo(0));
      AssertThat((int)aStateNum.stateBit,  Is().EqualTo(1));
      aStateNum = mapping->getNFAStateNumber(baseState+1);
      AssertThat(aStateNum.stateByte, Is().EqualTo(0));
      AssertThat((int)aStateNum.stateBit,  Is().EqualTo(2));
      aStateNum = mapping->getNFAStateNumber(baseState+2);
      AssertThat(aStateNum.stateByte, Is().EqualTo(0));
      AssertThat((int)aStateNum.stateBit,  Is().EqualTo(4));
      aStateNum = mapping->getNFAStateNumber(baseState+3);
      AssertThat(aStateNum.stateByte, Is().EqualTo(0));
      AssertThat((int)aStateNum.stateBit,  Is().EqualTo(8));
      aStateNum = mapping->getNFAStateNumber(baseState+4);
      AssertThat(aStateNum.stateByte, Is().EqualTo(0));
      AssertThat((int)aStateNum.stateBit,  Is().EqualTo(16));
      aStateNum = mapping->getNFAStateNumber(baseState+5);
      AssertThat(aStateNum.stateByte, Is().EqualTo(0));
      AssertThat((int)aStateNum.stateBit,  Is().EqualTo(32));
      aStateNum = mapping->getNFAStateNumber(baseState+6);
      AssertThat(aStateNum.stateByte, Is().EqualTo(0));
      AssertThat((int)aStateNum.stateBit,  Is().EqualTo(64));
      aStateNum = mapping->getNFAStateNumber(baseState+7);
      AssertThat(aStateNum.stateByte, Is().EqualTo(0));
      AssertThat((int)aStateNum.stateBit,  Is().EqualTo(128));
      aStateNum = mapping->getNFAStateNumber(baseState+8);
      AssertThat(aStateNum.stateByte, Is().EqualTo(1));
      AssertThat((int)aStateNum.stateBit,  Is().EqualTo(1));
      aStateNum = mapping->getNFAStateNumber(baseState+9);
      AssertThat(aStateNum.stateByte, Is().EqualTo(1));
      AssertThat((int)aStateNum.stateBit,  Is().EqualTo(2));
      aStateNum = mapping->getNFAStateNumber(baseState+10);
      AssertThat(aStateNum.stateByte, Is().EqualTo(1));
      AssertThat((int)aStateNum.stateBit,  Is().EqualTo(4));
      aStateNum = mapping->getNFAStateNumber(baseState+11);
      AssertThat(aStateNum.stateByte, Is().EqualTo(1));
      AssertThat((int)aStateNum.stateBit,  Is().EqualTo(8));
      aStateNum = mapping->getNFAStateNumber(baseState+12);
      AssertThat(aStateNum.stateByte, Is().EqualTo(1));
      AssertThat((int)aStateNum.stateBit,  Is().EqualTo(16));
      aStateNum = mapping->getNFAStateNumber(baseState+13);
      AssertThat(aStateNum.stateByte, Is().EqualTo(1));
      AssertThat((int)aStateNum.stateBit,  Is().EqualTo(32));
      aStateNum = mapping->getNFAStateNumber(baseState+14);
      AssertThat(aStateNum.stateByte, Is().EqualTo(1));
      AssertThat((int)aStateNum.stateBit,  Is().EqualTo(64));
      aStateNum = mapping->getNFAStateNumber(baseState+15);
      AssertThat(aStateNum.stateByte, Is().EqualTo(1));
      AssertThat((int)aStateNum.stateBit,  Is().EqualTo(128));
      aStateNum = mapping->getNFAStateNumber(baseState+16);
      AssertThat(aStateNum.stateByte, Is().EqualTo(2));
      AssertThat((int)aStateNum.stateBit,  Is().EqualTo(1));
      aStateNum = mapping->getNFAStateNumber(baseState+17);
      AssertThat(aStateNum.stateByte, Is().EqualTo(2));
      AssertThat((int)aStateNum.stateBit,  Is().EqualTo(2));
      aStateNum = mapping->getNFAStateNumber(baseState+18);
      AssertThat(aStateNum.stateByte, Is().EqualTo(2));
      AssertThat((int)aStateNum.stateBit,  Is().EqualTo(4));
      try {
        aStateNum = mapping->getNFAStateNumber(baseState+19);
        AssertThat(aStateNum.stateByte, Is().EqualTo(2));
        AssertThat((int)aStateNum.stateBit,  Is().EqualTo(16));
        AssertThat(false, Is().True());
      } catch (ParserException& e) {
        AssertThat(true, Is().True());
      }
      // stateMapper is owned by allocator
      delete allocator;
      delete nfa;
      delete classifier;
    });

  }); // dfa
});

}; // namespace DeterministicFiniteAutomaton
