#include <bandit/bandit.h>
using namespace bandit;

#include <string.h>
#include <stdio.h>
#include <exception>

#ifndef private
#define private public
#endif

#include <nfaFragments.h>

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

    /// We build the NFA associated with the very simple regular
    /// expression: /simple/. This yeilds a deep linear linked
    /// list of character comparisions started by a single
    /// split. Other than the last NFA::State (which is an NFA::Token
    /// state) all other states are simple NFA::Character states
    /// whose matchData is the character to be matched.
    it("Should build NFA for a simple regular expression", [&](){
      Classifier *classifier = new Classifier();
      NFA *nfa = new NFA(classifier);
      nfa->addRegularExpressionForToken("simple", 1);
      NFA::State *anNFAState = nfa->getNFAStartState();
      AssertThat(nfa->getNumberStates(), Is().EqualTo(8));
      AssertThat(anNFAState, Is().EqualTo(nfa->nfaStartState));
      AssertThat(anNFAState, Is().EqualTo(nfa->nfaLastStartState));
      NFA::State *baseState = nfa->states[nfa->curStateVector];
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
      AssertThat(nextState->matchData.t, Is().EqualTo(1));
      AssertThat(nextState->out,  Is().EqualTo((NFA::State*)0));
      AssertThat(nextState->out1, Is().EqualTo((NFA::State*)0));
    });

    /// We build the NFA assocaited with a simple linear regular
    /// expression full of excape characters to show that our escaping
    /// mechanism is working correctly.
    it("Should build NFA for a regular expression with escape characters", [&](){
      Classifier *classifier = new Classifier();
      NFA *nfa = new NFA(classifier);
      nfa->addRegularExpressionForToken("\\s\\(\\)\\|\\*\\+\\?\\\\", 1);
      NFA::State *anNFAState = nfa->getNFAStartState();
      AssertThat(nfa->getNumberStates(), Is().EqualTo(10));
      AssertThat(anNFAState, Is().EqualTo(nfa->nfaStartState));
      AssertThat(anNFAState, Is().EqualTo(nfa->nfaLastStartState));
      NFA::State *baseState = nfa->states[nfa->curStateVector];
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
      AssertThat(nextState->matchData.t, Is().EqualTo(1));
      AssertThat(nextState->out,  Is().EqualTo((NFA::State*)0));
      AssertThat(nextState->out1, Is().EqualTo((NFA::State*)0));
    });

    /// We need to verify that empty regular expressions
    /// are rejected by throwing a LexerException.
    it("Empty regular expressions should throw lexer exceptions", [&](){
      Classifier *classifier = new Classifier();
      NFA *nfa = new NFA(classifier);
      try {
        nfa->addRegularExpressionForToken("\\", 1);
        AssertThat(false, Is().True());
      } catch (LexerException& e) {
        AssertThat(true, Is().True());
      }
    });

    /// We need to verify the malformed regular expressions,
    /// such as: /*/, throw LexerExpressions.
    it("Malformed regular expressions throws lexer exception", [&](){
      Classifier *classifier = new Classifier();
      NFA *nfa = new NFA(classifier);
      try {
        nfa->addRegularExpressionForToken("*", 1);
        AssertThat(false, Is().True());
      } catch (LexerException& e) {
        AssertThat(true, Is().True());
      }
    });

    /// We need to verify that trailing \\ escapes (with no futher
    /// character to escape) are ignored. In this case the regular
    /// expression: /s\\/ does not throw any exceptions and has the
    /// correct states.
    it("Trailing \\ escapes are ignored", [&](){
      Classifier *classifier = new Classifier();
      NFA *nfa = new NFA(classifier);
      try {
        nfa->addRegularExpressionForToken("s\\", 1);
        AssertThat(true, Is().True());
        NFA::State *anNFAState = nfa->getNFAStartState();
        AssertThat(nfa->getNumberStates(), Is().EqualTo(3));
        AssertThat(anNFAState, Is().EqualTo(nfa->nfaStartState));
        AssertThat(anNFAState, Is().EqualTo(nfa->nfaLastStartState));
        NFA::State *baseState = nfa->states[nfa->curStateVector];
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
        AssertThat(nextState->matchData.t, Is().EqualTo(1));
        AssertThat(nextState->out,  Is().EqualTo((NFA::State*)0));
        AssertThat(nextState->out1, Is().EqualTo((NFA::State*)0));
      } catch (LexerException& e) {
        AssertThat(false, Is().True());
      }
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
      nfa->addRegularExpressionForToken("[!whitespace]+", 1);
      NFA::State *anNFAState = nfa->getNFAStartState();
      AssertThat(nfa->getNumberStates(), Is().EqualTo(4));
      AssertThat(anNFAState, Is().EqualTo(nfa->nfaStartState));
      AssertThat(anNFAState, Is().EqualTo(nfa->nfaLastStartState));
      NFA::State *baseState = nfa->states[nfa->curStateVector];
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
      AssertThat(nextState->matchData.t, Is().EqualTo(1));
      AssertThat(nextState->out,  Is().EqualTo((NFA::State*)0));
      AssertThat(nextState->out1, Is().EqualTo((NFA::State*)0));
    });

  }); // regularExpression2NFA

});
