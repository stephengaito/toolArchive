#include <string.h>
#include <stdio.h>
#include <exception>

#include <cUtils/specs/specs.h>

#ifndef protected
#define protected public
#endif

// The following macro is used by the NFA's initialization of the
// BlockAllocator.  For (most) of our testing this number MUST be larger
// than the largets number of states in any of the NFA's being tested
// so that we can have one continuous baseStates.
//
#define NUM_NFA_STATES_PER_BLOCK 20

#include <dynUtf8Parser/nfaBuilder.h>

/// \brief We test the correctness of the Nondeterministic Finite
/// Automata associated with a given Regular Expression.
///
/// NOTE: These tests are essentially only testing one function
/// which has a complex end result, by walking over the resulting
/// NFA structure. This leads to dense and fragile tests.
///
/// Since these NFAs will be used as part of a Parser with multiple
/// tokens to be recognized, the NFA start state(s) consist of
/// what is essentially a linked list of NFA::Split states whose
/// out pointer points to the (sub)NFA to recognize a given token
/// and whose out1 pointer points to the next (sub)NFA/token in
/// the linked list. Each (sub)NFA will end with an NFA::Token state
/// whose matchData stores the token ID associated with the recognized
/// token. All test hence must recognize both this first NFA::Split
/// and final NFA::Token states.
describe(NFA) {

  specSize(NFA::MatchType);
  specSize(NFA::MatchData);
  specSize(NFA::State*);
  specSize(NFA::State);

  it("should create NFA object with correct instance variables") {
    Classifier *classifier = new Classifier();
    shouldNotBeNULL(classifier);
    NFA *nfa = new NFA(classifier);
    shouldNotBeNULL(nfa->stateAllocator);
    shouldNotBeNULL(nfa->startStateIds);
    shouldBeZero(nfa->startState.getNumItems());
    shouldBeZero(nfa->numKnownStates);
    shouldBeEqual(nfa->utf8Classifier, classifier);
    delete nfa;
    delete classifier;
  } endIt();

  it("should be able to register lots of start states") {
    Classifier *classifier = new Classifier();
    shouldNotBeNULL(classifier);
    NFA *nfa = new NFA(classifier);
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
      nfa->appendNFAToStartState(buffer, states[i]);
      shouldBeEqual(nfa->getNumberStartStates(), (i+1));
      shouldBeEqual(nfa->findStartStateId(buffer), i);
      shouldBeEqual(nfa->getStartState(buffer), states[i]);
      shouldBeEqual(nfa->getStartState((NFA::StartStateId)i), states[i]);
    }
    delete nfa;
    delete classifier;
  } endIt();

  it("should be able to register lots of start states to the same start state") {
    Classifier *classifier = new Classifier();
    shouldNotBeNULL(classifier);
    NFA *nfa = new NFA(classifier);
    shouldNotBeNULL(nfa->startStateIds);
    shouldBeZero(nfa->startState.getNumItems());
    char buffer[100];
    char *startStateName = buffer;
    NFA::State *states[100];
    NFA::MatchData noMatchData;
    noMatchData.c.u = 0;
    for (size_t i = 0; i < 100; i++) {
      nfa->registerStartState("testStartState");
      states[i] = nfa->addState(NFA::Split, noMatchData, NULL, NULL, "test");
      nfa->appendNFAToStartState("testStartState", states[i]);
    }
    shouldBeEqual(nfa->getNumberStartStates(), 1);
    NFA::State *nextState = nfa->getStartState("testStartState");
    for (size_t i = 0; i < 100; i++) {
      shouldNotBeNULL(nextState);
      shouldBeEqual(nextState, states[i]);
      memset(buffer, 0, 100);
      sprintf(buffer, "testStartState[%zu]", i);
      startStateName = buffer;
      if (strcmp(nextState->message, startStateName) != 0) {
        shouldBeEqual(nextState->message, startStateName);
      }
      nextState = nextState->out1;
    }
     delete nfa;
     delete classifier;
  } endIt();

  /// We build the NFA associated with the very simple regular
  /// expression: /simple/. This yeilds a deep linear linked
  /// list of character comparisions started by a single
  /// split. Other than the last NFA::State (which is an NFA::Token
  /// state) all other states are simple NFA::Character states
  /// whose matchData is the character to be matched.
  it("Should build NFA for a simple regular expression") {
    Classifier *classifier = new Classifier();
    shouldNotBeNULL(classifier);
    NFA *nfa = new NFA(classifier);
    shouldNotBeNULL(nfa);
    NFABuilder *nfaBuilder = new NFABuilder(nfa);
    shouldNotBeNULL(nfaBuilder);
    nfaBuilder->compileRegularExpressionForTokenId("start", "simple", 1);
    NFA::State *anNFAState = nfa->getStartState("start");
    shouldBeEqual(nfa->getNumberStates(), 8);
    shouldBeEqual(anNFAState, nfa->startState.getItem(0, NULL));
    shouldBeEqual(nfa->getNumberStartStates(), 1);
    NFA::State *baseState =
      (NFA::State*)nfa->stateAllocator->blocks.getTop();
    shouldBeEqual(anNFAState, baseState);
    shouldBeEqual(anNFAState->matchType, NFA::Split);
    shouldBeEqual(anNFAState->matchData.c.u, (uint64_t)0);
    shouldBeNULL(anNFAState->out1);
    NFA::State *nextState = anNFAState->out;
    shouldBeEqual(nextState, baseState+1);
    shouldBeEqual(nextState->matchType, NFA::Character);
    utf8Char_t expectedChar;
    expectedChar.u = 0;
    expectedChar.c[0] = 's';
    shouldBeEqual(nextState->matchData.c.u, expectedChar.u);
    shouldBeNULL(nextState->out1);
    nextState = nextState->out;
    shouldBeEqual(nextState, baseState+2);
    shouldBeEqual(nextState->matchType, NFA::Character);
    expectedChar.c[0] = 'i';
    shouldBeEqual(nextState->matchData.c.u, expectedChar.u);
    shouldBeNULL(nextState->out1);
    nextState = nextState->out;
    shouldBeEqual(nextState, baseState+3);
    shouldBeEqual(nextState->matchType, NFA::Character);
    expectedChar.c[0] = 'm';
    shouldBeEqual(nextState->matchData.c.u, expectedChar.u);
    shouldBeNULL(nextState->out1);
    nextState = nextState->out;
    shouldBeEqual(nextState, baseState+4);
    shouldBeEqual(nextState->matchType, NFA::Character);
    expectedChar.c[0] = 'p';
    shouldBeEqual(nextState->matchData.c.u, expectedChar.u);
    shouldBeNULL(nextState->out1);
    nextState = nextState->out;
    shouldBeEqual(nextState, baseState+5);
    shouldBeEqual(nextState->matchType, NFA::Character);
    expectedChar.c[0] = 'l';
    shouldBeEqual(nextState->matchData.c.u, expectedChar.u);
    shouldBeNULL(nextState->out1);
    nextState = nextState->out;
    shouldBeEqual(nextState, baseState+6);
    shouldBeEqual(nextState->matchType, NFA::Character);
    expectedChar.c[0] = 'e';
    shouldBeEqual(nextState->matchData.c.u, expectedChar.u);
    shouldBeNULL(nextState->out1);
    nextState = nextState->out;
    shouldBeEqual(nextState, baseState+7);
    shouldBeEqual(nextState->matchType, NFA::Token);
    shouldBeEqual(nextState->matchData.t, 2); // token:1 ignore:false
    shouldBeNULL(nextState->out);
    shouldBeNULL(nextState->out1);
    delete nfaBuilder;
    delete nfa;
    delete classifier;
  } endIt();

  it("Should build NFA for a regular expression with alternatives") {
    Classifier *classifier = new Classifier();
    shouldNotBeNULL(classifier);
    NFA *nfa = new NFA(classifier);
    shouldNotBeNULL(nfa);
    NFABuilder *nfaBuilder = new NFABuilder(nfa);
    shouldNotBeNULL(nfaBuilder);
    nfaBuilder->compileRegularExpressionForTokenId("start", "(a|bc)", 1);
    shouldBeEqual(nfa->getNumberStates(), 6);
    NFA::State *startState = nfa->getStartState("start");
    shouldNotBeNULL(startState);
    NFA::State *baseState =
      (NFA::State*)nfa->stateAllocator->blocks.getTop();
    shouldBeEqual(startState, baseState);
    shouldBeEqual(startState->matchType, NFA::Split);
    shouldBeNULL(startState->out1);
    NFA::State *nextState = startState->out;
    shouldNotBeNULL(nextState);
    shouldBeEqual(nextState, baseState+4);
    shouldBeEqual(nextState->matchType, NFA::Split);
    NFA::State *aState = nextState->out;
    shouldNotBeNULL(aState);
    shouldBeEqual(aState, baseState+1);
    shouldBeEqual(aState->matchType, NFA::Character);
    shouldBeEqual(aState->matchData.c.c[0], 'a');
    shouldBeNULL(aState->out1);
    NFA::State *bState = nextState->out1;
    shouldNotBeNULL(bState);
    shouldBeEqual(bState, baseState+2);
    shouldBeEqual(bState->matchType, NFA::Character);
    shouldBeEqual(bState->matchData.c.c[0], 'b');
    shouldBeNULL(bState->out1);
    NFA::State *cState = bState->out;
    shouldNotBeNULL(cState);
    shouldBeEqual(cState, baseState+3);
    shouldBeEqual(cState->matchType, NFA::Character);
    shouldBeEqual(cState->matchData.c.c[0], 'c');
    shouldBeNULL(cState->out1);
    NFA::State *matchState = aState->out;
    shouldNotBeNULL(matchState);
    shouldBeEqual(matchState, cState->out);
    shouldBeEqual(matchState, baseState+5);
    shouldBeEqual(matchState->matchType, NFA::Token);
    shouldBeEqual(matchState->matchData.t, 2); // token:1 ignore:false
    shouldBeNULL(matchState->out);
    shouldBeNULL(matchState->out1);
    delete nfaBuilder;
    delete nfa;
    delete classifier;
  } endIt();

  /// We build the NFA assocaited with a simple linear regular
  /// expression full of excape characters to show that our escaping
  /// mechanism is working correctly.
  it("Should build NFA for a regular expression with escape characters") {
    Classifier *classifier = new Classifier();
    shouldNotBeNULL(classifier);
    NFA *nfa = new NFA(classifier);
    shouldNotBeNULL(nfa);
    NFABuilder *nfaBuilder = new NFABuilder(nfa);
    shouldNotBeNULL(nfaBuilder);
    nfaBuilder->compileRegularExpressionForTokenId("start", "\\s\\(\\)\\|\\*\\+\\?\\\\", 1);
    NFA::State *anNFAState = nfa->getStartState("start");
    shouldBeEqual(nfa->getNumberStates(), 10);
    shouldBeEqual(anNFAState, nfa->startState.getItem(0, NULL));
//    shouldBeEqual(anNFAState, nfa->nfaLastStartState);
    NFA::State *baseState =
      (NFA::State*)nfa->stateAllocator->blocks.getTop();
    shouldBeEqual(anNFAState, baseState);
    shouldBeEqual(anNFAState->matchType, NFA::Split);
    shouldBeEqual(anNFAState->matchData.c.u, (uint64_t)0);
    shouldBeNULL(anNFAState->out1);
    NFA::State *nextState = anNFAState->out;
    shouldBeEqual(nextState, baseState+1);
    shouldBeEqual(nextState->matchType, NFA::Character);
    utf8Char_t expectedChar;
    expectedChar.u = 0;
    expectedChar.c[0] = 's';
    shouldBeEqual(nextState->matchData.c.u, expectedChar.u);
    shouldBeNULL(nextState->out1);
    nextState = nextState->out;
    shouldBeEqual(nextState, baseState+2);
    shouldBeEqual(nextState->matchType, NFA::Character);
    expectedChar.c[0] = '(';
    shouldBeEqual(nextState->matchData.c.u, expectedChar.u);
    shouldBeNULL(nextState->out1);
    nextState = nextState->out;
    shouldBeEqual(nextState, baseState+3);
    shouldBeEqual(nextState->matchType, NFA::Character);
    expectedChar.c[0] = ')';
    shouldBeEqual(nextState->matchData.c.u, expectedChar.u);
    shouldBeNULL(nextState->out1);
    nextState = nextState->out;
    shouldBeEqual(nextState, baseState+4);
    shouldBeEqual(nextState->matchType, NFA::Character);
    expectedChar.c[0] = '|';
    shouldBeEqual(nextState->matchData.c.u, expectedChar.u);
    shouldBeNULL(nextState->out1);
    nextState = nextState->out;
    shouldBeEqual(nextState, baseState+5);
    shouldBeEqual(nextState->matchType, NFA::Character);
    expectedChar.c[0] = '*';
    shouldBeEqual(nextState->matchData.c.u, expectedChar.u);
    shouldBeNULL(nextState->out1);
    nextState = nextState->out;
    shouldBeEqual(nextState, baseState+6);
    shouldBeEqual(nextState->matchType, NFA::Character);
    expectedChar.c[0] = '+';
    shouldBeEqual(nextState->matchData.c.u, expectedChar.u);
    shouldBeNULL(nextState->out1);
    nextState = nextState->out;
    shouldBeEqual(nextState, baseState+7);
    shouldBeEqual(nextState->matchType, NFA::Character);
    expectedChar.c[0] = '?';
    shouldBeEqual(nextState->matchData.c.u, expectedChar.u);
    shouldBeNULL(nextState->out1);
    nextState = nextState->out;
    shouldBeEqual(nextState, baseState+8);
    shouldBeEqual(nextState->matchType, NFA::Character);
    expectedChar.c[0] = '\\';
    shouldBeEqual(nextState->matchData.c.u, expectedChar.u);
    shouldBeNULL(nextState->out1);
    nextState = nextState->out;
    shouldBeEqual(nextState, baseState+9);
    shouldBeEqual(nextState->matchType, NFA::Token);
    shouldBeEqual(nextState->matchData.t, 2); // token:1 ignore:false
    shouldBeNULL(nextState->out);
    shouldBeNULL(nextState->out1);
    delete nfaBuilder;
    delete nfa;
    delete classifier;
  } endIt();

  /// We need to verify that empty regular expressions
  /// are rejected by throwing a ParserException.
  it("Empty regular expressions should throw Parser exceptions") {
    Classifier *classifier = new Classifier();
    shouldNotBeNULL(classifier);
    NFA *nfa = new NFA(classifier);
    NFABuilder *nfaBuilder = new NFABuilder(nfa);
    try {
      nfaBuilder->compileRegularExpressionForTokenId("start", "\\", 1);
      shouldBeTrue(false);
    } catch (ParserException& e) { }
    delete nfaBuilder;
    delete nfa;
    delete classifier;
  } endIt();

  /// We need to verify the malformed regular expressions,
  /// such as: /*/, throw ParserExpressions.
  it("Malformed regular expressions throws Parser exception") {
    Classifier *classifier = new Classifier();
    shouldNotBeNULL(classifier);
    NFA *nfa = new NFA(classifier);
    NFABuilder *nfaBuilder = new NFABuilder(nfa);
    try {
      nfaBuilder->compileRegularExpressionForTokenId("start", "*", 1);
      shouldBeTrue(false);
    } catch (ParserException& e) { }
    delete nfaBuilder;
    delete nfa;
    delete classifier;
  } endIt();

  /// We need to verify that trailing \\ escapes (with no futher
  /// character to escape) are ignored. In this case the regular
  /// expression: /s\\/ does not throw any exceptions and has the
  /// correct states.
  it("Trailing \\ escapes are ignored") {
    Classifier *classifier = new Classifier();
    shouldNotBeNULL(classifier);
    NFA *nfa = new NFA(classifier);
    shouldNotBeNULL(nfa);
    NFABuilder *nfaBuilder = new NFABuilder(nfa);
    shouldNotBeNULL(nfaBuilder);
    try {
      nfaBuilder->compileRegularExpressionForTokenId("start", "s\\", 1);
      shouldBeTrue(true);
      NFA::State *anNFAState = nfa->getStartState("start");
      shouldBeEqual(nfa->getNumberStates(), 3);
      shouldBeEqual(anNFAState, nfa->startState.getItem(0, NULL));
//      shouldBeEqual(anNFAState, Is().EqualTo(nfa->nfaLastStartState));
      NFA::State *baseState =
        (NFA::State*)nfa->stateAllocator->blocks.getTop();
      shouldBeEqual(anNFAState, baseState);
      shouldBeEqual(anNFAState->matchType, NFA::Split);
      shouldBeEqual(anNFAState->matchData.c.u, (uint64_t)0);
      shouldBeNULL(anNFAState->out1);
      NFA::State *nextState = anNFAState->out;
      shouldBeEqual(nextState, baseState+1);
      shouldBeEqual(nextState->matchType, NFA::Character);
      utf8Char_t expectedChar;
      expectedChar.u = 0;
      expectedChar.c[0] = 's';
      shouldBeEqual(nextState->matchData.c.u, expectedChar.u);
      shouldBeNULL(nextState->out1);
      nextState = nextState->out;
      shouldBeEqual(nextState, baseState+2);
      shouldBeEqual(nextState->matchType, NFA::Token);
      shouldBeEqual(nextState->matchData.t, 2); // token:1 ignore:false
      shouldBeNULL(nextState->out);
      shouldBeNULL(nextState->out1);
    } catch (ParserException& e) {
      shouldBeTrue(false);
    }
    delete nfaBuilder;
    delete nfa;
    delete classifier;
  } endIt();

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
  it("Compile regular expression with classification") {
    Classifier *classifier = new Classifier();
    shouldNotBeNULL(classifier);
    classifier->registerClassSet("whitespace",1);
    classifier->classifyUtf8CharsAs(Utf8Chars::whiteSpaceChars,"whitespace");
    NFA *nfa = new NFA(classifier);
    shouldNotBeNULL(nfa);
    NFABuilder *nfaBuilder = new NFABuilder(nfa);
    shouldNotBeNULL(nfaBuilder);
    nfaBuilder->compileRegularExpressionForTokenId("start", "[!whitespace]+", 1);
    NFA::State *anNFAState = nfa->getStartState("start");
    shouldBeEqual(nfa->getNumberStates(), 4);
    shouldBeEqual(anNFAState, nfa->startState.getItem(0, NULL));
//    shouldBeEqual(anNFAState, nfa->nfaLastStartState);
    NFA::State *baseState =
      (NFA::State*)nfa->stateAllocator->blocks.getTop();
    shouldBeEqual(anNFAState, baseState);
    shouldBeEqual(anNFAState->matchType, NFA::Split);
    shouldBeNULL(anNFAState->out1);
    NFA::State *nextState = anNFAState->out;
    shouldBeEqual(nextState, baseState+1);
    shouldBeEqual(nextState->matchType, NFA::ClassSet);
    shouldBeEqual(nextState->matchData.s, ~1L);
    shouldNotBeNULL(nextState->out);
    shouldBeNULL(nextState->out1);
    nextState = nextState->out;
    shouldBeEqual(nextState, baseState+2);
    shouldBeEqual(nextState->matchType, NFA::Split);
    shouldBeEqual(nextState->out, baseState+1);
    shouldNotBeNULL(nextState->out1);
    nextState = nextState->out1;
    shouldBeEqual(nextState, baseState+3);
    shouldBeEqual(nextState->matchType, NFA::Token);
    shouldBeEqual(nextState->matchData.t, 2); // token:1 ignore:false
    shouldBeNULL(nextState->out);
    shouldBeNULL(nextState->out1);
    delete nfaBuilder;
    delete nfa;
    delete classifier;
  } endIt();

  /// We need to ensure that we can correctly compile a regular
  /// expression, such as: /{start}+/, which contains
  /// our (non-standard) syntax to reStart the NFA.
  ///
  /// NOTE that strictly speaking this is a left recursive
  /// grammar... but we are not (yet) interpreting it...
  /// just looking at the resulting structure.
  it("Compile regular expression with reStartStates") {
    Classifier *classifier = new Classifier();
    shouldNotBeNULL(classifier);
    NFA *nfa = new NFA(classifier);
    shouldNotBeNULL(nfa);
    NFABuilder *nfaBuilder = new NFABuilder(nfa);
    shouldNotBeNULL(nfaBuilder);
    nfaBuilder->compileRegularExpressionForTokenId("start", "{start}+", 1);
    NFA::State *anNFAState = nfa->getStartState("start");
//    shouldBeEqual(nfa->getNumberStates(), 4);
    shouldBeEqual(anNFAState, nfa->startState.getItem(0, NULL));
    NFA::State *baseState =
      (NFA::State*)nfa->stateAllocator->blocks.getTop();
    shouldBeEqual(anNFAState, baseState);
    shouldBeEqual(anNFAState->matchType, NFA::Split);
    shouldBeNULL(anNFAState->out1);
    NFA::State *nextState = anNFAState->out;
    shouldBeEqual(nextState, baseState+1);
    shouldBeEqual(nextState->matchType, NFA::ReStart);
    shouldBeZero(nextState->matchData.r);
    shouldNotBeNULL(nextState->out);
    shouldBeNULL(nextState->out1);
    nextState = nextState->out;
    shouldBeEqual(nextState, baseState+2);
    shouldBeEqual(nextState->matchType, NFA::Split);
    shouldBeEqual(nextState->out, baseState+1);
    shouldNotBeNULL(nextState->out1);
    nextState = nextState->out1;
    shouldBeEqual(nextState, baseState+3);
    shouldBeEqual(nextState->matchType, NFA::Token);
    shouldBeEqual(nextState->matchData.t, 2); //token:1 ignore:false
    shouldBeNULL(nextState->out);
    shouldBeNULL(nextState->out1);
    delete nfaBuilder;
    delete nfa;
    delete classifier;
  } endIt();

  it("should be able to add multiple regular-expression/tokenIds") {
    Classifier *classifier = new Classifier();
    shouldNotBeNULL(classifier);
    classifier->registerClassSet("whitespace",1);
    classifier->classifyUtf8CharsAs(Utf8Chars::whiteSpaceChars,"whitespace");
    NFA *nfa = new NFA(classifier);
    shouldNotBeNULL(nfa);
    NFABuilder *nfaBuilder = new NFABuilder(nfa);
    shouldNotBeNULL(nfaBuilder);
    nfaBuilder->compileRegularExpressionForTokenId("start", "[whitespace]+", 1);
    shouldBeEqual(nfa->getNumberStates(), 4);
    NFA::State *baseState =
     (NFA::State*)nfa->stateAllocator->blocks.getTop();
    //
    // check to ensure we have the regExp: /[whitespace]+/
    //
    NFA::State *nfaStartState0 = nfa->getStartState("start");
    shouldBeEqual(nfaStartState0, nfa->startState.getItem(0, NULL));
//    shouldBeEqual(nfaStartState0, nfa->nfaLastStartState);
    shouldBeEqual(nfaStartState0, baseState);
    shouldBeEqual(nfaStartState0->matchType, NFA::Split);
    shouldBeEqual(nfaStartState0->matchData.c.u, (uint64_t)0);
    shouldNotBeNULL(nfaStartState0->out);
    shouldBeNULL(nfaStartState0->out1);
    NFA::State *nextState = nfaStartState0->out;
    shouldBeEqual(nextState, baseState+1);
    shouldBeEqual(nextState->matchType, NFA::ClassSet);
    shouldBeEqual(nextState->matchData.s, 1L);
    shouldNotBeNULL(nextState->out);
    shouldBeNULL(nextState->out1);
    nextState = nextState->out;
    shouldBeEqual(nextState, baseState+2);
    shouldBeEqual(nextState->matchType, NFA::Split);
    shouldBeEqual(nextState->matchData.c.u, 0);
    shouldNotBeNULL(nextState->out);
    shouldBeEqual(nextState->out, baseState+1);
    shouldNotBeNULL(nextState->out1);
    nextState = nextState->out1;
    shouldBeEqual(nextState, baseState+3);
    shouldBeEqual(nextState->matchType, NFA::Token);
    shouldBeEqual(nextState->matchData.c.u, 2); //token:1 ignore:false
    shouldBeNULL(nextState->out);
    shouldBeNULL(nextState->out1);
    //
    // Now add the second regExp
    //
    nfaBuilder->compileRegularExpressionForTokenId("start", "[!whitespace]+", 2);
    shouldBeEqual(nfa->getNumberStates(), 8);
    //
    // check to ensure that the FIRST branch is the regExp: /[whitespace]+/
    //
    shouldBeEqual(nfaStartState0, nfa->startState.getItem(0, NULL));
//    shouldNotBeEqual(nfaStartState0, nfa->nfaLastStartState);
    shouldBeEqual(nfaStartState0, baseState);
    shouldBeEqual(nfaStartState0->matchType, NFA::Split);
    shouldBeEqual(nfaStartState0->matchData.c.u, (uint64_t)0);
    shouldNotBeNULL(nfaStartState0->out);
//    shouldNotBeNULL(nfaStartState0->out1);
//    shouldBeEqual(nfaStartState0->out1, nfa->nfaLastStartState);
    nextState = nfaStartState0->out;
    shouldBeEqual(nextState, baseState+1);
    shouldBeEqual(nextState->matchType, NFA::ClassSet);
    shouldBeEqual(nextState->matchData.s, 1L);
    shouldNotBeNULL(nextState->out);
    shouldBeNULL(nextState->out1);
    nextState = nextState->out;
    shouldBeEqual(nextState, baseState+2);
    shouldBeEqual(nextState->matchType, NFA::Split);
    shouldBeEqual(nextState->matchData.c.u, 0);
    shouldNotBeNULL(nextState->out);
    shouldBeEqual(nextState->out, baseState+1);
    shouldNotBeNULL(nextState->out1);
    nextState = nextState->out1;
    shouldBeEqual(nextState, baseState+3);
    shouldBeEqual(nextState->matchType, NFA::Token);
    shouldBeEqual(nextState->matchData.c.u, 2); //token:1 ignore:false
    shouldBeNULL(nextState->out);
    shouldBeNULL(nextState->out1);
    //
    // check to ensure that the SECOND branch is the regExp: /[!whitespace]+/
    //
    nextState = nfaStartState0->out1;
    shouldNotBeEqual(nextState, nfa->startState.getItem(0, NULL));
//    shouldBeEqual(nextState, nfa->nfaLastStartState);
    shouldBeEqual(nextState, baseState+4);
    shouldBeEqual(nextState->matchType, NFA::Split);
    shouldBeEqual(nextState->matchData.c.u, (uint64_t)0);
    shouldNotBeNULL(nextState->out);
    shouldBeNULL(nextState->out1);
    nextState = nextState->out;
    shouldBeEqual(nextState, baseState+5);
    shouldBeEqual(nextState->matchType, NFA::ClassSet);
    shouldBeEqual(nextState->matchData.s, ~1L);
    shouldNotBeNULL(nextState->out);
    shouldBeNULL(nextState->out1);
    nextState = nextState->out;
    shouldBeEqual(nextState, baseState+6);
    shouldBeEqual(nextState->matchType, NFA::Split);
    shouldBeEqual(nextState->matchData.c.u, 0);
    shouldNotBeNULL(nextState->out);
    shouldBeEqual(nextState->out, baseState+5);
    shouldNotBeNULL(nextState->out1);
    nextState = nextState->out1;
    shouldBeEqual(nextState, baseState+7);
    shouldBeEqual(nextState->matchType, NFA::Token);
    shouldBeEqual(nextState->matchData.c.u, 4); //token:2 ignore:false
    shouldBeNULL(nextState->out);
    shouldBeNULL(nextState->out1);
    delete nfaBuilder;
    delete nfa;
    delete classifier;
  } endIt();

} endDescribe(NFA);


