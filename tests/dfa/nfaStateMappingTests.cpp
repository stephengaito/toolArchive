#include <string.h>
#include <stdio.h>

#include <cUtils/specs/specs.h>

#ifndef protected
#define protected public
#endif

#include "dynUtf8Parser/nfaBuilder.h"
#include <dynUtf8Parser/dfa/dfa.h>

namespace DeterministicFiniteAutomaton {


/// \brief Test the NFAStateMapping class.
describe(DFA_NFAStateMapping) {

  specSize(NFAStateMapping);

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
    NFAStateMapping *stateMapping = allocator->nfaStateMapping;
    shouldNotBeNULL(stateMapping);
    shouldBeEqual(stateMapping->allocator, allocator);
    shouldNotBeNULL(stateMapping->nfaStatePtr2int);
    shouldNotBeNULL(stateMapping->int2nfaStatePtr);
    shouldBeZero(stateMapping->numKnownNFAStates);
    // stateMapper is owned by allocator
    delete allocator;
    delete nfaBuilder;
    delete nfa;
    delete classifier;
  } endIt();

  /// Show that NFAStateMapping::getNFAStateNumber computes correct
  /// NFAStateMapping::NFAStateNumber(s). In particular we need to
  /// show that we can deal with *more* than 8 NFA::States (i.e. more
  /// than the first byte in the NFAStateNumber/DFA::DState bit set).
  it("Should compute correct NFAStateNumbers using getNFAStateNumber") {
    Classifier *classifier = new Classifier();
    shouldNotBeNULL(classifier);
    NFA *nfa = new NFA(classifier);
    shouldNotBeNULL(nfa);
    NFABuilder *nfaBuilder = new NFABuilder(nfa);
    shouldNotBeNULL(nfaBuilder);
    nfaBuilder->compileRegularExpressionForTokenId("start",
      "thisisasimpletest", 1);
    shouldBeEqual(nfa->getNumberStates(), 19);
    StateAllocator *allocator = new StateAllocator(nfa);
    shouldNotBeNULL(allocator);
    NFAStateMapping *mapping = allocator->nfaStateMapping;
    shouldNotBeNULL(mapping);
    shouldBeZero(mapping->numKnownNFAStates);
    NFA::State *nfaStartState = nfa->getStartState("start");
    shouldNotBeNULL(nfaStartState);
    mapping->getNFAStateNumber(nfaStartState);
    shouldBeEqual(mapping->int2nfaStatePtr[0], nfaStartState);
    long long tmpNFAState = (long long)nfaStartState;
    char nfaStatePtr[sizeof(NFA::State*)];
    for (size_t i = 0; i < sizeof(NFA::State*); i++) {
      nfaStatePtr[i] = tmpNFAState & 0xFF;
      tmpNFAState >>=8;
    }
    shouldBeEqual(*hattrie_get(mapping->nfaStatePtr2int,
                               nfaStatePtr, sizeof(NFA::State*)), 1);
    shouldNotBeNULL(nfaStartState->out);
    mapping->getNFAStateNumber(nfaStartState->out);
    shouldBeEqual(mapping->int2nfaStatePtr[1], nfaStartState->out);
    tmpNFAState = (long long)(nfaStartState->out);
    for (size_t i = 0; i < sizeof(NFA::State*); i++) {
      nfaStatePtr[i] = tmpNFAState & 0xFF;
      tmpNFAState >>=8;
    }
    shouldBeEqual(*hattrie_get(mapping->nfaStatePtr2int,
                               nfaStatePtr, sizeof(NFA::State*)), 2);

    shouldBeNULL(mapping->int2nfaStatePtr[2]);
    shouldBeEqual(mapping->numKnownNFAStates, 2);
    NFAStateMapping::NFAStateNumber aStateNum =
      mapping->getNFAStateNumber(nfaStartState);
    shouldBeEqual(mapping->numKnownNFAStates, 2);
    shouldBeEqual(aStateNum.stateByte, 0);
    shouldBeEqual((int)aStateNum.stateBit, 1);
    aStateNum = mapping->getNFAStateNumber(nfaStartState->out);
    shouldBeEqual(mapping->numKnownNFAStates, 2);
    shouldBeEqual(aStateNum.stateByte, 0);
    shouldBeEqual((int)aStateNum.stateBit, 2);
    NFA::State *nextState = nfaStartState->out;
    shouldNotBeNULL(nextState);
    aStateNum = mapping->getNFAStateNumber(nextState->out);
    shouldBeEqual(mapping->numKnownNFAStates, 3);
    shouldBeEqual(aStateNum.stateByte, 0);
    shouldBeEqual((int)aStateNum.stateBit, 4);
    nextState = nextState->out;
    shouldNotBeNULL(nextState);
    aStateNum = mapping->getNFAStateNumber(nextState->out);
    shouldBeEqual(mapping->numKnownNFAStates, 4);
    shouldBeEqual(aStateNum.stateByte, 0);
    shouldBeEqual((int)aStateNum.stateBit, 8);
    // stateMapper is owned by allocator
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
    nfaBuilder->compileRegularExpressionForTokenId("start",
      "thisisasimpletest", 1);
    shouldBeEqual(nfa->getNumberStates(), 19);
    StateAllocator *allocator = new StateAllocator(nfa);
    shouldNotBeNULL(allocator);
    NFAStateMapping *mapping = allocator->nfaStateMapping;
    shouldNotBeNULL(mapping);
    NFA::State *baseState =
      (NFA::State*)nfa->stateAllocator->blocks.getTop();
    shouldNotBeNULL(baseState);
    NFAStateMapping::NFAStateNumber aStateNum =
       mapping->getNFAStateNumber(baseState);
    shouldBeEqual(aStateNum.stateByte, 0);
    shouldBeEqual((int)aStateNum.stateBit,  1);
    aStateNum = mapping->getNFAStateNumber(baseState+1);
    shouldBeEqual(aStateNum.stateByte, 0);
    shouldBeEqual((int)aStateNum.stateBit,  2);
    aStateNum = mapping->getNFAStateNumber(baseState+2);
    shouldBeEqual(aStateNum.stateByte, 0);
    shouldBeEqual((int)aStateNum.stateBit,  4);
    aStateNum = mapping->getNFAStateNumber(baseState+3);
    shouldBeEqual(aStateNum.stateByte, 0);
    shouldBeEqual((int)aStateNum.stateBit,  8);
    aStateNum = mapping->getNFAStateNumber(baseState+4);
    shouldBeEqual(aStateNum.stateByte, 0);
    shouldBeEqual((int)aStateNum.stateBit,  16);
    aStateNum = mapping->getNFAStateNumber(baseState+5);
    shouldBeEqual(aStateNum.stateByte, 0);
    shouldBeEqual((int)aStateNum.stateBit,  32);
    aStateNum = mapping->getNFAStateNumber(baseState+6);
    shouldBeEqual(aStateNum.stateByte, 0);
    shouldBeEqual((int)aStateNum.stateBit,  64);
    aStateNum = mapping->getNFAStateNumber(baseState+7);
    shouldBeEqual(aStateNum.stateByte, 0);
    shouldBeEqual((int)aStateNum.stateBit,  128);
    aStateNum = mapping->getNFAStateNumber(baseState+8);
    shouldBeEqual(aStateNum.stateByte, 1);
    shouldBeEqual((int)aStateNum.stateBit,  1);
    aStateNum = mapping->getNFAStateNumber(baseState+9);
    shouldBeEqual(aStateNum.stateByte, 1);
    shouldBeEqual((int)aStateNum.stateBit,  2);
    aStateNum = mapping->getNFAStateNumber(baseState+10);
    shouldBeEqual(aStateNum.stateByte, 1);
    shouldBeEqual((int)aStateNum.stateBit,  4);
    aStateNum = mapping->getNFAStateNumber(baseState+11);
    shouldBeEqual(aStateNum.stateByte, 1);
    shouldBeEqual((int)aStateNum.stateBit,  8);
    aStateNum = mapping->getNFAStateNumber(baseState+12);
    shouldBeEqual(aStateNum.stateByte, 1);
    shouldBeEqual((int)aStateNum.stateBit,  16);
    aStateNum = mapping->getNFAStateNumber(baseState+13);
    shouldBeEqual(aStateNum.stateByte, 1);
    shouldBeEqual((int)aStateNum.stateBit,  32);
    aStateNum = mapping->getNFAStateNumber(baseState+14);
    shouldBeEqual(aStateNum.stateByte, 1);
    shouldBeEqual((int)aStateNum.stateBit,  64);
    aStateNum = mapping->getNFAStateNumber(baseState+15);
    shouldBeEqual(aStateNum.stateByte, 1);
    shouldBeEqual((int)aStateNum.stateBit,  128);
    aStateNum = mapping->getNFAStateNumber(baseState+16);
    shouldBeEqual(aStateNum.stateByte, 2);
    shouldBeEqual((int)aStateNum.stateBit,  1);
    aStateNum = mapping->getNFAStateNumber(baseState+17);
    shouldBeEqual(aStateNum.stateByte, 2);
    shouldBeEqual((int)aStateNum.stateBit,  2);
    aStateNum = mapping->getNFAStateNumber(baseState+18);
    shouldBeEqual(aStateNum.stateByte, 2);
    shouldBeEqual((int)aStateNum.stateBit,  4);
    try {
      aStateNum = mapping->getNFAStateNumber(baseState+19);
      shouldBeEqual(aStateNum.stateByte, 2);
      shouldBeEqual((int)aStateNum.stateBit,  16);
      shouldNotReachThisPoint("should have thrown ParserException");
    } catch (ParserException& e) {
      shouldReachThisPoint();
    }
    // stateMapper is owned by allocator
    delete allocator;
    delete nfaBuilder;
    delete nfa;
    delete classifier;
  } endIt();

} endDescribe(DFA_NFAStateMapping);

}; // namespace DeterministicFiniteAutomaton
