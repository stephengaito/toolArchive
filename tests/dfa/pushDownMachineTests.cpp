#include <bandit/bandit.h>
using namespace bandit;

#include <string.h>
#include <stdio.h>
#include <exception>

#ifndef protected
#define protected public
#endif

#include "nfaBuilder.h"
#include "dfa/pushDownMachine.h"

using namespace DeterministicFiniteAutomaton;

// TODO: TEST MEMORY LEAKS (using C++ mocks)

go_bandit([](){

  printf("\n----------------------------------\n");
  printf(  "PushDownMachine\n");
  printf(  "PushDownMachine = %zu bytes (%zu bits)\n", sizeof(PushDownMachine),  sizeof(PushDownMachine)*8);
  printf(  "----------------------------------\n");

  /// \brief We test the AutomataState class.
  describe("PushDownMachine", [](){

    it("Should create an instance", [](){
      Classifier *classifier = new Classifier();
      NFA *nfa = new NFA(classifier);
      NFABuilder *nfaBuilder = new NFABuilder(nfa);
      nfaBuilder->compileRegularExpressionForTokenId("start", "(abab|abbb)", 1);
      DFA *dfa = new DFA(nfa);
      PushDownMachine *pdm = new PushDownMachine(dfa);
      AssertThat(pdm, Is().Not().EqualTo((void*)0));
      AssertThat(pdm->dfa, Equals(dfa));
      AssertThat(pdm->nfa, Equals(nfa));
      AssertThat(pdm->allocator, Equals(dfa->allocator));
      AssertThat(pdm->curState.message, Equals((void*)0));
      AssertThat(pdm->curState.token, Equals((void*)0));
      AssertThat(pdm->curState.stream, Equals((void*)0));
      AssertThat(pdm->curState.iterator, Equals((void*)0));
      AssertThat(pdm->curState.dState, Equals((void*)0));
      AssertThat(pdm->curState.allocator, Equals((void*)0));
      AssertThat(pdm->stack.getNumItems(), Equals(0));
    });

    it("Should be able to push/pop AutomataState", [](){
      Classifier *classifier = new Classifier();
      NFA *nfa = new NFA(classifier);
      NFABuilder *nfaBuilder = new NFABuilder(nfa);
      nfaBuilder->compileRegularExpressionForTokenId("start", "(abab|abbb)", 1);
      nfaBuilder->compileRegularExpressionForTokenId("other", "otherSimple", 1);
      nfaBuilder->compileRegularExpressionForTokenId("another", "anotherSimple", 1);
      DFA *dfa = new DFA(nfa);
      PushDownMachine *pdm = new PushDownMachine(dfa);
      AssertThat(pdm, Is().Not().EqualTo((void*)0));
      AssertThat(pdm->dfa, Equals(dfa));
      AssertThat(pdm->nfa, Equals(nfa));
      AssertThat(pdm->allocator, Equals(dfa->allocator));
      AssertThat(pdm->curState.message, Equals((void*)0));
      AssertThat(pdm->curState.token, Equals((void*)0));
      AssertThat(pdm->curState.stream, Equals((void*)0));
      AssertThat(pdm->curState.iterator, Equals((void*)0));
      AssertThat(pdm->curState.dState, Equals((void*)0));
      AssertThat(pdm->curState.allocator, Equals((void*)0));
      AssertThat(pdm->stack.getNumItems(), Equals(0));
      StateAllocator *allocator = dfa->getStateAllocator();
      Utf8Chars *someChars = new Utf8Chars("some characters");
      NFA::StartStateId startStateId = nfa->findStartStateId("start");
      NFA::State *nfaState = nfa->getStartState(startStateId);
      const char* testMessage = nfaState->message;
      State *dState = dfa->getDFAStartState(startStateId);
      pdm->curState.initialize(dfa, someChars, startStateId);
      AssertThat(pdm->curState.message, Is().Not().EqualTo((char*)testMessage));
      AssertThat(pdm->curState.message, Equals(testMessage));
      AssertThat(pdm->curState.token, Is().Not().EqualTo((void*)0));
      AssertThat(pdm->curState.stream, Is().Not().EqualTo((void*)0));
      AssertThat(pdm->curState.iterator, Is().Not().EqualTo((void*)0));
      AssertThat(pdm->curState.dState, Is().Not().EqualTo(dState));
      AssertThat(pdm->curState.allocator, Equals(allocator));
      State *otherDState = dfa->getDFAStartState("other");
      const char *otherMessage = "this is another message";
      AutomataState oldCurState0 = pdm->curState;
      AssertThat(oldCurState0.allocator, Equals(pdm->curState.allocator));
      AssertThat(oldCurState0.iterator,  Equals(pdm->curState.iterator));
      AssertThat(oldCurState0.dState,    Equals(pdm->curState.dState));
      AssertThat(oldCurState0.stream,    Equals(pdm->curState.stream));
      AssertThat(oldCurState0.token,     Equals(pdm->curState.token));
      AssertThat(oldCurState0.message,   Equals((char*)pdm->curState.message));
      pdm->push(NULL, otherDState, otherMessage);
      AssertThat(pdm->stack.getNumItems(), Equals(1));
      AssertThat(pdm->stack.numItems, Equals(1));
      AssertThat(pdm->stack.itemArray[pdm->stack.numItems-1].allocator,
        Equals(oldCurState0.allocator));
      AssertThat(pdm->stack.itemArray[pdm->stack.numItems-1].iterator,
        Equals(oldCurState0.iterator));
      AssertThat(pdm->stack.itemArray[pdm->stack.numItems-1].dState,
        Equals(oldCurState0.dState));
      AssertThat(pdm->stack.itemArray[pdm->stack.numItems-1].stream,
        Equals(oldCurState0.stream));
      AssertThat(pdm->stack.itemArray[pdm->stack.numItems-1].token,
        Equals(oldCurState0.token));
      AssertThat(pdm->stack.itemArray[pdm->stack.numItems-1].message,
        Equals(oldCurState0.message));
      AssertThat(pdm->curState.message, Is().Not().EqualTo((char*)testMessage));
      AssertThat(pdm->curState.message, Is().Not().EqualTo((char*)otherMessage));
      AssertThat(pdm->curState.message, Equals(otherMessage));
      AssertThat(pdm->curState.token, Is().Not().EqualTo((void*)0));
      AssertThat(pdm->curState.stream, Is().Not().EqualTo((void*)0));
      AssertThat(pdm->curState.iterator, Is().Not().EqualTo((void*)0));
      AssertThat(pdm->curState.dState, Is().Not().EqualTo(dState));
      AssertThat(pdm->curState.dState, Is().Not().EqualTo(otherDState));
      AssertThat(pdm->curState.allocator, Equals(allocator));
      State *anotherDState = dfa->getDFAStartState("another");
      const char *anotherMessage = "this is yet another message";
      AutomataState oldCurState1 = pdm->curState;
      AssertThat(oldCurState1.allocator, Equals(pdm->curState.allocator));
      AssertThat(oldCurState1.iterator,  Equals(pdm->curState.iterator));
      AssertThat(oldCurState1.dState,    Equals(pdm->curState.dState));
      AssertThat(oldCurState1.stream,    Equals(pdm->curState.stream));
      AssertThat(oldCurState1.token,     Equals(pdm->curState.token));
      AssertThat(oldCurState1.message,   Equals((char*)pdm->curState.message));
      pdm->push(NULL, anotherDState, anotherMessage);
      AssertThat(pdm->stack.getNumItems(), Equals(2));
      AssertThat(pdm->stack.numItems, Equals(2));
      AssertThat(pdm->stack.itemArray[pdm->stack.numItems-1].allocator,
        Equals(oldCurState1.allocator));
      AssertThat(pdm->stack.itemArray[pdm->stack.numItems-1].iterator,
        Equals(oldCurState1.iterator));
      AssertThat(pdm->stack.itemArray[pdm->stack.numItems-1].dState,
        Equals(oldCurState1.dState));
      AssertThat(pdm->stack.itemArray[pdm->stack.numItems-1].stream,
        Equals(oldCurState1.stream));
      AssertThat(pdm->stack.itemArray[pdm->stack.numItems-1].token,
        Equals(oldCurState1.token));
      AssertThat(pdm->stack.itemArray[pdm->stack.numItems-1].message,
        Equals(oldCurState1.message));
      AssertThat(pdm->curState.message, Is().Not().EqualTo((char*)testMessage));
      AssertThat(pdm->curState.message, Is().Not().EqualTo((char*)otherMessage));
      AssertThat(pdm->curState.message, Is().Not().EqualTo((char*)anotherMessage));
      AssertThat(pdm->curState.message, Equals(anotherMessage));
      AssertThat(pdm->curState.token,  Is().Not().EqualTo((void*)0));
      AssertThat(pdm->curState.stream, Is().Not().EqualTo((void*)0));
      AssertThat(pdm->curState.iterator, Is().Not().EqualTo((void*)0));
      AssertThat(pdm->curState.dState, Is().Not().EqualTo(dState));
      AssertThat(pdm->curState.dState, Is().Not().EqualTo(otherDState));
      AssertThat(pdm->curState.dState, Is().Not().EqualTo(anotherDState));
      AssertThat(pdm->curState.allocator, Equals(allocator));
      pdm->swap(NULL);
      AssertThat(pdm->stack.getNumItems(), Equals(2));
      AssertThat(pdm->stack.numItems, Equals(2));
      AssertThat(pdm->stack.itemArray[pdm->stack.numItems-1].allocator,
        Equals(oldCurState0.allocator));
      AssertThat(pdm->stack.itemArray[pdm->stack.numItems-1].iterator,
        Equals(oldCurState0.iterator));
      AssertThat(pdm->stack.itemArray[pdm->stack.numItems-1].dState,
        Equals(oldCurState0.dState));
      AssertThat(pdm->stack.itemArray[pdm->stack.numItems-1].stream,
        Equals(oldCurState0.stream));
      AssertThat(pdm->stack.itemArray[pdm->stack.numItems-1].token,
        Equals(oldCurState0.token));
      AssertThat(pdm->stack.itemArray[pdm->stack.numItems-1].message,
        Equals(oldCurState0.message));
      AssertThat(pdm->stack.itemArray[pdm->stack.numItems-2].allocator,
        Equals(oldCurState1.allocator));
      AssertThat(pdm->stack.itemArray[pdm->stack.numItems-2].iterator,
        Equals(oldCurState1.iterator));
      AssertThat(pdm->stack.itemArray[pdm->stack.numItems-2].dState,
        Equals(oldCurState1.dState));
      AssertThat(pdm->stack.itemArray[pdm->stack.numItems-2].stream,
        Equals(oldCurState1.stream));
      AssertThat(pdm->stack.itemArray[pdm->stack.numItems-2].token,
        Equals(oldCurState1.token));
      AssertThat(pdm->stack.itemArray[pdm->stack.numItems-2].message,
        Equals(oldCurState1.message));
      pdm->swap(NULL);
      pdm->pop(NULL, false);
      AssertThat(pdm->stack.getNumItems(), Equals(1));
      AssertThat(oldCurState1.allocator, Equals(pdm->curState.allocator));
      AssertThat(oldCurState1.iterator,  Equals(pdm->curState.iterator));
      AssertThat(oldCurState1.dState,    Equals(pdm->curState.dState));
      AssertThat(oldCurState1.stream,    Equals(pdm->curState.stream));
      AssertThat(oldCurState1.token,     Equals(pdm->curState.token));
      AssertThat(oldCurState1.message,   Equals((char*)pdm->curState.message));
      AssertThat(pdm->curState.message, Is().Not().EqualTo((char*)testMessage));
      AssertThat(pdm->curState.message, Is().Not().EqualTo((char*)otherMessage));
      AssertThat(pdm->curState.message, Is().Not().EqualTo((char*)anotherMessage));
      AssertThat(pdm->curState.message, Equals(otherMessage));
      AssertThat(pdm->curState.token,  Is().Not().EqualTo((void*)0));
      AssertThat(pdm->curState.stream, Is().Not().EqualTo((void*)0));
      AssertThat(pdm->curState.iterator, Is().Not().EqualTo((void*)0));
      AssertThat(pdm->curState.dState, Is().Not().EqualTo(dState));
      AssertThat(pdm->curState.dState, Is().Not().EqualTo(otherDState));
      AssertThat(pdm->curState.dState, Is().Not().EqualTo(anotherDState));
      AssertThat(pdm->curState.allocator, Equals(allocator));
      pdm->pop(NULL, false);
      AssertThat(pdm->stack.getNumItems(), Equals(0));
      AssertThat(oldCurState0.allocator, Equals(pdm->curState.allocator));
      AssertThat(oldCurState0.iterator,  Equals(pdm->curState.iterator));
      AssertThat(oldCurState0.dState,    Equals(pdm->curState.dState));
      AssertThat(oldCurState0.stream,    Equals(pdm->curState.stream));
      AssertThat(oldCurState0.token,     Equals(pdm->curState.token));
      AssertThat(oldCurState0.message,   Equals((char*)pdm->curState.message));
      AssertThat(pdm->curState.message, Is().Not().EqualTo((char*)testMessage));
      AssertThat(pdm->curState.message, Is().Not().EqualTo((char*)otherMessage));
      AssertThat(pdm->curState.message, Equals(testMessage));
      AssertThat(pdm->curState.token,  Is().Not().EqualTo((void*)0));
      AssertThat(pdm->curState.stream, Is().Not().EqualTo((void*)0));
      AssertThat(pdm->curState.iterator, Is().Not().EqualTo((void*)0));
      AssertThat(pdm->curState.dState, Is().Not().EqualTo(dState));
      AssertThat(pdm->curState.dState, Is().Not().EqualTo(otherDState));
      AssertThat(pdm->curState.allocator, Equals(allocator));
    });

  }); // describe parser

});
