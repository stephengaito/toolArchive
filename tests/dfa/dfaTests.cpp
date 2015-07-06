#include <string.h>
#include <stdio.h>

#include <cUtils/specs/specs.h>

#ifndef protected
#define protected public
#endif

#include "dynUtf8Parser/nfaBuilder.h"
#include <dynUtf8Parser/dfa/pushDownMachine.h>

namespace DeterministicFiniteAutomaton {

/// \brief Test the ability of a given DFA class to compile, on the fly,
/// a DFA corresponding to a given NFA.
describe(DFA) {

  specSize(DFA);

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
    DFA *dfa = new DFA(nfa);
    shouldNotBeNULL(dfa);
    shouldBeEqual(dfa->nfa, nfa);
    shouldNotBeNULL(dfa->allocator);
    shouldNotBeNULL(dfa->nextStateMapping);
    shouldNotBeNULL(dfa->startState);
    shouldBeEqual(dfa->numStartStates, 1);
    shouldBeNULL(dfa->startState[0]);
    shouldNotBeNULL(((void*)dfa->tokensState));
    for( size_t i = 0; i < dfa->allocator->stateSize; i++) {
      shouldBeZero(dfa->tokensState[i]);
    }
    State *startState = dfa->getDFAStartState((NFA::StartStateId)0);
    shouldNotBeNULL(dfa->startState[0]);
    shouldBeEqual((void*)dfa->startState[0], (void*)startState);
    shouldBeEqual((int)dfa->startState[0][0], 15);
    for( size_t i = 1; i < dfa->allocator->stateSize; i++) {
      shouldBeZero(dfa->startState[0][i]);
    }
    shouldBeFalse(dfa->allocator->isSubStateOf(dfa->startState[0], dfa->tokensState));
    delete dfa;
    delete nfaBuilder;
    delete nfa;
    delete classifier;
  } endIt();

  it("should be able to register lots of start states") {
    Classifier *classifier = new Classifier();
    shouldNotBeNULL(classifier);
    NFA *nfa = new NFA(classifier);
    shouldNotBeNULL(nfa);
    shouldNotBeNULL(nfa->startStateIds);
    shouldBeZero(nfa->startState.getNumItems());
    char buffer[100];
    NFA::State *states[100];
    NFA::MatchData noMatchData;
    noMatchData.c.u = 0;
    for (size_t i = 0; i < 100; i++) {
      memset(buffer, 0, 100);
      sprintf(buffer, "%zu", i);
      nfa->registerStartState(buffer);
      states[i] = nfa->addState(NFA::Split, noMatchData, NULL, NULL, "test");
      shouldNotBeNULL(states[i]);
      nfa->appendNFAToStartState(buffer, states[i]);
      shouldBeEqual(nfa->getNumberStartStates(), i+1);
      shouldBeEqual(nfa->findStartStateId(buffer), i);
      shouldBeEqual((void*)nfa->getStartState(buffer), (void*)states[i]);
      shouldBeEqual((void*)nfa->getStartState((NFA::StartStateId)i), (void*)states[i]);
    }
    DFA *dfa = new DFA(nfa);
    shouldNotBeNULL(dfa);
    shouldBeEqual(nfa->getNumberStartStates(), 100);
    shouldNotBeNULL(dfa->startState);
    shouldBeEqual(dfa->numStartStates, 100);
    for (size_t i = 0; i < 100; i++) {
      shouldBeNULL((void*)dfa->startState[i]);
      shouldBeEqual((void*)nfa->getStartState((NFA::StartStateId)i), (void*)states[i]);
      dfa->getDFAStartState(i);
      shouldNotBeNULL((void*)dfa->startState[i]);
    }
    delete dfa;
    delete nfa;
    delete classifier;
  } endIt();

  /// Show that DFA::computeNextDFAState can compile a simple
  /// NFA corresponding to the regular expression: /(abab|abbb)/
  /// which has only characters to match.
  it("Should computeNextDFAState with no generic states") {
    Classifier *classifier = new Classifier();
    shouldNotBeNULL(classifier);
    NFA *nfa = new NFA(classifier);
    shouldNotBeNULL(nfa);
    NFABuilder *nfaBuilder = new NFABuilder(nfa);
    shouldNotBeNULL(nfaBuilder);
    nfaBuilder->compileRegularExpressionForTokenId("start", "(abab|abbb)", 1);
    shouldBeEqual(nfa->getNumberStates(), 11);
    DFA *dfa = new DFA(nfa);
    shouldNotBeNULL(dfa);
    StateAllocator *allocator = dfa->allocator;
    shouldNotBeNULL(allocator);
    NextStateMapping *mapping = dfa->nextStateMapping;
    shouldNotBeNULL(mapping);
    shouldBeZero(allocator->allocatedUnusedStack.getNumItems());
    State *specificState = allocator->allocateANewState(); // this will be the specific state
    State *genericState  = allocator->allocateANewState(); // this will be generic state
    State *startState    = allocator->allocateANewState(); // this will be the start state
    shouldNotBeNULL((void*)specificState);
    shouldNotBeNULL((void*)genericState);
    shouldNotBeNULL((void*)startState);
    allocator->unallocateState(specificState);
    allocator->unallocateState(genericState);
    allocator->unallocateState(startState); // last in first out of stack
    utf8Char_t firstChar;
    firstChar.u = 0;
    firstChar.c[0] = 'a';
    Classifier::classSet_t classificationSet = 0;
    State *nextDFAState =
      dfa->computeNextDFAState(dfa->getDFAStartState("start"),
                               firstChar,
                               classificationSet);
    shouldNotBeNULL((void*)nextDFAState);
    shouldBeEqual((void*)nextDFAState, (void*)specificState);
    shouldBeTrue(allocator->isStateEmpty(genericState));
    State **registeredGenericState =
      (State**)hattrie_tryget(mapping->nextDFAStateMap,
                              genericState, allocator->stateSize);
    shouldBeNULL((void**)registeredGenericState);

    shouldBeFalse(allocator->isStateEmpty(specificState));
    State **registeredSpecificState =
      (State**)hattrie_tryget(mapping->nextDFAStateMap,
                              specificState, allocator->stateSize);
    shouldNotBeNULL((void**)registeredSpecificState);
    shouldBeEqual((void*)*registeredSpecificState, (void*)specificState);
    shouldBeEqual(((int)nextDFAState[0]), (int)0x30);
    for (size_t i = 1; i < allocator->stateSize; i++) {
      shouldBeEqual(((int)nextDFAState[i]), (int)0x00);
    }
    delete dfa;
    delete nfaBuilder;
    delete nfa;
    delete classifier;
  } endIt();

  /// Show that DFA::computeNextDFAState can compile an
  /// NFA corresponding to the regular expression:
  /// /(abab|[!whitespace]bbb)/ which has both characters
  /// and Classifier::classSet_t(s) to match.
  it("Should computeNextDFAState with generic states") {
    Classifier *classifier = new Classifier();
    shouldNotBeNULL(classifier);
    classifier->registerClassSet("whitespace",1);
    classifier->classifyUtf8CharsAs(Utf8Chars::whiteSpaceChars,"whitespace");
    shouldBeEqual(classifier->getClassSet(" "), 1);
    shouldBeEqual(classifier->getClassSet("a"), ~1L);
    NFA *nfa = new NFA(classifier);
    shouldNotBeNULL(nfa);
    NFABuilder *nfaBuilder = new NFABuilder(nfa);
    shouldNotBeNULL(nfaBuilder);
    nfaBuilder->compileRegularExpressionForTokenId("start", "(abab|[!whitespace]bbb)", 1);
    shouldBeEqual(nfa->getNumberStates(), 11);
    DFA *dfa = new DFA(nfa);
    shouldNotBeNULL(dfa);
    StateAllocator *allocator = dfa->allocator;
    shouldNotBeNULL(allocator);
    NextStateMapping *mapping = dfa->nextStateMapping;
    shouldNotBeNULL(mapping);
    shouldBeZero(allocator->allocatedUnusedStack.getNumItems());
    State *specificState = allocator->allocateANewState(); // this will be the specific state
    State *genericState  = allocator->allocateANewState(); // this will be generic state
    State *startState    = allocator->allocateANewState(); // this will be the start state
    shouldNotBeNULL((void*)specificState);
    shouldNotBeNULL((void*)genericState);
    shouldNotBeNULL((void*)startState);
    allocator->unallocateState(specificState);
    allocator->unallocateState(genericState);
    allocator->unallocateState(startState); // last in first out of stack
    shouldBeEqual((void*)allocator->allocatedUnusedStack.getItem(0, NULL),
      (void*)specificState);
    shouldBeEqual((void*)allocator->allocatedUnusedStack.getItem(1, NULL),
      (void*)genericState);
    utf8Char_t firstChar;
    firstChar.u = 0;
    firstChar.c[0] = 'a';
    Classifier::classSet_t classificationSet =
      classifier->getClassSet(firstChar);
    shouldBeEqual(classificationSet, ~1L);
    State *nextDFAState =
      dfa->computeNextDFAState(dfa->getDFAStartState("start"),
                               firstChar,
                               classificationSet);
    shouldNotBeNULL((void*)nextDFAState);
    shouldBeEqual((void*)nextDFAState, (void*)specificState);
    shouldBeFalse(allocator->isStateEmpty(specificState));
    shouldBeEqual(((int)specificState[0]), (int)0x30);
    for (size_t i = 1; i < dfa->allocator->stateSize; i++) {
      shouldBeEqual(((int)specificState[i]), (int)0x00);
    }
    State **registeredSpecificState =
      (State**)hattrie_tryget(mapping->nextDFAStateMap,
                              specificState, allocator->stateSize);
    shouldNotBeNULL((void**)registeredSpecificState);
    shouldBeEqual((void*)*registeredSpecificState, (void*)specificState);

    shouldBeFalse(allocator->isStateEmpty(genericState));
    State **registeredGenericState =
      (State**)hattrie_tryget(mapping->nextDFAStateMap,
                              genericState, allocator->stateSize);
    shouldNotBeNULL((void**)registeredGenericState);
    shouldBeEqual((void*)*registeredGenericState, (void*)genericState);
    shouldBeEqual(((int)genericState[0]), (int)0x20);
    for (size_t i = 1; i < dfa->allocator->stateSize; i++) {
      shouldBeEqual(((int)genericState[i]), (int)0x00);
    }
    delete dfa;
    delete nfaBuilder;
    delete nfa;
    delete classifier;
  } endIt();

  /// Show that DFA::computeNextDFAState can compile an
  /// NFA corresponding to the regular expression:
  /// /([!whitespace]bab|[!whitespace]bbb)/ which has only
  /// Classifier::classSet_t(s) to match in the first transition.
  it("computeNextDFAState with only generic states") {
    Classifier *classifier = new Classifier();
    shouldNotBeNULL(classifier);
    classifier->registerClassSet("whitespace",1);
    classifier->classifyUtf8CharsAs(Utf8Chars::whiteSpaceChars,"whitespace");
    shouldBeEqual(classifier->getClassSet(" "), 1);
    shouldBeEqual(classifier->getClassSet("a"), ~1L);
    NFA *nfa = new NFA(classifier);
    shouldNotBeNULL(nfa);
    NFABuilder *nfaBuilder = new NFABuilder(nfa);
    shouldNotBeNULL(nfaBuilder);
    nfaBuilder->compileRegularExpressionForTokenId("start", "([!whitespace]bab|[!whitespace]bbb)", 1);
    shouldBeEqual(nfa->getNumberStates(), 11);
    DFA *dfa = new DFA(nfa);
    shouldNotBeNULL(dfa);
    StateAllocator *allocator = dfa->allocator;
    shouldNotBeNULL(allocator);
    NextStateMapping *mapping = dfa->nextStateMapping;
    shouldNotBeNULL(mapping);
    shouldBeZero(allocator->allocatedUnusedStack.getNumItems());
    State *specificState = allocator->allocateANewState(); // this will be the specific state
    State *genericState = allocator->allocateANewState(); // this will be generic state
    State *startState    = allocator->allocateANewState(); // this will be the start state
    shouldNotBeNULL((void*)specificState);
    shouldNotBeNULL((void*)genericState);
    shouldNotBeNULL((void*)startState);
    allocator->unallocateState(specificState);
    allocator->unallocateState(genericState);
    allocator->unallocateState(startState); // last in first out stack
    shouldBeEqual((void*)allocator->allocatedUnusedStack.getItem(0, NULL),
      (void*)specificState);
    shouldBeEqual((void*)allocator->allocatedUnusedStack.getItem(1, NULL),
      (void*)genericState);
    utf8Char_t firstChar;
    firstChar.u = 0;
    firstChar.c[0] = 'a';
    Classifier::classSet_t classificationSet =
      classifier->getClassSet(firstChar);
    shouldBeEqual(classificationSet, ~1L);
    State *nextDFAState =
      dfa->computeNextDFAState(dfa->getDFAStartState("start"),
                               firstChar,
                               classificationSet);
    shouldNotBeNULL((void*)nextDFAState);
    shouldBeEqual((void*)nextDFAState, (void*)genericState);
    shouldBeFalse(allocator->isStateEmpty(genericState));
    shouldBeEqual(((int)genericState[0]), (int)0x30);
    for (size_t i = 1; i < allocator->stateSize; i++) {
      shouldBeEqual(((int)genericState[i]), (int)0x00);
    }
    State **registeredGenericState =
      (State**)hattrie_tryget(mapping->nextDFAStateMap,
                              genericState, allocator->stateSize);
    shouldNotBeNULL((void**)registeredGenericState);
    shouldBeEqual((void*)*registeredGenericState, (void*)genericState);

    shouldBeTrue(allocator->isStateEmpty(specificState));
    State **registeredSpecificState =
      (State**)hattrie_tryget(mapping->nextDFAStateMap,
                              specificState, allocator->stateSize);
    shouldBeNULL((void**)registeredSpecificState);
    shouldBeEqual(((int)nextDFAState[0]), (int)0x30);
    for (size_t i = 1; i < allocator->stateSize; i++) {
      shouldBeEqual(((int)nextDFAState[i]), (int)0x00);
    }
    delete dfa;
    delete nfaBuilder;
    delete nfa;
    delete classifier;
  } endIt();

  it("Show that DFA::getNextToken works with a simple regular expression") {
    Classifier *classifier = new Classifier();
    shouldNotBeNULL(classifier);
    NFA *nfa = new NFA(classifier);
    shouldNotBeNULL(nfa);
    NFABuilder *nfaBuilder = new NFABuilder(nfa);
    shouldNotBeNULL(nfaBuilder);
    nfaBuilder->compileRegularExpressionForTokenId("start", "simple", 1);
    shouldBeEqual(nfa->getNumberStates(), 8);
    DFA *dfa = new DFA(nfa);
    shouldNotBeNULL(dfa);
    PushDownMachine *pdm = new PushDownMachine(dfa);
    shouldNotBeNULL(pdm);
    Utf8Chars *stream0 = new Utf8Chars("simple");
    shouldNotBeNULL(stream0);
    PDMTracer *pdmTracer =
//      new PDMTracer("Parse and tokenize 'simple'", stdout);
      new PDMTracer("Parse and tokenize 'simple'", NULL);
    shouldNotBeNULL(pdmTracer);
    Token *aToken = pdm->runFromUsing("start", stream0, pdmTracer);
    shouldNotBeNULL(aToken);
    shouldBeEqual(aToken->tokenId,   1);
    shouldNotBeNULL((void*)aToken->textStart);
    shouldBeEqual((void*)aToken->textStart, (void*)stream0->getStart());
    shouldNotBeZero(aToken->textLength);
    shouldBeEqual(aToken->textLength, 6);
    shouldBeZero(aToken->tokens.getNumItems());
    Utf8Chars *stream1 = new Utf8Chars("notSoSimple");
    shouldNotBeNULL(stream1);
    delete aToken;
    delete pdmTracer;
    pdmTracer =
//      new PDMTracer("Parse and tokenize 'notSoSimple'", stdout);
      new PDMTracer("Parse and tokenize 'notSoSimple'", NULL);
    shouldNotBeNULL(pdmTracer);
    aToken = pdm->runFromUsing("start", stream1, pdmTracer);
    shouldBeNULL(aToken);
    delete pdmTracer;
    delete stream0;
    delete stream1;
    delete pdm;
    delete dfa;
    delete nfaBuilder;
    delete nfa;
    delete classifier;
  } endIt();

  it("Show that DFA::getNextToken works with simple regular expression",
     "with alternate patterns") {
    Classifier *classifier = new Classifier();
    shouldNotBeNULL(classifier);
    NFA *nfa = new NFA(classifier);
    shouldNotBeNULL(nfa);
    NFABuilder *nfaBuilder = new NFABuilder(nfa);
    shouldNotBeNULL(nfaBuilder);
    nfaBuilder->compileRegularExpressionForTokenId("start",
      "(simple|notSoSimple)", 1);
    shouldBeEqual(nfa->getNumberStates(), 20);
    NFA::State *baseState =
      (NFA::State*)nfa->stateAllocator->blocks.getTop();
    shouldNotBeNULL((void*)baseState);
    for (size_t i = 0; i < 19; i++) {
      shouldNotBeEqual(baseState[i].matchType, NFA::Token);
    }
    shouldBeEqual(baseState[19].matchType, NFA::Token);
    for (size_t i = 1; i < 6; i++) {
      shouldBeEqual((void*)baseState[i].out, (void*)(baseState+i+1));
    }
    shouldBeEqual((void*)baseState[6].out, (void*)(baseState+19));
    DFA *dfa = new DFA(nfa);
    shouldNotBeNULL(dfa);
    PushDownMachine *pdm = new PushDownMachine(dfa);
    shouldNotBeNULL(pdm);
    Utf8Chars *stream0 = new Utf8Chars("simple");
    shouldNotBeNULL(stream0);
    PDMTracer *pdmTracer =
//      new PDMTracer("Parse and tokenize 'simple'", stdout);
      new PDMTracer("Parse and tokenize 'simple'", NULL);
    shouldNotBeNULL(pdmTracer);
    Token *aToken = pdm->runFromUsing("start", stream0, pdmTracer);
    shouldNotBeNULL(aToken);
    shouldNotBeNULL((void*)aToken->textStart);
    shouldBeEqual((void*)aToken->textStart, (void*)stream0->getStart());
    shouldNotBeZero(aToken->textLength);
    shouldBeEqual(aToken->textLength, 6);
    shouldBeZero(aToken->tokens.getNumItems());
    shouldBeEqual(aToken->tokenId, 1);
    Utf8Chars *stream1 = new Utf8Chars("notSoSimple");
    shouldNotBeNULL(stream1);
    delete aToken;
    delete pdmTracer;
    pdmTracer =
//      new PDMTracer("Parse and tokenize 'notSoSimple'", stdout);
      new PDMTracer("Parse and tokenize 'notSoSimple'", NULL);
    shouldNotBeNULL(pdmTracer);
    aToken = pdm->runFromUsing("start", stream1, pdmTracer);
    shouldNotBeNULL(aToken);
    shouldNotBeNULL((void*)aToken->textStart);
    shouldBeEqual((void*)aToken->textStart, (void*)stream1->getStart());
    shouldNotBeZero(aToken->textLength);
    shouldBeEqual(aToken->textLength, 11);
    shouldBeZero(aToken->tokens.getNumItems());
    shouldBeEqual(aToken->tokenId, 1);
    delete aToken;
    delete pdmTracer;
    delete stream0;
    delete stream1;
    delete pdm;
    delete dfa;
    delete nfaBuilder;
    delete nfa;
    delete classifier;
  } endIt();

  it("Show that DFA::getNextToken works with a regular expression",
     "with only Classifier::classSet_t transitions") {
    Classifier *classifier = new Classifier();
    shouldNotBeNULL(classifier);
    classifier->registerClassSet("whitespace",1);
    classifier->classifyUtf8CharsAs(Utf8Chars::whiteSpaceChars,"whitespace");
    shouldBeEqual(classifier->getClassSet(" "), 1);
    shouldBeEqual(classifier->getClassSet("a"), ~1L);
    NFA *nfa = new NFA(classifier);
    shouldNotBeNULL(nfa);
    NFABuilder *nfaBuilder = new NFABuilder(nfa);
    shouldNotBeNULL(nfaBuilder);
    nfaBuilder->compileRegularExpressionForTokenId("start", "[!whitespace]+", 1);
    shouldBeEqual(nfa->getNumberStates(), 4);
    DFA *dfa = new DFA(nfa);
    shouldNotBeNULL(dfa);
    PushDownMachine *pdm = new PushDownMachine(dfa);
    shouldNotBeNULL(pdm);
    Utf8Chars *stream0 = new Utf8Chars("sillysomeNonWhiteSpace");
    shouldNotBeNULL(stream0);
    PDMTracer *pdmTracer =
//      new PDMTracer("Parse and tokenize 'sillysomeNonWhiteSpace'", stdout);
      new PDMTracer("Parse and tokenize 'sillysomeNonWhiteSpace'", NULL);
    shouldNotBeNULL(pdmTracer);
    Token *aToken = pdm->runFromUsing("start", stream0, pdmTracer);
    shouldNotBeNULL(aToken);
    shouldNotBeNULL((void*)aToken->textStart);
    shouldBeEqual((void*)aToken->textStart, (void*)stream0->getStart());
    shouldNotBeZero(aToken->textLength);
    shouldBeEqual(aToken->textLength, 22);
    shouldBeZero(aToken->tokens.getNumItems());
    shouldBeEqual(aToken->tokenId, 1);
    delete aToken;
    delete pdmTracer;
    delete stream0;
    delete pdm;
    delete dfa;
    delete nfaBuilder;
    delete nfa;
    delete classifier;
  } endIt();

  it("Show that DFA::getNextToken works with multiple regExp/TokenIds") {
    Classifier *classifier = new Classifier();
    shouldNotBeNULL(classifier);
    classifier->registerClassSet("whitespace",1);
    classifier->classifyUtf8CharsAs(Utf8Chars::whiteSpaceChars,"whitespace");
    NFA *nfa = new NFA(classifier);
    shouldNotBeNULL(nfa);
    NFABuilder *nfaBuilder = new NFABuilder(nfa);
    shouldNotBeNULL(nfaBuilder);
    nfaBuilder->compileRegularExpressionForTokenId("start", "[whitespace]+", 1);
    nfaBuilder->compileRegularExpressionForTokenId("start", "[!whitespace]+", 2);
    shouldBeEqual(nfa->getNumberStates(), 8);
    DFA *dfa = new DFA(nfa);
    shouldNotBeNULL(dfa);
    StateAllocator *allocator = dfa->allocator;
    shouldNotBeNULL(allocator);
    dfa->getDFAStartState("start");
    NFA::State *nfaState =
      allocator->stateMatchesToken(dfa->startState[0], dfa->tokensState);
    shouldBeNULL(nfaState);
    PushDownMachine *pdm = new PushDownMachine(dfa);
    shouldNotBeNULL(pdm);
    Utf8Chars *stream0 = new Utf8Chars("sillysomeNonWhiteSpace   ");
    shouldNotBeNULL(stream0);
    PDMTracer *pdmTracer =
//      new PDMTracer("Parse and tokenize 'sillysomeNonWhiteSpace   '", stdout);
      new PDMTracer("Parse and tokenize 'sillysomeNonWhiteSpace   '", NULL);
    shouldNotBeNULL(pdmTracer);
    Token *aToken = pdm->runFromUsing("start", stream0, pdmTracer, true);
    shouldNotBeNULL(aToken);
    shouldNotBeNULL((void*)aToken->textStart);
    shouldBeEqual((void*)aToken->textStart, (void*)stream0->getStart());
    shouldNotBeZero(aToken->textLength);
    shouldBeEqual(aToken->textLength, 22);
    shouldBeZero(aToken->tokens.getNumItems());
    shouldBeEqual(aToken->tokenId, 2);
    Utf8Chars *stream1 = new Utf8Chars("   sillysomeNonWhiteSpace");
    shouldNotBeNULL(stream1);
    delete aToken;
    delete pdmTracer;
    pdmTracer =
//      new PDMTracer("Parse and tokenize '   sillysomeNonWhiteSpace'", stdout);
      new PDMTracer("Parse and tokenize '   sillysomeNonWhiteSpace'", NULL);
    shouldNotBeNULL(pdmTracer);
    aToken = pdm->runFromUsing("start", stream1, pdmTracer, true);
    shouldNotBeNULL(aToken);
    shouldNotBeNULL((void*)aToken->textStart);
    shouldBeEqual((void*)aToken->textStart, (void*)stream1->getStart());
    shouldNotBeZero(aToken->textLength);
    shouldBeEqual(aToken->textLength, 3);
    shouldBeZero(aToken->tokens.getNumItems());
    shouldBeEqual(aToken->tokenId, 1);
    delete aToken;
    delete stream0;
    delete stream1;
    delete pdmTracer;
    delete pdm;
    delete dfa;
    delete nfaBuilder;
    delete nfa;
    delete classifier;
  } endIt();

} endDescribe(DFA);

}; // namespace DeterministicFiniteAutomaton
