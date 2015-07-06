#include <string.h>
#include <stdio.h>
#include <exception>

#include <cUtils/specs/specs.h>

#ifndef protected
#define protected public
#endif

#include "dynUtf8Parser/nfaBuilder.h"
#include "dynUtf8Parser/dfa/pushDownMachine.h"

using namespace DeterministicFiniteAutomaton;

/// \brief We test the AutomataState class.
describe(PushDownMachine) {

  specSize(PushDownMachine);

  it("Should create an instance") {
    Classifier *classifier = new Classifier();
    shouldNotBeNULL(classifier);
    NFA *nfa = new NFA(classifier);
    shouldNotBeNULL(nfa);
    NFABuilder *nfaBuilder = new NFABuilder(nfa);
    shouldNotBeNULL(nfaBuilder);
    nfaBuilder->compileRegularExpressionForTokenId("start", "(abab|abbb)", 1);
    DFA *dfa = new DFA(nfa);
    shouldNotBeNULL(dfa);
    PushDownMachine *pdm = new PushDownMachine(dfa);
    shouldNotBeNULL(pdm);
    shouldBeEqual(pdm->dfa, dfa);
    shouldBeEqual(pdm->nfa, nfa);
    shouldBeEqual(pdm->allocator, dfa->allocator);
    shouldBeNULL(pdm->curState.token);
    shouldBeNULL(pdm->curState.stream);
    shouldBeNULL(pdm->curState.iterator);
    shouldBeNULL(pdm->curState.dState);
    shouldBeNULL(pdm->curState.allocator);
    shouldBeZero(pdm->stack.getNumItems());
    delete pdm;
    delete dfa;
    delete nfaBuilder;
    delete nfa;
    delete classifier;
  } endIt();

} endDescribe(PushDownMachine);
