#include <bandit/bandit.h>
using namespace bandit;

#include <string.h>
#include <stdio.h>

#ifndef private
#define private public
#endif

#include "nfaBuilder.h"
#include <dfa/pushDownMachine.h>

namespace DeterministicFiniteAutomaton {

go_bandit([](){

  printf("\n----------------------------------\n");
  printf(  "dfa\n");
  printf(  "           DFA = %zu bytes (%zu bits)\n", sizeof(DFA), sizeof(DFA)*8);
  printf(  "----------------------------------\n");

  /// \brief Test the ability of a given DFA class to compile, on the fly,
  /// a DFA corresponding to a given NFA.
  describe("DFA", [](){


    /// Show that we can create an appropriately allocated DFA
    /// from a given NFA.
    it("Should have correct sizes and pointers setup", [&](){
      Classifier *classifier = new Classifier();
      NFA *nfa = new NFA(classifier);
      NFABuilder *nfaBuilder = new NFABuilder(nfa);
      nfaBuilder->compileRegularExpressionForTokenId("start", "(abab|abbb)", 1);
      AssertThat(nfa->getNumberStates(), Is().EqualTo(11));
      ParseTrees *parseTrees = new ParseTrees();
      DFA *dfa = new DFA(nfa, parseTrees);
      AssertThat(dfa, Is().Not().EqualTo((void*)0));
      AssertThat(dfa->nfa, Equals(nfa));
      AssertThat(dfa->allocator, Is().Not().EqualTo((void*)0));
      AssertThat(dfa->nextStateMapping, Is().Not().EqualTo((void*)0));
      AssertThat(dfa->startState, Is().Not().EqualTo((State**)0));
      AssertThat(dfa->numStartStates, Equals(1));
      AssertThat(dfa->startState[0], Equals((State*)0));
      AssertThat(((void*)dfa->tokensState), Is().Not().EqualTo((void*)0));
      for( size_t i = 0; i < dfa->allocator->stateSize; i++) {
        AssertThat(dfa->tokensState[i], Equals(0));
      }
      State *startState = dfa->getDFAStartState((NFA::StartStateId)0);
      AssertThat(dfa->startState[0], Is().Not().EqualTo((State*)0));
      AssertThat(dfa->startState[0], Equals(startState));
      AssertThat((int)dfa->startState[0][0], Equals(15));
      for( size_t i = 1; i < dfa->allocator->stateSize; i++) {
        AssertThat(dfa->startState[0][i], Equals(0));
      }
      AssertThat(dfa->allocator->isSubStateOf(dfa->startState[0], dfa->tokensState), Is().False());
      delete dfa;
      delete parseTrees;
      delete nfa;
      delete classifier;
    });

    it("should be able to register lots of start states", [](){
      Classifier *classifier = new Classifier();
      NFA *nfa = new NFA(classifier);
      AssertThat(nfa->startStateIds,  Is().Not().EqualTo((void*)0));
      AssertThat(nfa->startState.getNumItems(), Equals(0));
      char buffer[100];
      NFA::State *states[100];
      NFA::MatchData noMatchData;
      noMatchData.c.u = 0;
      for (size_t i = 0; i < 100; i++) {
        memset(buffer, 0, 100);
        sprintf(buffer, "%zu", i);
        nfa->registerStartState(buffer);
        states[i] = nfa->addState(NFA::Split, noMatchData, NULL, NULL);
        nfa->appendNFAToStartState(buffer, states[i]);
        AssertThat(nfa->getNumberStartStates(), Equals(i+1));
        AssertThat(nfa->findStartStateId(buffer), Equals(i));
        AssertThat(nfa->getStartState(buffer), Equals(states[i]));
        AssertThat(nfa->getStartState((NFA::StartStateId)i), Equals(states[i]));
      }
      ParseTrees *parseTrees = new ParseTrees();
      DFA *dfa = new DFA(nfa, parseTrees);
      AssertThat(nfa->getNumberStartStates(), Equals(100));
      AssertThat(dfa->startState, Is().Not().EqualTo((void*)0));
      AssertThat(dfa->numStartStates, Equals(100));
      for (size_t i = 0; i < 100; i++) {
        AssertThat((State*)dfa->startState[i], Equals((State*)0));
        AssertThat(nfa->getStartState((NFA::StartStateId)i), Equals(states[i]));
        dfa->getDFAStartState(i);
        AssertThat((State*)dfa->startState[i], Is().Not().EqualTo((State*)0));
      }
      delete dfa;
      delete parseTrees;
      delete nfa;
      delete classifier;
    });

    /// Show that DFA::computeNextDFAState can compile a simple
    /// NFA corresponding to the regular expression: /(abab|abbb)/
    /// which has only characters to match.
    it("Should computeNextDFAState with no generic states", [&](){
      Classifier *classifier = new Classifier();
      NFA *nfa = new NFA(classifier);
      NFABuilder *nfaBuilder = new NFABuilder(nfa);
      nfaBuilder->compileRegularExpressionForTokenId("start", "(abab|abbb)", 1);
      AssertThat(nfa->getNumberStates(), Is().EqualTo(11));
      ParseTrees *parseTrees = new ParseTrees();
      DFA *dfa = new DFA(nfa, parseTrees);
      AssertThat(dfa, Is().Not().EqualTo((void*)0));
      StateAllocator *allocator = dfa->allocator;
      AssertThat(allocator, Is().Not().EqualTo((void*)0));
      NextStateMapping *mapping = dfa->nextStateMapping;
      AssertThat(mapping, Is().Not().EqualTo((void*)0));
      AssertThat(allocator->allocatedUnusedStack.getNumItems(), Equals(0));
      State *specificState = allocator->allocateANewState(); // this will be the specific state
      State *genericState  = allocator->allocateANewState(); // this will be generic state
      State *startState    = allocator->allocateANewState(); // this will be the start state
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
      AssertThat((void*)nextDFAState, Is().Not().EqualTo((void*)0));
      AssertThat((void*)nextDFAState, Is().EqualTo((void*)specificState));
      AssertThat(allocator->isStateEmpty(genericState), Is().True());
      State **registeredGenericState =
        (State**)hattrie_tryget(mapping->nextDFAStateMap,
                                genericState, allocator->stateSize);
      AssertThat((void**)registeredGenericState, Is().EqualTo((void*)0));

      AssertThat(allocator->isStateEmpty(specificState), Is().False());
      State **registeredSpecificState =
        (State**)hattrie_tryget(mapping->nextDFAStateMap,
                                specificState, allocator->stateSize);
      AssertThat((void**)registeredSpecificState,
        Is().Not().EqualTo((void*)0));
      AssertThat((void*)*registeredSpecificState,
        Is().EqualTo((void*)specificState));
      AssertThat(((int)nextDFAState[0]), Is().EqualTo((int)0x30));
      for (size_t i = 1; i < allocator->stateSize; i++) {
        AssertThat(((int)nextDFAState[i]), Is().EqualTo((int)0x00));
      }
      delete dfa;
      delete parseTrees;
      delete nfa;
      delete classifier;
    });

    /// Show that DFA::computeNextDFAState can compile an
    /// NFA corresponding to the regular expression:
    /// /(abab|[!whitespace]bbb)/ which has both characters
    /// and Classifier::classSet_t(s) to match.
    it("Should computeNextDFAState with generic states", [&](){
      Classifier *classifier = new Classifier();
      classifier->registerClassSet("whitespace",1);
      classifier->classifyUtf8CharsAs(Utf8Chars::whiteSpaceChars,"whitespace");
      AssertThat(classifier->getClassSet(" "), Is().EqualTo(1));
      AssertThat(classifier->getClassSet("a"), Is().EqualTo(~1L));
      NFA *nfa = new NFA(classifier);
      NFABuilder *nfaBuilder = new NFABuilder(nfa);
      nfaBuilder->compileRegularExpressionForTokenId("start", "(abab|[!whitespace]bbb)", 1);
      AssertThat(nfa->getNumberStates(), Is().EqualTo(11));
      ParseTrees *parseTrees = new ParseTrees();
      DFA *dfa = new DFA(nfa, parseTrees);
      AssertThat(dfa, Is().Not().EqualTo((void*)0));
      StateAllocator *allocator = dfa->allocator;
      AssertThat(allocator, Is().Not().EqualTo((void*)0));
      NextStateMapping *mapping = dfa->nextStateMapping;
      AssertThat(mapping, Is().Not().EqualTo((void*)0));
      AssertThat(allocator->allocatedUnusedStack.getNumItems(), Equals(0));
      State *specificState = allocator->allocateANewState(); // this will be the specific state
      State *genericState  = allocator->allocateANewState(); // this will be generic state
      State *startState    = allocator->allocateANewState(); // this will be the start state
      allocator->unallocateState(specificState);
      allocator->unallocateState(genericState);
      allocator->unallocateState(startState); // last in first out of stack
      AssertThat(allocator->allocatedUnusedStack.getItem(0, NULL),
        Equals(specificState));
      AssertThat(allocator->allocatedUnusedStack.getItem(1, NULL),
        Equals(genericState));
      utf8Char_t firstChar;
      firstChar.u = 0;
      firstChar.c[0] = 'a';
      Classifier::classSet_t classificationSet =
        classifier->getClassSet(firstChar);
      AssertThat(classificationSet, Is().EqualTo(~1L));
      State *nextDFAState =
        dfa->computeNextDFAState(dfa->getDFAStartState("start"),
                                 firstChar,
                                 classificationSet);
      AssertThat((void*)nextDFAState, Is().Not().EqualTo((void*)0));
      AssertThat((void*)nextDFAState, Is().EqualTo((void*)specificState));
      AssertThat(allocator->isStateEmpty(specificState), Is().False());
      AssertThat(((int)specificState[0]), Is().EqualTo((int)0x30));
      for (size_t i = 1; i < dfa->allocator->stateSize; i++) {
        AssertThat(((int)specificState[i]), Is().EqualTo((int)0x00));
      }
      State **registeredSpecificState =
        (State**)hattrie_tryget(mapping->nextDFAStateMap,
                                specificState, allocator->stateSize);
      AssertThat((void**)registeredSpecificState, Is().Not().EqualTo((void*)0));
      AssertThat((void*)*registeredSpecificState, Is().EqualTo((void*)specificState));

      AssertThat(allocator->isStateEmpty(genericState), Is().False());
      State **registeredGenericState =
        (State**)hattrie_tryget(mapping->nextDFAStateMap,
                                genericState, allocator->stateSize);
      AssertThat((void**)registeredGenericState, Is().Not().EqualTo((void*)0));
      AssertThat((void*)*registeredGenericState, Is().EqualTo((void*)genericState));
      AssertThat(((int)genericState[0]), Is().EqualTo((int)0x20));
      for (size_t i = 1; i < dfa->allocator->stateSize; i++) {
        AssertThat(((int)genericState[i]), Is().EqualTo((int)0x00));
      }
      delete dfa;
      delete parseTrees;
      delete nfa;
      delete classifier;
    });

    /// Show that DFA::computeNextDFAState can compile an
    /// NFA corresponding to the regular expression:
    /// /([!whitespace]bab|[!whitespace]bbb)/ which has only
    /// Classifier::classSet_t(s) to match in the first transition.
    it("computeNextDFAState with only generic states", [&](){
      Classifier *classifier = new Classifier();
      classifier->registerClassSet("whitespace",1);
      classifier->classifyUtf8CharsAs(Utf8Chars::whiteSpaceChars,"whitespace");
      AssertThat(classifier->getClassSet(" "), Is().EqualTo(1));
      AssertThat(classifier->getClassSet("a"), Is().EqualTo(~1L));
      NFA *nfa = new NFA(classifier);
      NFABuilder *nfaBuilder = new NFABuilder(nfa);
      nfaBuilder->compileRegularExpressionForTokenId("start", "([!whitespace]bab|[!whitespace]bbb)", 1);
      AssertThat(nfa->getNumberStates(), Is().EqualTo(11));
      ParseTrees *parseTrees = new ParseTrees();
      DFA *dfa = new DFA(nfa, parseTrees);
      AssertThat(dfa, Is().Not().EqualTo((void*)0));
      StateAllocator *allocator = dfa->allocator;
      AssertThat(allocator, Is().Not().EqualTo((void*)0));
      NextStateMapping *mapping = dfa->nextStateMapping;
      AssertThat(mapping, Is().Not().EqualTo((void*)0));
      AssertThat(allocator->allocatedUnusedStack.getNumItems(), Equals(0));
      State *specificState = allocator->allocateANewState(); // this will be the specific state
      State *genericState = allocator->allocateANewState(); // this will be generic state
      State *startState    = allocator->allocateANewState(); // this will be the start state
      allocator->unallocateState(specificState);
      allocator->unallocateState(genericState);
      allocator->unallocateState(startState); // last in first out stack
      AssertThat(allocator->allocatedUnusedStack.getItem(0, NULL),
        Equals(specificState));
      AssertThat(allocator->allocatedUnusedStack.getItem(1, NULL),
        Equals(genericState));
      utf8Char_t firstChar;
      firstChar.u = 0;
      firstChar.c[0] = 'a';
      Classifier::classSet_t classificationSet =
        classifier->getClassSet(firstChar);
      AssertThat(classificationSet, Is().EqualTo(~1L));
      State *nextDFAState =
        dfa->computeNextDFAState(dfa->getDFAStartState("start"),
                                 firstChar,
                                 classificationSet);
      AssertThat((void*)nextDFAState, Is().Not().EqualTo((void*)0));
      AssertThat((void*)nextDFAState, Is().EqualTo((void*)genericState));
      AssertThat(allocator->isStateEmpty(genericState), Is().False());
      AssertThat(((int)genericState[0]), Is().EqualTo((int)0x30));
      for (size_t i = 1; i < allocator->stateSize; i++) {
        AssertThat(((int)genericState[i]), Is().EqualTo((int)0x00));
      }
      State **registeredGenericState =
        (State**)hattrie_tryget(mapping->nextDFAStateMap,
                                genericState, allocator->stateSize);
      AssertThat((void**)registeredGenericState, Is().Not().EqualTo((void*)0));
      AssertThat((void*)*registeredGenericState, Is().EqualTo((void*)genericState));

      AssertThat(allocator->isStateEmpty(specificState), Is().True());
      State **registeredSpecificState =
        (State**)hattrie_tryget(mapping->nextDFAStateMap,
                                specificState, allocator->stateSize);
      AssertThat((void**)registeredSpecificState, Is().EqualTo((void*)0));
      AssertThat(((int)nextDFAState[0]), Is().EqualTo((int)0x30));
      for (size_t i = 1; i < allocator->stateSize; i++) {
        AssertThat(((int)nextDFAState[i]), Is().EqualTo((int)0x00));
      }
      delete dfa;
      delete parseTrees;
      delete nfa;
      delete classifier;
    });

    it("Show that DFA::getNextToken works with a simple regular expression", [&](){
      Classifier *classifier = new Classifier();
      NFA *nfa = new NFA(classifier);
      NFABuilder *nfaBuilder = new NFABuilder(nfa);
      nfaBuilder->compileRegularExpressionForTokenId("start", "simple", 1);
      AssertThat(nfa->getNumberStates(), Is().EqualTo(8));
      ParseTrees *parseTrees = new ParseTrees();
      DFA *dfa = new DFA(nfa, parseTrees);
      AssertThat(dfa, Is().Not().EqualTo((void*)0));
      PushDownMachine *pdm = new PushDownMachine(dfa);
      AssertThat(pdm, Is().Not().EqualTo((void*)0));
      Utf8Chars *stream0 = new Utf8Chars("simple");
      ParseTrees::Token *aToken = pdm->runFromUsing("start", stream0);
      AssertThat(aToken, Is().Not().EqualTo((void*)0));
      AssertThat(aToken->wrappedId, Is().EqualTo(2)); // token:1 ignore:false
      AssertThat(aToken->textStart, Is().Not().EqualTo((char*)0));
      AssertThat(aToken->textStart, Equals(stream0->getStart()));
      AssertThat(aToken->textLength, Is().Not().EqualTo(0));
      AssertThat(aToken->textLength, Equals(6));
      AssertThat(aToken->numTokens, Equals(0));
//      Utf8Chars *stream1 = new Utf8Chars("notSoSimple");
//      aToken = pdm->runFromUsing("start", stream1);
//      AssertThat(aToken, Equals((void*)0));
      delete pdm;
      delete dfa;
      delete parseTrees;
      delete nfa;
      delete classifier;
    });

    it("Show that DFA::getNextToken works with simple regular expression with alternate patterns", [&](){
      Classifier *classifier = new Classifier();
      NFA *nfa = new NFA(classifier);
      NFABuilder *nfaBuilder = new NFABuilder(nfa);
      nfaBuilder->compileRegularExpressionForTokenId("start", "(simple|notSoSimple)", 1);
      AssertThat(nfa->getNumberStates(), Is().EqualTo(20));
      NFA::State *baseState =
        (NFA::State*)nfa->stateAllocator->blocks.getTop();
      for (size_t i = 0; i < 19; i++) {
        AssertThat(baseState[i].matchType, Is().Not().EqualTo(NFA::Token));
      }
      AssertThat(baseState[19].matchType, Equals(NFA::Token));
      for (size_t i = 1; i < 6; i++) {
        AssertThat(baseState[i].out, Equals(baseState+i+1));
      }
      AssertThat(baseState[6].out, Equals(baseState+19));
      ParseTrees *parseTrees = new ParseTrees();
      DFA *dfa = new DFA(nfa, parseTrees);
      AssertThat(dfa, Is().Not().EqualTo((void*)0));
      PushDownMachine *pdm = new PushDownMachine(dfa);
      AssertThat(pdm, Is().Not().EqualTo((void*)0));
      Utf8Chars *stream0 = new Utf8Chars("simple");
      ParseTrees::Token *aToken = pdm->runFromUsing("start", stream0);
      AssertThat(aToken, Is().Not().EqualTo((void*)0));
//      AssertThat(aToken->textStart, Is().Not().EqualTo((char*)0));
//      AssertThat(aToken->textLength, Is().Not().EqualTo(0));
      AssertThat(aToken->numTokens, Equals(0));
      AssertThat(aToken->wrappedId, Is().EqualTo(2)); // token:1 ignore:false
      Utf8Chars *stream1 = new Utf8Chars("notSoSimple");
      aToken = pdm->runFromUsing("start", stream1);
      AssertThat(aToken, Is().Not().EqualTo((void*)0));
//      AssertThat(aToken->textStart, Is().Not().EqualTo((char*)0));
//      AssertThat(aToken->textLength, Is().Not().EqualTo(0));
      AssertThat(aToken->numTokens, Equals(0));
      AssertThat(aToken->wrappedId, Is().EqualTo(2)); // token:1 ignore:false
      delete dfa;
      delete parseTrees;
      delete nfa;
      delete classifier;
    });

   it("Show that DFA::getNextToken works with a regular expression with only Classifier::classSet_t transitions", [&](){
      Classifier *classifier = new Classifier();
      classifier->registerClassSet("whitespace",1);
      classifier->classifyUtf8CharsAs(Utf8Chars::whiteSpaceChars,"whitespace");
      AssertThat(classifier->getClassSet(" "), Is().EqualTo(1));
      AssertThat(classifier->getClassSet("a"), Is().EqualTo(~1L));
      NFA *nfa = new NFA(classifier);
      NFABuilder *nfaBuilder = new NFABuilder(nfa);
      nfaBuilder->compileRegularExpressionForTokenId("start", "[!whitespace]+", 1);
      AssertThat(nfa->getNumberStates(), Is().EqualTo(4));
      ParseTrees *parseTrees = new ParseTrees();
      DFA *dfa = new DFA(nfa, parseTrees);
      AssertThat(dfa, Is().Not().EqualTo((void*)0));
      PushDownMachine *pdm = new PushDownMachine(dfa);
      AssertThat(pdm, Is().Not().EqualTo((void*)0));
      Utf8Chars *stream0 = new Utf8Chars("sillysomeNonWhiteSpace");
      ParseTrees::Token *aToken = pdm->runFromUsing("start", stream0);
      AssertThat(aToken, Is().Not().EqualTo((void*)0));
//      AssertThat(aToken->textStart, Is().Not().EqualTo((char*)0));
//      AssertThat(aToken->textLength, Is().Not().EqualTo(0));
      AssertThat(aToken->numTokens, Equals(0));
      AssertThat(aToken->wrappedId, Is().EqualTo(2)); //token:1 ignore:false
      delete dfa;
      delete parseTrees;
      delete nfa;
      delete classifier;
    });

    it("Show that DFA::getNextToken works with multiple regExp/TokenIds", [&](){
      Classifier *classifier = new Classifier();
      classifier->registerClassSet("whitespace",1);
      classifier->classifyUtf8CharsAs(Utf8Chars::whiteSpaceChars,"whitespace");
      NFA *nfa = new NFA(classifier);
      NFABuilder *nfaBuilder = new NFABuilder(nfa);
      nfaBuilder->compileRegularExpressionForTokenId("start", "[whitespace]+", 1);
      nfaBuilder->compileRegularExpressionForTokenId("start", "[!whitespace]+", 2);
      AssertThat(nfa->getNumberStates(), Is().EqualTo(8));
      ParseTrees *parseTrees = new ParseTrees();
      DFA *dfa = new DFA(nfa, parseTrees);
      AssertThat(dfa, Is().Not().EqualTo((void*)0));
      StateAllocator *allocator = dfa->allocator;
      AssertThat(allocator, Is().Not().EqualTo((void*)0));
      dfa->getDFAStartState("start");
      NFA::State *nfaState =
        allocator->stateMatchesToken(dfa->startState[0], dfa->tokensState);
      AssertThat(nfaState, Is().EqualTo((void*)0));
      PushDownMachine *pdm = new PushDownMachine(dfa);
      AssertThat(pdm, Is().Not().EqualTo((void*)0));
      Utf8Chars *stream0 = new Utf8Chars("sillysomeNonWhiteSpace   ");
      ParseTrees::Token *aToken = pdm->runFromUsing("start", stream0);
      AssertThat(aToken, Is().Not().EqualTo((void*)0));
//      AssertThat(aToken->textStart, Is().Not().EqualTo((char*)0));
//      AssertThat(aToken->textLength, Is().Not().EqualTo(0));
      AssertThat(aToken->numTokens, Equals(0));
      AssertThat(aToken->wrappedId, Is().EqualTo(4)); //token:2 ignore:false
      Utf8Chars *stream1 = new Utf8Chars("   sillysomeNonWhiteSpace");
      aToken = pdm->runFromUsing("start", stream1);
      AssertThat(aToken, Is().Not().EqualTo((void*)0));
//      AssertThat(aToken->textStart, Is().Not().EqualTo((char*)0));
//      AssertThat(aToken->textLength, Is().Not().EqualTo(0));
      AssertThat(aToken->numTokens, Equals(0));
      AssertThat(aToken->wrappedId, Is().EqualTo(2)); //token:1 ignore:false
      delete dfa;
      delete parseTrees;
      delete nfa;
      delete classifier;
    });

  }); // dfa
});

}; // namespace DeterministicFiniteAutomaton
