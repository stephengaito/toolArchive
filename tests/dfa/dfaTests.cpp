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
      DFA *dfa = new DFA(nfa);
      AssertThat(dfa, Is().Not().EqualTo((void*)0));
      AssertThat(dfa->nfa, Equals(nfa));
      AssertThat(dfa->allocator, Is().Not().EqualTo((void*)0));
      AssertThat(dfa->nextStateMapping, Is().Not().EqualTo((void*)0));
      AssertThat(((void*)dfa->dfaStartState), Is().Not().EqualTo((void*)0));
      AssertThat((int)dfa->dfaStartState[0], Equals(15));
      for( size_t i = 1; i < dfa->allocator->stateSize; i++) {
        AssertThat(dfa->dfaStartState[i], Equals(0));
      }
      AssertThat(((void*)dfa->tokensState), Is().Not().EqualTo((void*)0));
      for( size_t i = 0; i < dfa->allocator->stateSize; i++) {
        AssertThat(dfa->tokensState[i], Equals(0));
      }
      AssertThat(dfa->allocator->isSubStateOf(dfa->dfaStartState, dfa->tokensState), Is().False());
      delete dfa;
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
      DFA *dfa = new DFA(nfa);
      AssertThat(dfa, Is().Not().EqualTo((void*)0));
      StateAllocator *allocator = dfa->allocator;
      AssertThat(allocator, Is().Not().EqualTo((void*)0));
      NextStateMapping *mapping = dfa->nextStateMapping;
      AssertThat(mapping, Is().Not().EqualTo((void*)0));
      AssertThat(allocator->allocatedUnusedState0, Is().EqualTo((void*)0));
      AssertThat(allocator->allocatedUnusedState1, Is().EqualTo((void*)0));
      AssertThat(allocator->allocatedUnusedState2, Is().EqualTo((void*)0));
      State *aState0 = allocator->allocateANewState(); // this will be generic state
      State *aState1 = allocator->allocateANewState(); // this will be the specific state
      State *aState2 = allocator->allocateANewState(); // should never be used
      allocator->unallocateState(aState0);
      allocator->unallocateState(aState1);
      allocator->unallocateState(aState2);
      utf8Char_t firstChar;
      firstChar.u = 0;
      firstChar.c[0] = 'a';
      Classifier::classSet_t classificationSet = 0;
      State *nextDFAState =
        dfa->computeNextDFAState(dfa->dfaStartState,
                                 firstChar,
                                 classificationSet);
      AssertThat((void*)nextDFAState, Is().Not().EqualTo((void*)0));
      AssertThat((void*)nextDFAState, Is().EqualTo((void*)aState1));
      AssertThat(allocator->isStateEmpty(aState0), Is().True());
      State **registeredState0 =
        (State**)hattrie_tryget(mapping->nextDFAStateMap,
                                aState0, allocator->stateSize);
      AssertThat((void**)registeredState0, Is().EqualTo((void*)0));

      AssertThat(allocator->isStateEmpty(aState1), Is().False());
      State **registeredState1 =
        (State**)hattrie_tryget(mapping->nextDFAStateMap,
                                aState1, allocator->stateSize);
      AssertThat((void**)registeredState1, Is().Not().EqualTo((void*)0));
      AssertThat((void*)*registeredState1, Is().EqualTo((void*)aState1));
      AssertThat(((int)nextDFAState[0]), Is().EqualTo((int)0x30));
      for (size_t i = 1; i < allocator->stateSize; i++) {
        AssertThat(((int)nextDFAState[i]), Is().EqualTo((int)0x00));
      }
      delete dfa;
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
      DFA *dfa = new DFA(nfa);
      AssertThat(dfa, Is().Not().EqualTo((void*)0));
      StateAllocator *allocator = dfa->allocator;
      AssertThat(allocator, Is().Not().EqualTo((void*)0));
      NextStateMapping *mapping = dfa->nextStateMapping;
      AssertThat(mapping, Is().Not().EqualTo((void*)0));
      AssertThat(allocator->allocatedUnusedState0, Is().EqualTo((void*)0));
      AssertThat(allocator->allocatedUnusedState1, Is().EqualTo((void*)0));
      AssertThat(allocator->allocatedUnusedState2, Is().EqualTo((void*)0));
      State *aState0 = allocator->allocateANewState(); // this will be generic state
      State *aState1 = allocator->allocateANewState(); // this will be the specific state
      State *aState2 = allocator->allocateANewState(); // should never be used
      allocator->unallocateState(aState0);
      allocator->unallocateState(aState1);
      allocator->unallocateState(aState2);
      utf8Char_t firstChar;
      firstChar.u = 0;
      firstChar.c[0] = 'a';
      Classifier::classSet_t classificationSet =
        classifier->getClassSet(firstChar);
      AssertThat(classificationSet, Is().EqualTo(~1L));
      State *nextDFAState =
        dfa->computeNextDFAState(dfa->dfaStartState,
                                 firstChar,
                                 classificationSet);
      AssertThat((void*)nextDFAState, Is().Not().EqualTo((void*)0));
      AssertThat((void*)nextDFAState, Is().EqualTo((void*)aState0));
      AssertThat(allocator->isStateEmpty(aState0), Is().False());
      AssertThat(((int)aState0[0]), Is().EqualTo((int)0x20));
      for (size_t i = 1; i < dfa->allocator->stateSize; i++) {
        AssertThat(((int)aState0[i]), Is().EqualTo((int)0x00));
      }
      State **registeredState0 =
        (State**)hattrie_tryget(mapping->nextDFAStateMap,
                                aState0, allocator->stateSize);
      AssertThat((void**)registeredState0, Is().Not().EqualTo((void*)0));
      AssertThat((void*)*registeredState0, Is().EqualTo((void*)aState0));

      AssertThat(allocator->isStateEmpty(aState1), Is().False());
      State **registeredState1 =
        (State**)hattrie_tryget(mapping->nextDFAStateMap,
                                aState1, allocator->stateSize);
      AssertThat((void**)registeredState1, Is().Not().EqualTo((void*)0));
      AssertThat((void*)*registeredState1, Is().EqualTo((void*)aState1));
      AssertThat(((int)nextDFAState[0]), Is().EqualTo((int)0x20));
      for (size_t i = 1; i < dfa->allocator->stateSize; i++) {
        AssertThat(((int)nextDFAState[i]), Is().EqualTo((int)0x00));
      }
      delete dfa;
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
      DFA *dfa = new DFA(nfa);
      AssertThat(dfa, Is().Not().EqualTo((void*)0));
      StateAllocator *allocator = dfa->allocator;
      AssertThat(allocator, Is().Not().EqualTo((void*)0));
      NextStateMapping *mapping = dfa->nextStateMapping;
      AssertThat(mapping, Is().Not().EqualTo((void*)0));
      AssertThat(allocator->allocatedUnusedState0, Is().EqualTo((void*)0));
      AssertThat(allocator->allocatedUnusedState1, Is().EqualTo((void*)0));
      AssertThat(allocator->allocatedUnusedState2, Is().EqualTo((void*)0));
      State *aState0 = allocator->allocateANewState(); // this will be generic state
      State *aState1 = allocator->allocateANewState(); // this will be the specific state
      State *aState2 = allocator->allocateANewState(); // should never be used
      allocator->unallocateState(aState0);
      allocator->unallocateState(aState1);
      allocator->unallocateState(aState2);
      utf8Char_t firstChar;
      firstChar.u = 0;
      firstChar.c[0] = 'a';
      Classifier::classSet_t classificationSet =
        classifier->getClassSet(firstChar);
      AssertThat(classificationSet, Is().EqualTo(~1L));
      State *nextDFAState =
        dfa->computeNextDFAState(dfa->dfaStartState,
                                 firstChar,
                                 classificationSet);
      AssertThat((void*)nextDFAState, Is().Not().EqualTo((void*)0));
      AssertThat((void*)nextDFAState, Is().EqualTo((void*)aState0));
      AssertThat(allocator->isStateEmpty(aState0), Is().False());
      AssertThat(((int)aState0[0]), Is().EqualTo((int)0x30));
      for (size_t i = 1; i < allocator->stateSize; i++) {
        AssertThat(((int)aState0[i]), Is().EqualTo((int)0x00));
      }
      State **registeredState0 =
        (State**)hattrie_tryget(mapping->nextDFAStateMap,
                                aState0, allocator->stateSize);
      AssertThat((void**)registeredState0, Is().Not().EqualTo((void*)0));
      AssertThat((void*)*registeredState0, Is().EqualTo((void*)aState0));

      AssertThat(allocator->isStateEmpty(aState1), Is().True());
      State **registeredState1 =
        (State**)hattrie_tryget(mapping->nextDFAStateMap,
                                aState1, allocator->stateSize);
      AssertThat((void**)registeredState1, Is().EqualTo((void*)0));
      AssertThat(((int)nextDFAState[0]), Is().EqualTo((int)0x30));
      for (size_t i = 1; i < allocator->stateSize; i++) {
        AssertThat(((int)nextDFAState[i]), Is().EqualTo((int)0x00));
      }
      delete dfa;
      delete nfa;
      delete classifier;
    });

    it("Show that DFA::getNextToken works with a simple regular expression", [&](){
      Classifier *classifier = new Classifier();
      NFA *nfa = new NFA(classifier);
      NFABuilder *nfaBuilder = new NFABuilder(nfa);
      nfaBuilder->compileRegularExpressionForTokenId("start", "simple", 1);
      AssertThat(nfa->getNumberStates(), Is().EqualTo(8));
      DFA *dfa = new DFA(nfa);
      AssertThat(dfa, Is().Not().EqualTo((void*)0));
      Utf8Chars *stream0 = new Utf8Chars("simple");
      NFA::TokenId aTokenId = dfa->getNextTokenId(stream0);
      AssertThat(aTokenId, Is().EqualTo(1));
      Utf8Chars *stream1 = new Utf8Chars("notSoSimple");
      aTokenId = dfa->getNextTokenId(stream1);
      AssertThat(aTokenId, Is().EqualTo(-1));
      delete dfa;
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
        (NFA::State*)nfa->stateAllocator->blocks[nfa->stateAllocator->nextBlock - 1];
      for (size_t i = 0; i < 19; i++) {
        AssertThat(baseState[i].matchType, Is().Not().EqualTo(NFA::Token));
      }
      AssertThat(baseState[19].matchType, Equals(NFA::Token));
      for (size_t i = 1; i < 6; i++) {
        AssertThat(baseState[i].out, Equals(baseState+i+1));
      }
      AssertThat(baseState[6].out, Equals(baseState+19));
      DFA *dfa = new DFA(nfa);
      AssertThat(dfa, Is().Not().EqualTo((void*)0));
      Utf8Chars *stream0 = new Utf8Chars("simple");
      NFA::TokenId aTokenId = dfa->getNextTokenId(stream0);
      AssertThat(aTokenId, Is().EqualTo(1));
      Utf8Chars *stream1 = new Utf8Chars("notSoSimple");
      aTokenId = dfa->getNextTokenId(stream1);
      AssertThat(aTokenId, Is().EqualTo(1));
      delete dfa;
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
      DFA *dfa = new DFA(nfa);
      AssertThat(dfa, Is().Not().EqualTo((void*)0));
      Utf8Chars *stream0 = new Utf8Chars("sillysomeNonWhiteSpace");
      NFA::TokenId aTokenId = dfa->getNextTokenId(stream0);
      AssertThat(aTokenId, Is().EqualTo(1));
      delete dfa;
      delete nfa;
      delete classifier;
    });

    it("Show that DFA::getNextTokenId works with multiple regExp/TokenIds", [&](){
      Classifier *classifier = new Classifier();
      classifier->registerClassSet("whitespace",1);
      classifier->classifyUtf8CharsAs(Utf8Chars::whiteSpaceChars,"whitespace");
      NFA *nfa = new NFA(classifier);
      NFABuilder *nfaBuilder = new NFABuilder(nfa);
      nfaBuilder->compileRegularExpressionForTokenId("start", "[whitespace]+", 1);
      nfaBuilder->compileRegularExpressionForTokenId("start", "[!whitespace]+", 2);
      AssertThat(nfa->getNumberStates(), Is().EqualTo(8));
      DFA *dfa = new DFA(nfa);
      AssertThat(dfa, Is().Not().EqualTo((void*)0));
      StateAllocator *allocator = dfa->allocator;
      AssertThat(allocator, Is().Not().EqualTo((void*)0));
      NFA::State *nfaState =
        allocator->stateMatchesToken(dfa->dfaStartState, dfa->tokensState);
      AssertThat(nfaState, Is().EqualTo((void*)0));
      Utf8Chars *stream0 = new Utf8Chars("sillysomeNonWhiteSpace   ");
      NFA::TokenId aTokenId = dfa->getNextTokenId(stream0);
      AssertThat(aTokenId, Is().EqualTo(2));
      Utf8Chars *stream1 = new Utf8Chars("   sillysomeNonWhiteSpace");
      aTokenId = dfa->getNextTokenId(stream1);
      AssertThat(aTokenId, Is().EqualTo(1));
      delete dfa;
      delete nfa;
      delete classifier;
    });

  }); // dfa
});

}; // namespace DeterministicFiniteAutomaton
