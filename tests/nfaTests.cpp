#include <bandit/bandit.h>
using namespace bandit;

#include <string.h>
#include <stdio.h>
#include <exception>

#ifndef private
#define private public
#endif

// The following macro is used by the NFA's initialization of the
// BlockAllocator.  For (most) of our testing this number MUST be larger
// than the largets number of states in any of the NFA's being tested
// so that we can have one continuous baseStates.
//
#define NUM_NFA_STATES_PER_BLOCK 20

#include <nfaBuilder.h>

go_bandit([](){

  printf("\n----------------------------------\n");
  printf(  "nfa\n");
  printf(  "MatchType = %zu bytes (%zu bits)\n", sizeof(NFA::MatchType), sizeof(NFA::MatchType)*8);
  printf(  "MatchData = %zu bytes (%zu bits)\n", sizeof(NFA::MatchData), sizeof(NFA::MatchData)*8);
  printf(  "NFAState* = %zu bytes (%zu bits)\n", sizeof(NFA::State*), sizeof(NFA::State*)*8);
  printf(  " NFAState = %zu bytes (%zu bits)\n", sizeof(NFA::State),  sizeof(NFA::State)*8);
  printf(  "----------------------------------\n");

  /// \brief We test the correctness of the Nondeterministic Finite
  /// Automata associated with a given Regular Expression.
  ///
  /// NOTE: These tests are essentially only testing one function
  /// which has a complex end result, by walking over the resulting
  /// NFA structure. This leads to dense and fragile tests.
  ///
  /// Since these NFAs will be used as part of a Lexer with multiple
  /// tokens to be recognized, the NFA start state(s) consist of
  /// what is essentially a linked list of NFA::Split states whose
  /// out pointer points to the (sub)NFA to recognize a given token
  /// and whose out1 pointer points to the next (sub)NFA/token in
  /// the linked list. Each (sub)NFA will end with an NFA::Token state
  /// whose matchData stores the token ID associated with the recognized
  /// token. All test hence must recognize both this first NFA::Split
  /// and final NFA::Token states.
  describe("NFA", [](){

    it("should create NFA object with correct instance variables", [](){
      Classifier *classifier = new Classifier();
      NFA *nfa = new NFA(classifier);
      AssertThat(nfa->stateAllocator, Is().Not().EqualTo((void*)0));
      AssertThat(nfa->startStateIds,  Is().Not().EqualTo((void*)0));
      AssertThat(nfa->startState.getNumItems(), Equals(0));
      AssertThat(nfa->numKnownStates, Equals(0));
      AssertThat(nfa->utf8Classifier, Equals(classifier));
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
        states[i] = nfa->addState(NFA::Split, noMatchData, NULL, NULL, "test");
        nfa->appendNFAToStartState(buffer, states[i]);
        AssertThat(nfa->getNumberStartStates(), Equals(i+1));
        AssertThat(nfa->findStartStateId(buffer), Equals(i));
        AssertThat(nfa->getStartState(buffer), Equals(states[i]));
        AssertThat(nfa->getStartState((NFA::StartStateId)i), Equals(states[i]));
      }
      delete nfa;
      delete classifier;
    });

    /// We build the NFA associated with the very simple regular
    /// expression: /simple/. This yeilds a deep linear linked
    /// list of character comparisions started by a single
    /// split. Other than the last NFA::State (which is an NFA::Token
    /// state) all other states are simple NFA::Character states
    /// whose matchData is the character to be matched.
    it("Should build NFA for a simple regular expression", [&](){
      Classifier *classifier = new Classifier();
      NFA *nfa = new NFA(classifier);
      NFABuilder *nfaBuilder = new NFABuilder(nfa);
      nfaBuilder->compileRegularExpressionForTokenId("start", "simple", 1);
      NFA::State *anNFAState = nfa->getStartState("start");
      AssertThat(nfa->getNumberStates(), Is().EqualTo(8));
      AssertThat(anNFAState, Is().EqualTo(nfa->startState.getItem(0, NULL)));
      AssertThat(nfa->getNumberStartStates(), Equals(1));
      NFA::State *baseState =
        (NFA::State*)nfa->stateAllocator->blocks.getTop();
      AssertThat(anNFAState, Is().EqualTo(baseState));
      AssertThat(anNFAState->matchType, Is().EqualTo(NFA::Split));
      AssertThat(anNFAState->matchData.c.u, Is().EqualTo((uint64_t)0));
      AssertThat(anNFAState->out1, Is().EqualTo((NFA::State*)0));
      NFA::State *nextState = anNFAState->out;
      AssertThat(nextState, Is().EqualTo(baseState+1));
      AssertThat(nextState->matchType, Is().EqualTo(NFA::Character));
      utf8Char_t expectedChar;
      expectedChar.u = 0;
      expectedChar.c[0] = 's';
      AssertThat(nextState->matchData.c.u, Is().EqualTo(expectedChar.u));
      AssertThat(nextState->out1, Is().EqualTo((NFA::State*)0));
      nextState = nextState->out;
      AssertThat(nextState, Is().EqualTo(baseState+2));
      AssertThat(nextState->matchType, Is().EqualTo(NFA::Character));
      expectedChar.c[0] = 'i';
      AssertThat(nextState->matchData.c.u, Is().EqualTo(expectedChar.u));
      AssertThat(nextState->out1, Is().EqualTo((NFA::State*)0));
      nextState = nextState->out;
      AssertThat(nextState, Is().EqualTo(baseState+3));
      AssertThat(nextState->matchType, Is().EqualTo(NFA::Character));
      expectedChar.c[0] = 'm';
      AssertThat(nextState->matchData.c.u, Is().EqualTo(expectedChar.u));
      AssertThat(nextState->out1, Is().EqualTo((NFA::State*)0));
      nextState = nextState->out;
      AssertThat(nextState, Is().EqualTo(baseState+4));
      AssertThat(nextState->matchType, Is().EqualTo(NFA::Character));
      expectedChar.c[0] = 'p';
      AssertThat(nextState->matchData.c.u, Is().EqualTo(expectedChar.u));
      AssertThat(nextState->out1, Is().EqualTo((NFA::State*)0));
      nextState = nextState->out;
      AssertThat(nextState, Is().EqualTo(baseState+5));
      AssertThat(nextState->matchType, Is().EqualTo(NFA::Character));
      expectedChar.c[0] = 'l';
      AssertThat(nextState->matchData.c.u, Is().EqualTo(expectedChar.u));
      AssertThat(nextState->out1, Is().EqualTo((NFA::State*)0));
      nextState = nextState->out;
      AssertThat(nextState, Is().EqualTo(baseState+6));
      AssertThat(nextState->matchType, Is().EqualTo(NFA::Character));
      expectedChar.c[0] = 'e';
      AssertThat(nextState->matchData.c.u, Is().EqualTo(expectedChar.u));
      AssertThat(nextState->out1, Is().EqualTo((NFA::State*)0));
      nextState = nextState->out;
      AssertThat(nextState, Is().EqualTo(baseState+7));
      AssertThat(nextState->matchType, Is().EqualTo(NFA::Token));
      AssertThat(nextState->matchData.t, Is().EqualTo(2)); // token:1 ignore:false
      AssertThat(nextState->out,  Is().EqualTo((NFA::State*)0));
      AssertThat(nextState->out1, Is().EqualTo((NFA::State*)0));
      delete nfa;
      delete classifier;
    });

    it("Should build NFA for a regular expression with alternatives", [&](){
      Classifier *classifier = new Classifier();
      NFA *nfa = new NFA(classifier);
      NFABuilder *nfaBuilder = new NFABuilder(nfa);
      nfaBuilder->compileRegularExpressionForTokenId("start", "(a|bc)", 1);
      AssertThat(nfa->getNumberStates(), Equals(6));
      NFA::State *startState = nfa->getStartState("start");
      AssertThat(startState, Is().Not().EqualTo((void*)0));
      NFA::State *baseState =
        (NFA::State*)nfa->stateAllocator->blocks.getTop();
      AssertThat(startState, Equals(baseState));
      AssertThat(startState->matchType, Equals(NFA::Split));
      AssertThat(startState->out1, Equals((void*)0));
      NFA::State *nextState = startState->out;
      AssertThat(nextState, Is().Not().EqualTo((void*)0));
      AssertThat(nextState, Equals(baseState+4));
      AssertThat(nextState->matchType, Equals(NFA::Split));
      NFA::State *aState = nextState->out;
      AssertThat(aState, Is().Not().EqualTo((void*)0));
      AssertThat(aState, Equals(baseState+1));
      AssertThat(aState->matchType, Equals(NFA::Character));
      AssertThat(aState->matchData.c.c[0], Equals('a'));
      AssertThat(aState->out1, Equals((void*)0));
      NFA::State *bState = nextState->out1;
      AssertThat(bState, Is().Not().EqualTo((void*)0));
      AssertThat(bState, Equals(baseState+2));
      AssertThat(bState->matchType, Equals(NFA::Character));
      AssertThat(bState->matchData.c.c[0], Equals('b'));
      AssertThat(bState->out1, Equals((void*)0));
      NFA::State *cState = bState->out;
      AssertThat(cState, Is().Not().EqualTo((void*)0));
      AssertThat(cState, Equals(baseState+3));
      AssertThat(cState->matchType, Equals(NFA::Character));
      AssertThat(cState->matchData.c.c[0], Equals('c'));
      AssertThat(cState->out1, Equals((void*)0));
      NFA::State *matchState = aState->out;
      AssertThat(matchState, Is().Not().EqualTo((void*)0));
      AssertThat(matchState, Equals(cState->out));
      AssertThat(matchState, Equals(baseState+5));
      AssertThat(matchState->matchType, Equals(NFA::Token));
      AssertThat(matchState->matchData.t, Equals(2)); // token:1 ignore:false
      AssertThat(matchState->out, Equals((void*)0));
      AssertThat(matchState->out1, Equals((void*)0));
    });

    /// We build the NFA assocaited with a simple linear regular
    /// expression full of excape characters to show that our escaping
    /// mechanism is working correctly.
    it("Should build NFA for a regular expression with escape characters", [&](){
      Classifier *classifier = new Classifier();
      NFA *nfa = new NFA(classifier);
      NFABuilder *nfaBuilder = new NFABuilder(nfa);
      nfaBuilder->compileRegularExpressionForTokenId("start", "\\s\\(\\)\\|\\*\\+\\?\\\\", 1);
      NFA::State *anNFAState = nfa->getStartState("start");
      AssertThat(nfa->getNumberStates(), Is().EqualTo(10));
      AssertThat(anNFAState, Is().EqualTo(nfa->startState.getItem(0, NULL)));
//      AssertThat(anNFAState, Is().EqualTo(nfa->nfaLastStartState));
      NFA::State *baseState =
        (NFA::State*)nfa->stateAllocator->blocks.getTop();
      AssertThat(anNFAState, Is().EqualTo(baseState));
      AssertThat(anNFAState->matchType, Is().EqualTo(NFA::Split));
      AssertThat(anNFAState->matchData.c.u, Is().EqualTo((uint64_t)0));
      AssertThat(anNFAState->out1, Is().EqualTo((NFA::State*)0));
      NFA::State *nextState = anNFAState->out;
      AssertThat(nextState, Is().EqualTo(baseState+1));
      AssertThat(nextState->matchType, Is().EqualTo(NFA::Character));
      utf8Char_t expectedChar;
      expectedChar.u = 0;
      expectedChar.c[0] = 's';
      AssertThat(nextState->matchData.c.u, Is().EqualTo(expectedChar.u));
      AssertThat(nextState->out1, Is().EqualTo((NFA::State*)0));
      nextState = nextState->out;
      AssertThat(nextState, Is().EqualTo(baseState+2));
      AssertThat(nextState->matchType, Is().EqualTo(NFA::Character));
      expectedChar.c[0] = '(';
      AssertThat(nextState->matchData.c.u, Is().EqualTo(expectedChar.u));
      AssertThat(nextState->out1, Is().EqualTo((NFA::State*)0));
      nextState = nextState->out;
      AssertThat(nextState, Is().EqualTo(baseState+3));
      AssertThat(nextState->matchType, Is().EqualTo(NFA::Character));
      expectedChar.c[0] = ')';
      AssertThat(nextState->matchData.c.u, Is().EqualTo(expectedChar.u));
      AssertThat(nextState->out1, Is().EqualTo((NFA::State*)0));
      nextState = nextState->out;
      AssertThat(nextState, Is().EqualTo(baseState+4));
      AssertThat(nextState->matchType, Is().EqualTo(NFA::Character));
      expectedChar.c[0] = '|';
      AssertThat(nextState->matchData.c.u, Is().EqualTo(expectedChar.u));
      AssertThat(nextState->out1, Is().EqualTo((NFA::State*)0));
      nextState = nextState->out;
      AssertThat(nextState, Is().EqualTo(baseState+5));
      AssertThat(nextState->matchType, Is().EqualTo(NFA::Character));
      expectedChar.c[0] = '*';
      AssertThat(nextState->matchData.c.u, Is().EqualTo(expectedChar.u));
      AssertThat(nextState->out1, Is().EqualTo((NFA::State*)0));
      nextState = nextState->out;
      AssertThat(nextState, Is().EqualTo(baseState+6));
      AssertThat(nextState->matchType, Is().EqualTo(NFA::Character));
      expectedChar.c[0] = '+';
      AssertThat(nextState->matchData.c.u, Is().EqualTo(expectedChar.u));
      AssertThat(nextState->out1, Is().EqualTo((NFA::State*)0));
      nextState = nextState->out;
      AssertThat(nextState, Is().EqualTo(baseState+7));
      AssertThat(nextState->matchType, Is().EqualTo(NFA::Character));
      expectedChar.c[0] = '?';
      AssertThat(nextState->matchData.c.u, Is().EqualTo(expectedChar.u));
      AssertThat(nextState->out1, Is().EqualTo((NFA::State*)0));
      nextState = nextState->out;
      AssertThat(nextState, Is().EqualTo(baseState+8));
      AssertThat(nextState->matchType, Is().EqualTo(NFA::Character));
      expectedChar.c[0] = '\\';
      AssertThat(nextState->matchData.c.u, Is().EqualTo(expectedChar.u));
      AssertThat(nextState->out1, Is().EqualTo((NFA::State*)0));
      nextState = nextState->out;
      AssertThat(nextState, Is().EqualTo(baseState+9));
      AssertThat(nextState->matchType, Is().EqualTo(NFA::Token));
      AssertThat(nextState->matchData.t, Is().EqualTo(2)); // token:1 ignore:false
      AssertThat(nextState->out,  Is().EqualTo((NFA::State*)0));
      AssertThat(nextState->out1, Is().EqualTo((NFA::State*)0));
      delete nfa;
      delete classifier;
    });

    /// We need to verify that empty regular expressions
    /// are rejected by throwing a LexerException.
    it("Empty regular expressions should throw lexer exceptions", [&](){
      Classifier *classifier = new Classifier();
      NFA *nfa = new NFA(classifier);
      NFABuilder *nfaBuilder = new NFABuilder(nfa);
      try {
        nfaBuilder->compileRegularExpressionForTokenId("start", "\\", 1);
        AssertThat(false, Is().True());
      } catch (LexerException& e) {
        AssertThat(true, Is().True());
      }
      delete nfa;
      delete classifier;
    });

    /// We need to verify the malformed regular expressions,
    /// such as: /*/, throw LexerExpressions.
    it("Malformed regular expressions throws lexer exception", [&](){
      Classifier *classifier = new Classifier();
      NFA *nfa = new NFA(classifier);
      NFABuilder *nfaBuilder = new NFABuilder(nfa);
      try {
        nfaBuilder->compileRegularExpressionForTokenId("start", "*", 1);
        AssertThat(false, Is().True());
      } catch (LexerException& e) {
        AssertThat(true, Is().True());
      }
      delete nfa;
      delete classifier;
    });

    /// We need to verify that trailing \\ escapes (with no futher
    /// character to escape) are ignored. In this case the regular
    /// expression: /s\\/ does not throw any exceptions and has the
    /// correct states.
    it("Trailing \\ escapes are ignored", [&](){
      Classifier *classifier = new Classifier();
      NFA *nfa = new NFA(classifier);
      NFABuilder *nfaBuilder = new NFABuilder(nfa);
      try {
        nfaBuilder->compileRegularExpressionForTokenId("start", "s\\", 1);
        AssertThat(true, Is().True());
        NFA::State *anNFAState = nfa->getStartState("start");
        AssertThat(nfa->getNumberStates(), Is().EqualTo(3));
        AssertThat(anNFAState, Is().EqualTo(nfa->startState.getItem(0, NULL)));
//        AssertThat(anNFAState, Is().EqualTo(nfa->nfaLastStartState));
        NFA::State *baseState =
          (NFA::State*)nfa->stateAllocator->blocks.getTop();
        AssertThat(anNFAState, Is().EqualTo(baseState));
        AssertThat(anNFAState->matchType, Is().EqualTo(NFA::Split));
        AssertThat(anNFAState->matchData.c.u, Is().EqualTo((uint64_t)0));
        AssertThat(anNFAState->out1, Is().EqualTo((NFA::State*)0));
        NFA::State *nextState = anNFAState->out;
        AssertThat(nextState, Is().EqualTo(baseState+1));
        AssertThat(nextState->matchType, Is().EqualTo(NFA::Character));
        utf8Char_t expectedChar;
        expectedChar.u = 0;
        expectedChar.c[0] = 's';
        AssertThat(nextState->matchData.c.u, Is().EqualTo(expectedChar.u));
        AssertThat(nextState->out1, Is().EqualTo((NFA::State*)0));
        nextState = nextState->out;
        AssertThat(nextState, Is().EqualTo(baseState+2));
        AssertThat(nextState->matchType, Is().EqualTo(NFA::Token));
        AssertThat(nextState->matchData.t, Is().EqualTo(2)); // token:1 ignore:false
        AssertThat(nextState->out,  Is().EqualTo((NFA::State*)0));
        AssertThat(nextState->out1, Is().EqualTo((NFA::State*)0));
      } catch (LexerException& e) {
        AssertThat(false, Is().True());
      }
      delete nfa;
      delete classifier;
    });

    /// We need to ensure that we can correctly compile a regular
    /// expression, such as: /[!whitespace]+/, which contains
    /// our (non-standard) syntax to match UTF8 character classes.
    ///
    /// This NFA should contain an NFA::ClassSet whose matchData
    /// is the required classSet_t. Note that compilation process
    /// accesses the Classifier to find the classSet_t corresponding
    /// to the requested class name. Note also that class names
    /// which are *not* known to the Classifier are returned with
    /// the Classifier's default classSet_t.
    it("Compile regular expression with classification", [&](){
      Classifier *classifier = new Classifier();
      classifier->registerClassSet("whitespace",1);
      classifier->classifyUtf8CharsAs(Utf8Chars::whiteSpaceChars,"whitespace");
      NFA *nfa = new NFA(classifier);
      NFABuilder *nfaBuilder = new NFABuilder(nfa);
      nfaBuilder->compileRegularExpressionForTokenId("start", "[!whitespace]+", 1);
      NFA::State *anNFAState = nfa->getStartState("start");
      AssertThat(nfa->getNumberStates(), Is().EqualTo(4));
      AssertThat(anNFAState, Is().EqualTo(nfa->startState.getItem(0, NULL)));
//      AssertThat(anNFAState, Is().EqualTo(nfa->nfaLastStartState));
      NFA::State *baseState =
        (NFA::State*)nfa->stateAllocator->blocks.getTop();
      AssertThat(anNFAState, Is().EqualTo(baseState));
      AssertThat(anNFAState->matchType, Is().EqualTo(NFA::Split));
      AssertThat(anNFAState->out1, Is().EqualTo((NFA::State*)0));
      NFA::State *nextState = anNFAState->out;
      AssertThat(nextState, Is().EqualTo(baseState+1));
      AssertThat(nextState->matchType, Is().EqualTo(NFA::ClassSet));
      AssertThat(nextState->matchData.s, Is().EqualTo(~1L));
      AssertThat(nextState->out, Is().Not().EqualTo((NFA::State*)0));
      AssertThat(nextState->out1, Is().EqualTo((NFA::State*)0));
      nextState = nextState->out;
      AssertThat(nextState, Is().EqualTo(baseState+2));
      AssertThat(nextState->matchType, Is().EqualTo(NFA::Split));
      AssertThat(nextState->out, Is().EqualTo(baseState+1));
      AssertThat(nextState->out1, Is().Not().EqualTo((NFA::State*)0));
      nextState = nextState->out1;
      AssertThat(nextState, Is().EqualTo(baseState+3));
      AssertThat(nextState->matchType, Is().EqualTo(NFA::Token));
      AssertThat(nextState->matchData.t, Is().EqualTo(2)); // token:1 ignore:false
      AssertThat(nextState->out,  Is().EqualTo((NFA::State*)0));
      AssertThat(nextState->out1, Is().EqualTo((NFA::State*)0));
      delete nfa;
      delete classifier;
    });

    /// We need to ensure that we can correctly compile a regular
    /// expression, such as: /{start}+/, which contains
    /// our (non-standard) syntax to reStart the NFA.
    ///
    /// NOTE that strictly speaking this is a left recursive
    /// grammar... but we are not (yet) interpreting it...
    /// just looking at the resulting structure.
    it("Compile regular expression with reStartStates", [&](){
      Classifier *classifier = new Classifier();
      NFA *nfa = new NFA(classifier);
      NFABuilder *nfaBuilder = new NFABuilder(nfa);
      nfaBuilder->compileRegularExpressionForTokenId("start", "{start}+", 1);
      NFA::State *anNFAState = nfa->getStartState("start");
//      AssertThat(nfa->getNumberStates(), Is().EqualTo(4));
      AssertThat(anNFAState, Is().EqualTo(nfa->startState.getItem(0, NULL)));
      NFA::State *baseState =
        (NFA::State*)nfa->stateAllocator->blocks.getTop();
      AssertThat(anNFAState, Is().EqualTo(baseState));
      AssertThat(anNFAState->matchType, Is().EqualTo(NFA::Split));
      AssertThat(anNFAState->out1, Is().EqualTo((NFA::State*)0));
      NFA::State *nextState = anNFAState->out;
      AssertThat(nextState, Is().EqualTo(baseState+1));
      AssertThat(nextState->matchType, Is().EqualTo(NFA::ReStart));
      AssertThat(nextState->matchData.r, Is().EqualTo(0));
      AssertThat(nextState->out, Is().Not().EqualTo((NFA::State*)0));
      AssertThat(nextState->out1, Is().EqualTo((NFA::State*)0));
      nextState = nextState->out;
      AssertThat(nextState, Is().EqualTo(baseState+2));
      AssertThat(nextState->matchType, Is().EqualTo(NFA::Split));
      AssertThat(nextState->out, Is().EqualTo(baseState+1));
      AssertThat(nextState->out1, Is().Not().EqualTo((NFA::State*)0));
      nextState = nextState->out1;
      AssertThat(nextState, Is().EqualTo(baseState+3));
      AssertThat(nextState->matchType, Is().EqualTo(NFA::Token));
      AssertThat(nextState->matchData.t, Is().EqualTo(2)); //token:1 ignore:false
      AssertThat(nextState->out,  Is().EqualTo((NFA::State*)0));
      AssertThat(nextState->out1, Is().EqualTo((NFA::State*)0));
      delete nfa;
      delete classifier;
    });

    it("should be able to add multiple regular-expression/tokenIds", [&](){
      Classifier *classifier = new Classifier();
      classifier->registerClassSet("whitespace",1);
      classifier->classifyUtf8CharsAs(Utf8Chars::whiteSpaceChars,"whitespace");
      NFA *nfa = new NFA(classifier);
      NFABuilder *nfaBuilder = new NFABuilder(nfa);
      nfaBuilder->compileRegularExpressionForTokenId("start", "[whitespace]+", 1);
      AssertThat(nfa->getNumberStates(), Is().EqualTo(4));
      NFA::State *baseState =
       (NFA::State*)nfa->stateAllocator->blocks.getTop();
      //
      // check to ensure we have the regExp: /[whitespace]+/
      //
      NFA::State *nfaStartState0 = nfa->getStartState("start");
      AssertThat(nfaStartState0, Is().EqualTo(nfa->startState.getItem(0, NULL)));
//      AssertThat(nfaStartState0, Is().EqualTo(nfa->nfaLastStartState));
      AssertThat(nfaStartState0, Is().EqualTo(baseState));
      AssertThat(nfaStartState0->matchType, Is().EqualTo(NFA::Split));
      AssertThat(nfaStartState0->matchData.c.u, Is().EqualTo((uint64_t)0));
      AssertThat(nfaStartState0->out, Is().Not().EqualTo((void*)0));
      AssertThat(nfaStartState0->out1, Is().EqualTo((void*)0));
      NFA::State *nextState = nfaStartState0->out;
      AssertThat(nextState, Is().EqualTo(baseState+1));
      AssertThat(nextState->matchType, Is().EqualTo(NFA::ClassSet));
      AssertThat(nextState->matchData.s, Is().EqualTo(1L));
      AssertThat(nextState->out, Is().Not().EqualTo((void*)0));
      AssertThat(nextState->out1, Is().EqualTo((void*)0));
      nextState = nextState->out;
      AssertThat(nextState, Is().EqualTo(baseState+2));
      AssertThat(nextState->matchType, Is().EqualTo(NFA::Split));
      AssertThat(nextState->matchData.c.u, Is().EqualTo(0));
      AssertThat(nextState->out, Is().Not().EqualTo((void*)0));
      AssertThat(nextState->out, Is().EqualTo(baseState+1));
      AssertThat(nextState->out1, Is().Not().EqualTo((void*)0));
      nextState = nextState->out1;
      AssertThat(nextState, Is().EqualTo(baseState+3));
      AssertThat(nextState->matchType, Is().EqualTo(NFA::Token));
      AssertThat(nextState->matchData.c.u, Is().EqualTo(2)); //token:1 ignore:false
      AssertThat(nextState->out, Is().EqualTo((void*)0));
      AssertThat(nextState->out1, Is().EqualTo((void*)0));
      //
      // Now add the second regExp
      //
      nfaBuilder->compileRegularExpressionForTokenId("start", "[!whitespace]+", 2);
      AssertThat(nfa->getNumberStates(), Is().EqualTo(8));
      //
      // check to ensure that the FIRST branch is the regExp: /[whitespace]+/
      //
      AssertThat(nfaStartState0, Is().EqualTo(nfa->startState.getItem(0, NULL)));
//      AssertThat(nfaStartState0, Is().Not().EqualTo(nfa->nfaLastStartState));
      AssertThat(nfaStartState0, Is().EqualTo(baseState));
      AssertThat(nfaStartState0->matchType, Is().EqualTo(NFA::Split));
      AssertThat(nfaStartState0->matchData.c.u, Is().EqualTo((uint64_t)0));
      AssertThat(nfaStartState0->out, Is().Not().EqualTo((void*)0));
//      AssertThat(nfaStartState0->out1, Is().Not().EqualTo((void*)0));
  //    AssertThat(nfaStartState0->out1, Is().EqualTo(nfa->nfaLastStartState));
      nextState = nfaStartState0->out;
      AssertThat(nextState, Is().EqualTo(baseState+1));
      AssertThat(nextState->matchType, Is().EqualTo(NFA::ClassSet));
      AssertThat(nextState->matchData.s, Is().EqualTo(1L));
      AssertThat(nextState->out, Is().Not().EqualTo((void*)0));
      AssertThat(nextState->out1, Is().EqualTo((void*)0));
      nextState = nextState->out;
      AssertThat(nextState, Is().EqualTo(baseState+2));
      AssertThat(nextState->matchType, Is().EqualTo(NFA::Split));
      AssertThat(nextState->matchData.c.u, Is().EqualTo(0));
      AssertThat(nextState->out, Is().Not().EqualTo((void*)0));
      AssertThat(nextState->out, Is().EqualTo(baseState+1));
      AssertThat(nextState->out1, Is().Not().EqualTo((void*)0));
      nextState = nextState->out1;
      AssertThat(nextState, Is().EqualTo(baseState+3));
      AssertThat(nextState->matchType, Is().EqualTo(NFA::Token));
      AssertThat(nextState->matchData.c.u, Is().EqualTo(2)); //token:1 ignore:false
      AssertThat(nextState->out, Is().EqualTo((void*)0));
      AssertThat(nextState->out1, Is().EqualTo((void*)0));
      //
      // check to ensure that the SECOND branch is the regExp: /[!whitespace]+/
      //
      nextState = nfaStartState0->out1;
      AssertThat(nextState, Is().Not().EqualTo(nfa->startState.getItem(0, NULL)));
//      AssertThat(nextState, Is().EqualTo(nfa->nfaLastStartState));
      AssertThat(nextState, Is().EqualTo(baseState+4));
      AssertThat(nextState->matchType, Is().EqualTo(NFA::Split));
      AssertThat(nextState->matchData.c.u, Is().EqualTo((uint64_t)0));
      AssertThat(nextState->out, Is().Not().EqualTo((void*)0));
      AssertThat(nextState->out1, Is().EqualTo((void*)0));
      nextState = nextState->out;
      AssertThat(nextState, Is().EqualTo(baseState+5));
      AssertThat(nextState->matchType, Is().EqualTo(NFA::ClassSet));
      AssertThat(nextState->matchData.s, Is().EqualTo(~1L));
      AssertThat(nextState->out, Is().Not().EqualTo((void*)0));
      AssertThat(nextState->out1, Is().EqualTo((void*)0));
      nextState = nextState->out;
      AssertThat(nextState, Is().EqualTo(baseState+6));
      AssertThat(nextState->matchType, Is().EqualTo(NFA::Split));
      AssertThat(nextState->matchData.c.u, Is().EqualTo(0));
      AssertThat(nextState->out, Is().Not().EqualTo((void*)0));
      AssertThat(nextState->out, Is().EqualTo(baseState+5));
      AssertThat(nextState->out1, Is().Not().EqualTo((void*)0));
      nextState = nextState->out1;
      AssertThat(nextState, Is().EqualTo(baseState+7));
      AssertThat(nextState->matchType, Is().EqualTo(NFA::Token));
      AssertThat(nextState->matchData.c.u, Is().EqualTo(4)); //token:2 ignore:false
      AssertThat(nextState->out, Is().EqualTo((void*)0));
      AssertThat(nextState->out1, Is().EqualTo((void*)0));
      delete nfa;
      delete classifier;
    });

  }); // regularExpression2NFA

});
