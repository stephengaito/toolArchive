#include <bandit/bandit.h>
using namespace bandit;

#include <string.h>
#include <stdio.h>

#ifndef private
#define private public
#endif

#include <dfa.h>

go_bandit([](){

  printf("\n----------------------------------\n");
  printf(  "dfa\n");
  printf(  "           DFA = %zu bytes (%zu bits)\n", sizeof(DFA::DFA), sizeof(DFA::DFA)*8);
  printf(  "NFAStateNumber = %zu bytes (%zu bits)\n", sizeof(DFA::NFAStateNumber), sizeof(DFA::NFAStateNumber)*8);
  printf(  "----------------------------------\n");

  /// \brief Test the ability of a given DFA class to compile, on the fly,
  /// a DFA corresponding to a given NFA.
  describe("DFA", [](){

    /// Show that we can create an appropriately allocated DFA
    /// from a given NFA.
    it("Should have correct sizes and pointers setup", [&](){
      Classifier *classifier = new Classifier();
      NFA *nfa = new NFA(classifier);
      nfa->addRegularExpressionForToken("(abab|abbb)", 1);
      AssertThat(nfa->getNumberStates(), Is().EqualTo(11));
      DFA *dfa = new DFA(nfa);
      AssertThat(dfa->dfaStateSize, Is().EqualTo(2)); // at most 16 NFA state bits
      AssertThat(dfa->dStateVectorSize, Is().EqualTo(40));
      AssertThat(dfa->dfaStateProbeSize, Is().EqualTo(2+sizeof(utf8Char_t)));
      AssertThat(dfa->dfaStateProbe, Is().Not().EqualTo((void*)0));
      AssertThat(dfa->nfaStatePtr2int, Is().Not().EqualTo((void*)0));
      AssertThat(dfa->int2nfaStatePtr, Is().Not().EqualTo((NFA::State**)0));
      AssertThat(dfa->allocatedUnusedDState0, Is().EqualTo((void*)0));
      AssertThat(dfa->allocatedUnusedDState1, Is().EqualTo((void*)0));
      AssertThat(dfa->allocatedUnusedDState2, Is().EqualTo((void*)0));
      AssertThat(dfa->dStates, Is().Not().EqualTo((void*)0));
      AssertThat(dfa->curDStateVector, Is().EqualTo(1));
      AssertThat(((void*)dfa->dfaStartState), Is().EqualTo((void*)dfa->dStates[0]));
      AssertThat(((void*)dfa->tokensDState), Is().EqualTo((void*)(dfa->dStates[0]+dfa->dfaStateSize)));
      for( size_t i = 0; i < dfa->dfaStateSize; i++) {
        AssertThat(dfa->tokensDState[i], Is().EqualTo(0));
      }
      AssertThat(dfa->isSubDState(dfa->dfaStartState, dfa->tokensDState), Is().False());
    });

    /// Show that we can allocate and unallocate DFA::DStates, to/from
    /// the three allocatedUnusedDState{0|1|2}.
    it("Allocate and unallocate DStates", [&](){
      Classifier *classifier = new Classifier();
      NFA *nfa = new NFA(classifier);
      nfa->addRegularExpressionForToken("(abab|abbb)", 1);
      AssertThat(nfa->getNumberStates(), Is().EqualTo(11));
      DFA *dfa = new DFA(nfa);
      AssertThat(dfa->allocatedUnusedDState0, Is().EqualTo((void*)0));
      AssertThat(dfa->allocatedUnusedDState1, Is().EqualTo((void*)0));
      AssertThat(dfa->allocatedUnusedDState2, Is().EqualTo((void*)0));
      DFA::DState *aDState0 = dfa->allocateANewDState();
      DFA::DState *aDState1 = dfa->allocateANewDState();
      DFA::DState *aDState2 = dfa->allocateANewDState();
      DFA::DState *aDState3 = dfa->allocateANewDState();
      AssertThat(aDState0, Is().Not().EqualTo(aDState1));
      AssertThat(aDState0, Is().Not().EqualTo(aDState2));
      AssertThat(aDState1, Is().Not().EqualTo(aDState2));
      AssertThat(dfa->allocatedUnusedDState0, Is().EqualTo((void*)0));
      AssertThat(dfa->allocatedUnusedDState1, Is().EqualTo((void*)0));
      AssertThat(dfa->allocatedUnusedDState2, Is().EqualTo((void*)0));
      dfa->unallocateADState(aDState0);
      AssertThat(dfa->allocatedUnusedDState0, Is().EqualTo(aDState0));
      AssertThat(dfa->allocatedUnusedDState1, Is().EqualTo((void*)0));
      AssertThat(dfa->allocatedUnusedDState2, Is().EqualTo((void*)0));
      dfa->unallocateADState(aDState1);
      AssertThat(dfa->allocatedUnusedDState0, Is().EqualTo(aDState0));
      AssertThat(dfa->allocatedUnusedDState1, Is().EqualTo(aDState1));
      AssertThat(dfa->allocatedUnusedDState2, Is().EqualTo((void*)0));
      dfa->unallocateADState(aDState2);
      AssertThat(dfa->allocatedUnusedDState0, Is().EqualTo(aDState0));
      AssertThat(dfa->allocatedUnusedDState1, Is().EqualTo(aDState1));
      AssertThat(dfa->allocatedUnusedDState2, Is().EqualTo(aDState2));
      dfa->unallocateADState(aDState3); // quietly dropped aDState3 ;-(
      AssertThat(dfa->allocatedUnusedDState0, Is().EqualTo(aDState0));
      AssertThat(dfa->allocatedUnusedDState1, Is().EqualTo(aDState1));
      AssertThat(dfa->allocatedUnusedDState2, Is().EqualTo(aDState2));
      DFA::DState *aNewDState0 = dfa->allocateANewDState();
      AssertThat(aDState0, Is().EqualTo(aNewDState0));
      AssertThat(dfa->allocatedUnusedDState0, Is().EqualTo((void*)0));
      AssertThat(dfa->allocatedUnusedDState1, Is().EqualTo(aDState1));
      AssertThat(dfa->allocatedUnusedDState2, Is().EqualTo(aDState2));
      dfa->unallocateADState(aNewDState0);
      AssertThat(dfa->allocatedUnusedDState0, Is().EqualTo(aNewDState0));
      AssertThat(dfa->allocatedUnusedDState1, Is().EqualTo(aDState1));
      AssertThat(dfa->allocatedUnusedDState2, Is().EqualTo(aDState2));
      for (size_t i = 0; i < 100; i++) {
        DFA::DState *someNewDStates = dfa->allocateANewDState();
        AssertThat(someNewDStates, Is().Not().EqualTo((void*)0));
      }
    });

    /// Show that DFA::getNFAStateNumber computes correct
    /// DFA::NFAStateNumber(s). In particular we need to show that
    /// we can deal with *more* than 8 NFA::States (i.e. more than the
    /// first byte in the NFAStateNumber/DFA::DState bit set).
    it("Should compute correct NFAStateNumbers using getNFAStateNumber", [&](){
      Classifier *classifier = new Classifier();
      NFA *nfa = new NFA(classifier);
      nfa->addRegularExpressionForToken("thisisasimpletest", 1);
      AssertThat(nfa->getNumberStates(), Is().EqualTo(19));
      DFA *dfa = new DFA(nfa);
      AssertThat(dfa->numKnownNFAStates, Is().EqualTo(2));
      NFA::State *nfaStartState = nfa->getNFAStartState();
      AssertThat(dfa->int2nfaStatePtr[0], Is().EqualTo(nfaStartState));
      long long tmpNFAState = (long long)nfaStartState;
      char nfaStatePtr[sizeof(NFA::State*)];
      for (size_t i = 0; i < sizeof(NFA::State*); i++) {
        nfaStatePtr[i] = tmpNFAState & 0xFF;
        tmpNFAState >>=8;
      }
      AssertThat(*hattrie_get(dfa->nfaStatePtr2int, nfaStatePtr, sizeof(NFA::State*)), Is().EqualTo(1));
      AssertThat(dfa->int2nfaStatePtr[1], Is().EqualTo(nfaStartState->out));
      tmpNFAState = (long long)(nfaStartState->out);
      for (size_t i = 0; i < sizeof(NFA::State*); i++) {
        nfaStatePtr[i] = tmpNFAState & 0xFF;
        tmpNFAState >>=8;
      }
      AssertThat(*hattrie_get(dfa->nfaStatePtr2int, nfaStatePtr, sizeof(NFA::State*)), Is().EqualTo(2));
      AssertThat(dfa->int2nfaStatePtr[2], Is().EqualTo((void*)0));
      AssertThat(dfa->numKnownNFAStates, Is().EqualTo(2));
      DFA::NFAStateNumber aStateNum = dfa->getNFAStateNumber(nfaStartState);
      AssertThat(dfa->numKnownNFAStates, Is().EqualTo(2));
      AssertThat(aStateNum.stateByte, Is().EqualTo(0));
      AssertThat((int)aStateNum.stateBit, Is().EqualTo(1));
      aStateNum = dfa->getNFAStateNumber(nfaStartState->out);
      AssertThat(dfa->numKnownNFAStates, Is().EqualTo(2));
      AssertThat(aStateNum.stateByte, Is().EqualTo(0));
      AssertThat((int)aStateNum.stateBit, Is().EqualTo(2));
      NFA::State *nextState = nfaStartState->out;
      aStateNum = dfa->getNFAStateNumber(nextState->out);
      AssertThat(dfa->numKnownNFAStates, Is().EqualTo(3));
      AssertThat(aStateNum.stateByte, Is().EqualTo(0));
      AssertThat((int)aStateNum.stateBit, Is().EqualTo(4));
      nextState = nextState->out;
      aStateNum = dfa->getNFAStateNumber(nextState->out);
      AssertThat(dfa->numKnownNFAStates, Is().EqualTo(4));
      AssertThat(aStateNum.stateByte, Is().EqualTo(0));
      AssertThat((int)aStateNum.stateBit, Is().EqualTo(8));
    });

    it("Show that DFA::getNFAStateNumber can deal with lots of NFA::States", [&](){
      Classifier *classifier = new Classifier();
      NFA *nfa = new NFA(classifier);
      nfa->addRegularExpressionForToken("thisisasimpletest", 1);
      AssertThat(nfa->getNumberStates(), Is().EqualTo(19));
      DFA *dfa = new DFA(nfa);
      NFA::State *baseState = nfa->states[nfa->curStateVector];
      DFA::NFAStateNumber aStateNum = dfa->getNFAStateNumber(baseState);
      AssertThat(aStateNum.stateByte, Is().EqualTo(0));
      AssertThat((int)aStateNum.stateBit,  Is().EqualTo(1));
      aStateNum = dfa->getNFAStateNumber(baseState+1);
      AssertThat(aStateNum.stateByte, Is().EqualTo(0));
      AssertThat((int)aStateNum.stateBit,  Is().EqualTo(2));
      aStateNum = dfa->getNFAStateNumber(baseState+2);
      AssertThat(aStateNum.stateByte, Is().EqualTo(0));
      AssertThat((int)aStateNum.stateBit,  Is().EqualTo(4));
      aStateNum = dfa->getNFAStateNumber(baseState+3);
      AssertThat(aStateNum.stateByte, Is().EqualTo(0));
      AssertThat((int)aStateNum.stateBit,  Is().EqualTo(8));
      aStateNum = dfa->getNFAStateNumber(baseState+4);
      AssertThat(aStateNum.stateByte, Is().EqualTo(0));
      AssertThat((int)aStateNum.stateBit,  Is().EqualTo(16));
      aStateNum = dfa->getNFAStateNumber(baseState+5);
      AssertThat(aStateNum.stateByte, Is().EqualTo(0));
      AssertThat((int)aStateNum.stateBit,  Is().EqualTo(32));
      aStateNum = dfa->getNFAStateNumber(baseState+6);
      AssertThat(aStateNum.stateByte, Is().EqualTo(0));
      AssertThat((int)aStateNum.stateBit,  Is().EqualTo(64));
      aStateNum = dfa->getNFAStateNumber(baseState+7);
      AssertThat(aStateNum.stateByte, Is().EqualTo(0));
      AssertThat((int)aStateNum.stateBit,  Is().EqualTo(128));
      aStateNum = dfa->getNFAStateNumber(baseState+8);
      AssertThat(aStateNum.stateByte, Is().EqualTo(1));
      AssertThat((int)aStateNum.stateBit,  Is().EqualTo(1));
      aStateNum = dfa->getNFAStateNumber(baseState+9);
      AssertThat(aStateNum.stateByte, Is().EqualTo(1));
      AssertThat((int)aStateNum.stateBit,  Is().EqualTo(2));
      aStateNum = dfa->getNFAStateNumber(baseState+10);
      AssertThat(aStateNum.stateByte, Is().EqualTo(1));
      AssertThat((int)aStateNum.stateBit,  Is().EqualTo(4));
      aStateNum = dfa->getNFAStateNumber(baseState+11);
      AssertThat(aStateNum.stateByte, Is().EqualTo(1));
      AssertThat((int)aStateNum.stateBit,  Is().EqualTo(8));
      aStateNum = dfa->getNFAStateNumber(baseState+12);
      AssertThat(aStateNum.stateByte, Is().EqualTo(1));
      AssertThat((int)aStateNum.stateBit,  Is().EqualTo(16));
      aStateNum = dfa->getNFAStateNumber(baseState+13);
      AssertThat(aStateNum.stateByte, Is().EqualTo(1));
      AssertThat((int)aStateNum.stateBit,  Is().EqualTo(32));
      aStateNum = dfa->getNFAStateNumber(baseState+14);
      AssertThat(aStateNum.stateByte, Is().EqualTo(1));
      AssertThat((int)aStateNum.stateBit,  Is().EqualTo(64));
      aStateNum = dfa->getNFAStateNumber(baseState+15);
      AssertThat(aStateNum.stateByte, Is().EqualTo(1));
      AssertThat((int)aStateNum.stateBit,  Is().EqualTo(128));
      aStateNum = dfa->getNFAStateNumber(baseState+16);
      AssertThat(aStateNum.stateByte, Is().EqualTo(2));
      AssertThat((int)aStateNum.stateBit,  Is().EqualTo(1));
      aStateNum = dfa->getNFAStateNumber(baseState+17);
      AssertThat(aStateNum.stateByte, Is().EqualTo(2));
      AssertThat((int)aStateNum.stateBit,  Is().EqualTo(2));
      aStateNum = dfa->getNFAStateNumber(baseState+18);
      AssertThat(aStateNum.stateByte, Is().EqualTo(2));
      AssertThat((int)aStateNum.stateBit,  Is().EqualTo(4));
      try {
        aStateNum = dfa->getNFAStateNumber(baseState+19);
        AssertThat(aStateNum.stateByte, Is().EqualTo(2));
        AssertThat((int)aStateNum.stateBit,  Is().EqualTo(16));
        AssertThat(false, Is().True());
      } catch (LexerException& e) {
        AssertThat(true, Is().True());
      }
    });

    /// Using the regular expression: /(abab|abbb)/ which has
    /// a moderately complex starting collection of NFA::State(s),
    /// show that we can compute the correct DFA::DState.
    it("Should compute the correct start state", [&](){
      Classifier *classifier = new Classifier();
      NFA *nfa = new NFA(classifier);
      nfa->addRegularExpressionForToken("(abab|abbb)", 1);
      AssertThat(nfa->getNumberStates(), Is().EqualTo(11));
      DFA *dfa = new DFA(nfa);
      NFA::State *nfaStartState = nfa->getNFAStartState();
      AssertThat(nfaStartState, Is().Not().EqualTo((void*)0));
      AssertThat(nfaStartState->matchType, Is().EqualTo(NFA::Split));
      AssertThat(nfaStartState->out, Is().Not().EqualTo((void*)0));
      AssertThat(nfaStartState->out1, Is().EqualTo((void*)0));
      DFA::NFAStateNumber nfaStateNum = dfa->getNFAStateNumber(nfaStartState);
      AssertThat(dfa->int2nfaStatePtr[0], Is().EqualTo(nfaStartState));
      AssertThat(nfaStateNum.stateByte, Is().EqualTo(0));
      AssertThat((int)nfaStateNum.stateBit,  Is().EqualTo(1));
      AssertThat(((int)dfa->dfaStartState[0]), Is().EqualTo((int)0x0F));
      for (size_t i = 1; i < dfa->dfaStateSize; i++) {
        AssertThat(((int)dfa->dfaStartState[i]), Is().EqualTo((int)0x00));
      }
    });

    it("Should be able to register a DFA::DState using DFA::registerDState", [&](){
      Classifier *classifier = new Classifier();
      NFA *nfa = new NFA(classifier);
      nfa->addRegularExpressionForToken("(abab|abbb)", 1);
      AssertThat(nfa->getNumberStates(), Is().EqualTo(11));
      DFA *dfa = new DFA(nfa);
      // dfaStartState is already registered..
      DFA::DState **registeredDState =
        (DFA::DState **)hattrie_tryget(dfa->nextDFAStateMap,
                                      dfa->dfaStartState,
                                      dfa->dfaStateSize);
      AssertThat((void*)registeredDState, Is().Not().EqualTo((void*)0));
      AssertThat((void*)*registeredDState, Is().EqualTo((void*)dfa->dfaStartState));
    });

    /// Show that DFA::computeNextDFAState can compile a simple
    /// NFA corresponding to the regular expression: /(abab|abbb)/
    /// which has only characters to match.
    it("Should computeNextDFAState with no generic states", [&](){
      Classifier *classifier = new Classifier();
      NFA *nfa = new NFA(classifier);
      nfa->addRegularExpressionForToken("(abab|abbb)", 1);
      AssertThat(nfa->getNumberStates(), Is().EqualTo(11));
      DFA *dfa = new DFA(nfa);
      AssertThat(dfa->allocatedUnusedDState0, Is().EqualTo((void*)0));
      AssertThat(dfa->allocatedUnusedDState1, Is().EqualTo((void*)0));
      AssertThat(dfa->allocatedUnusedDState2, Is().EqualTo((void*)0));
      DFA::DState *aDState0 = dfa->allocateANewDState(); // this will be generic state
      DFA::DState *aDState1 = dfa->allocateANewDState(); // this will be the specific state
      DFA::DState *aDState2 = dfa->allocateANewDState(); // should never be used
      dfa->unallocateADState(aDState0);
      dfa->unallocateADState(aDState1);
      dfa->unallocateADState(aDState2);
      utf8Char_t firstChar;
      firstChar.u = 0;
      firstChar.c[0] = 'a';
      Classifier::classSet_t classificationSet = 0;
      DFA::DState *nextDFAState =
        dfa->computeNextDFAState(dfa->dfaStartState,
                                 firstChar,
                                 classificationSet);
      AssertThat((void*)nextDFAState, Is().Not().EqualTo((void*)0));
      AssertThat((void*)nextDFAState, Is().EqualTo((void*)aDState1));
      AssertThat(dfa->isEmptyDState(aDState0), Is().True());
      DFA::DState **registeredDState0 =
        (DFA::DState**)hattrie_tryget(dfa->nextDFAStateMap,
                                      aDState0,
                                      dfa->dfaStateSize);
      AssertThat((void**)registeredDState0, Is().EqualTo((void*)0));

      AssertThat(dfa->isEmptyDState(aDState1), Is().False());
      DFA::DState **registeredDState1 =
        (DFA::DState**)hattrie_tryget(dfa->nextDFAStateMap,
                                      aDState1,
                                      dfa->dfaStateSize);
      AssertThat((void**)registeredDState1, Is().Not().EqualTo((void*)0));
      AssertThat((void*)*registeredDState1, Is().EqualTo((void*)aDState1));
      AssertThat(((int)nextDFAState[0]), Is().EqualTo((int)0x30));
      for (size_t i = 1; i < dfa->dfaStateSize; i++) {
        AssertThat(((int)nextDFAState[i]), Is().EqualTo((int)0x00));
      }
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
      AssertThat(classifier->getClassSet("a"), Is().EqualTo(~0L));
      NFA *nfa = new NFA(classifier);
      nfa->addRegularExpressionForToken("(abab|[!whitespace]bbb)", 1);
      AssertThat(nfa->getNumberStates(), Is().EqualTo(11));
      DFA *dfa = new DFA(nfa);
      AssertThat(dfa->allocatedUnusedDState0, Is().EqualTo((void*)0));
      AssertThat(dfa->allocatedUnusedDState1, Is().EqualTo((void*)0));
      AssertThat(dfa->allocatedUnusedDState2, Is().EqualTo((void*)0));
      DFA::DState *aDState0 = dfa->allocateANewDState(); // this will be generic state
      DFA::DState *aDState1 = dfa->allocateANewDState(); // this will be the specific state
      DFA::DState *aDState2 = dfa->allocateANewDState(); // should never be used
      dfa->unallocateADState(aDState0);
      dfa->unallocateADState(aDState1);
      dfa->unallocateADState(aDState2);
      utf8Char_t firstChar;
      firstChar.u = 0;
      firstChar.c[0] = 'a';
      Classifier::classSet_t classificationSet =
        classifier->getClassSet(firstChar);
      AssertThat(classificationSet, Is().EqualTo(~0L));
      DFA::DState *nextDFAState =
        dfa->computeNextDFAState(dfa->dfaStartState,
                                 firstChar,
                                 classificationSet);
      AssertThat((void*)nextDFAState, Is().Not().EqualTo((void*)0));
      AssertThat((void*)nextDFAState, Is().EqualTo((void*)aDState0));
      AssertThat(dfa->isEmptyDState(aDState0), Is().False());
      AssertThat(((int)aDState0[0]), Is().EqualTo((int)0x20));
      for (size_t i = 1; i < dfa->dfaStateSize; i++) {
        AssertThat(((int)aDState0[i]), Is().EqualTo((int)0x00));
      }
      DFA::DState **registeredDState0 =
        (DFA::DState**)hattrie_tryget(dfa->nextDFAStateMap,
                                      aDState0,
                                      dfa->dfaStateSize);
      AssertThat((void**)registeredDState0, Is().Not().EqualTo((void*)0));
      AssertThat((void*)*registeredDState0, Is().EqualTo((void*)aDState0));

      AssertThat(dfa->isEmptyDState(aDState1), Is().False());
      DFA::DState **registeredDState1 =
        (DFA::DState**)hattrie_tryget(dfa->nextDFAStateMap,
                                      aDState1,
                                      dfa->dfaStateSize);
      AssertThat((void**)registeredDState1, Is().Not().EqualTo((void*)0));
      AssertThat((void*)*registeredDState1, Is().EqualTo((void*)aDState1));
      AssertThat(((int)nextDFAState[0]), Is().EqualTo((int)0x20));
      for (size_t i = 1; i < dfa->dfaStateSize; i++) {
        AssertThat(((int)nextDFAState[i]), Is().EqualTo((int)0x00));
      }
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
      AssertThat(classifier->getClassSet("a"), Is().EqualTo(~0L));
      NFA *nfa = new NFA(classifier);
      nfa->addRegularExpressionForToken("([!whitespace]bab|[!whitespace]bbb)", 1);
      AssertThat(nfa->getNumberStates(), Is().EqualTo(11));
      DFA *dfa = new DFA(nfa);
      AssertThat(dfa->allocatedUnusedDState0, Is().EqualTo((void*)0));
      AssertThat(dfa->allocatedUnusedDState1, Is().EqualTo((void*)0));
      AssertThat(dfa->allocatedUnusedDState2, Is().EqualTo((void*)0));
      DFA::DState *aDState0 = dfa->allocateANewDState(); // this will be generic state
      DFA::DState *aDState1 = dfa->allocateANewDState(); // this will be the specific state
      DFA::DState *aDState2 = dfa->allocateANewDState(); // should never be used
      dfa->unallocateADState(aDState0);
      dfa->unallocateADState(aDState1);
      dfa->unallocateADState(aDState2);
      utf8Char_t firstChar;
      firstChar.u = 0;
      firstChar.c[0] = 'a';
      Classifier::classSet_t classificationSet =
        classifier->getClassSet(firstChar);
      AssertThat(classificationSet, Is().EqualTo(~0L));
      DFA::DState *nextDFAState =
        dfa->computeNextDFAState(dfa->dfaStartState,
                                 firstChar,
                                 classificationSet);
      AssertThat((void*)nextDFAState, Is().Not().EqualTo((void*)0));
      AssertThat((void*)nextDFAState, Is().EqualTo((void*)aDState0));
      AssertThat(dfa->isEmptyDState(aDState0), Is().False());
      AssertThat(((int)aDState0[0]), Is().EqualTo((int)0x30));
      for (size_t i = 1; i < dfa->dfaStateSize; i++) {
        AssertThat(((int)aDState0[i]), Is().EqualTo((int)0x00));
      }
      DFA::DState **registeredDState0 =
        (DFA::DState**)hattrie_tryget(dfa->nextDFAStateMap,
                                      aDState0,
                                      dfa->dfaStateSize);
      AssertThat((void**)registeredDState0, Is().Not().EqualTo((void*)0));
      AssertThat((void*)*registeredDState0, Is().EqualTo((void*)aDState0));

      AssertThat(dfa->isEmptyDState(aDState1), Is().True());
      DFA::DState **registeredDState1 =
        (DFA::DState**)hattrie_tryget(dfa->nextDFAStateMap,
                                      aDState1,
                                      dfa->dfaStateSize);
      AssertThat((void**)registeredDState1, Is().EqualTo((void*)0));
      AssertThat(((int)nextDFAState[0]), Is().EqualTo((int)0x30));
      for (size_t i = 1; i < dfa->dfaStateSize; i++) {
        AssertThat(((int)nextDFAState[i]), Is().EqualTo((int)0x00));
      }
    });

    it("Show that DFA::getNextToken works with a simple regular expression", [&](){
      Classifier *classifier = new Classifier();
      NFA *nfa = new NFA(classifier);
      nfa->addRegularExpressionForToken("simple", 1);
      AssertThat(nfa->getNumberStates(), Is().EqualTo(8));
      DFA *dfa = new DFA(nfa);
      Utf8Chars *stream0 = new Utf8Chars("simple");
      AssertThat(dfa->getNextToken(stream0), Is().True());
      Utf8Chars *stream1 = new Utf8Chars("notSoSimple");
      AssertThat(dfa->getNextToken(stream1), Is().False());
    });

    it("Show that DFA::getNextToken works with simple regular expression with alternate patterns", [&](){
      Classifier *classifier = new Classifier();
      NFA *nfa = new NFA(classifier);
      nfa->addRegularExpressionForToken("(simple|notSoSimple)", 1);
      AssertThat(nfa->getNumberStates(), Is().EqualTo(20));
      DFA *dfa = new DFA(nfa);
      Utf8Chars *stream0 = new Utf8Chars("simple");
      AssertThat(dfa->getNextToken(stream0), Is().True());
      Utf8Chars *stream1 = new Utf8Chars("notSoSimple");
      AssertThat(dfa->getNextToken(stream1), Is().True());
    });

    it("Show that DFA::getNextToken works with a regular expression with only Classifier::classSet_t transitions", [&](){
      Classifier *classifier = new Classifier();
      classifier->registerClassSet("whitespace",1);
      classifier->classifyUtf8CharsAs(Utf8Chars::whiteSpaceChars,"whitespace");
      AssertThat(classifier->getClassSet(" "), Is().EqualTo(1));
      AssertThat(classifier->getClassSet("a"), Is().EqualTo(~0L));
      NFA *nfa = new NFA(classifier);
      nfa->addRegularExpressionForToken("[!whitespace]+", 1);
      AssertThat(nfa->getNumberStates(), Is().EqualTo(4));
      DFA *dfa = new DFA(nfa);
      Utf8Chars *stream0 = new Utf8Chars("sillysomeNonWhiteSpace");
      AssertThat(dfa->getNextToken(stream0), Is().True());
    });

  }); // dfa
});
