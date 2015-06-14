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
      AssertThat(pdm->curState.token, Equals((void*)0));
      AssertThat(pdm->curState.stream, Equals((void*)0));
      AssertThat(pdm->curState.iterator, Equals((void*)0));
      AssertThat(pdm->curState.dState, Equals((void*)0));
      AssertThat(pdm->curState.allocator, Equals((void*)0));
      AssertThat(pdm->stack.getNumItems(), Equals(0));
    });

  }); // describe parser

});
