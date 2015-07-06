#include <string.h>
#include <stdio.h>
#include <exception>

#include <cUtils/specs/specs.h>

#ifndef protected
#define protected public
#endif

#include "dynUtf8Parser/nfaBuilder.h"
#include "dynUtf8Parser/dfa/automataState.h"

using namespace DeterministicFiniteAutomaton;

/// \brief We test the AutomataState class.
describe(AutomataState) {

  specSize(AutomataState);

  it("Create a NULL AutomataState and initialize it") {
    AutomataState automataState;
    shouldBeNULL(automataState.token);
    shouldBeNULL(automataState.stream);
    shouldBeNULL(automataState.iterator);
    shouldBeNULL(automataState.dState);
    shouldBeNULL(automataState.allocator);
    Classifier *classifier = new Classifier();
    shouldNotBeNULL(classifier);
    NFA *nfa = new NFA(classifier);
    shouldNotBeNULL(nfa);
    NFABuilder *nfaBuilder = new NFABuilder(nfa);
    shouldNotBeNULL(nfaBuilder);
    nfaBuilder->compileRegularExpressionForTokenId("start", "simple", 1);
    DFA *dfa = new DFA(nfa);
    shouldNotBeNULL(dfa);
    StateAllocator *allocator = dfa->getStateAllocator();
    Utf8Chars *someChars = new Utf8Chars("some characters");
    NFA::StartStateId startStateId = nfa->findStartStateId("start");
    State *dState = dfa->getDFAStartState(startStateId);
    automataState.initialize(dfa, someChars, startStateId);
    shouldBeEqual(automataState.dfa, dfa);
    shouldBeEqual(automataState.allocator, allocator);
    shouldNotBeNULL(automataState.token);
    shouldNotBeNULL(automataState.stream);
    shouldNotBeEqual((void*)automataState.stream, (void*)someChars);
    shouldNotBeNULL(automataState.iterator);
    shouldNotBeNULL(automataState.dState);
    shouldNotBeEqual((void*)automataState.dState, (void*)dState);
    automataState.clear();
    delete someChars;
    delete dfa;
    delete nfaBuilder;
    delete nfa;
    delete classifier;
  } endIt();

  it("CopyFrom an AutomataState") {
    Classifier *classifier = new Classifier();
    shouldNotBeNULL(classifier);
    NFA *nfa = new NFA(classifier);
    shouldNotBeNULL(nfa);
    NFABuilder *nfaBuilder = new NFABuilder(nfa);
    shouldNotBeNULL(nfaBuilder);
    nfaBuilder->compileRegularExpressionForTokenId("start", "simple", 1);
    nfaBuilder->compileRegularExpressionForTokenId("other", "otherSimple", 1);
    DFA *dfa = new DFA(nfa);
    shouldNotBeNULL(dfa);
    StateAllocator *allocator = dfa->getStateAllocator();
    Utf8Chars *someChars = new Utf8Chars("some characters");
    NFA::StartStateId startStateId = nfa->findStartStateId("start");
    State *dState = dfa->getDFAStartState(startStateId);
    AutomataState automataState;
    automataState.initialize(dfa, someChars, startStateId);
    shouldBeEqual(automataState.dfa, dfa);
    shouldBeEqual(automataState.allocator, allocator);
    shouldNotBeNULL(automataState.token);
    shouldNotBeNULL(automataState.stream);
    shouldNotBeNULL(automataState.iterator);
    shouldNotBeNULL(automataState.dState);
    shouldNotBeEqual((void*)automataState.dState, (void*)dState);
    shouldBeEqual(automataState.allocator, allocator);
    AutomataState newAutomataState;
    shouldBeNULL(newAutomataState.token);
    shouldBeNULL(newAutomataState.stream);
    shouldBeNULL(newAutomataState.iterator);
    shouldBeNULL(newAutomataState.dState);
    shouldBeNULL(newAutomataState.allocator);
    newAutomataState.copyFrom(automataState, true, true);
    shouldNotBeNULL(newAutomataState.token);
    shouldBeEqual(newAutomataState.stream,    automataState.stream);
    shouldBeEqual(newAutomataState.iterator,  automataState.iterator);
    shouldBeEqual((void*)newAutomataState.dState,    (void*)automataState.dState);
    shouldBeEqual(newAutomataState.allocator, allocator);
    newAutomataState.copyFrom(automataState, false, false);
    shouldBeEqual(newAutomataState.token,     automataState.token);
    shouldBeEqual(newAutomataState.stream,    automataState.stream);
    shouldBeEqual(newAutomataState.iterator,  automataState.iterator);
    shouldBeEqual((void*)newAutomataState.dState,    (void*)automataState.dState);
    shouldBeEqual(newAutomataState.allocator, allocator);
    //
    // newAutomataState is now a copy of automataState so we ONLY want to
    // clear the last one....
    //
    // automataState.clear();
    newAutomataState.clear();
    delete someChars;
    delete dfa;
    delete nfaBuilder;
    delete nfa;
    delete classifier;
  } endIt();

  it("Clear an AutomataState") {
    Classifier *classifier = new Classifier();
    shouldNotBeNULL(classifier);
    NFA *nfa = new NFA(classifier);
    shouldNotBeNULL(nfa);
    NFABuilder *nfaBuilder = new NFABuilder(nfa);
    shouldNotBeNULL(nfaBuilder);
    nfaBuilder->compileRegularExpressionForTokenId("start", "simple", 1);
    nfaBuilder->compileRegularExpressionForTokenId("other", "otherSimple", 1);
    DFA *dfa = new DFA(nfa);
    shouldNotBeNULL(dfa);
    StateAllocator *allocator = dfa->getStateAllocator();
    Utf8Chars *someChars = new Utf8Chars("some characters");
    NFA::StartStateId startStateId = nfa->findStartStateId("start");
    State *dState = dfa->getDFAStartState(startStateId);
    AutomataState automataState;
    automataState.initialize(dfa, someChars, startStateId);
    shouldBeEqual(automataState.dfa, dfa);
    shouldBeEqual(automataState.allocator, allocator);
    shouldNotBeNULL(automataState.token);
    shouldNotBeNULL(automataState.stream);
    shouldNotBeNULL(automataState.iterator);
    shouldNotBeEqual((void*)automataState.dState, (void*)dState);
    shouldBeEqual(automataState.allocator, allocator);
    automataState.clear();
    shouldBeNULL(automataState.token);
    shouldBeNULL(automataState.stream);
    shouldBeNULL(automataState.iterator);
    shouldBeNULL(automataState.dState);
    shouldBeNULL(automataState.allocator);
    delete someChars;
    delete dfa;
    delete nfaBuilder;
    delete nfa;
    delete classifier;
  } endIt();

} endDescribe(AutomataState);
