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

  describe("NFA::compileRegularExpression", [](){

    it("should build NFA for a simple regular expression", [&](){
      Classifier *classifier = new Classifier();
      NFA *nfa = new NFA(classifier);
      nfa->compileRegularExpression("simple");
      NFA::State *anNFAState = nfa->getNFAStartState();
      AssertThat(nfa->getNumberStates(), Is().EqualTo(7));
      NFA::State *baseState = nfa->states[nfa->curStateVector];
      AssertThat(anNFAState, Is().EqualTo(baseState));
      AssertThat(anNFAState->matchType, Is().EqualTo(NFA::Character));
      utf8Char_t expectedChar;
      expectedChar.u = 0;
      expectedChar.c[0] = 's';
      AssertThat(anNFAState->matchData.c.u, Is().EqualTo(expectedChar.u));
      AssertThat(anNFAState->out1, Is().EqualTo((NFA::State*)0));
      NFA::State *nextState = anNFAState->out;
      AssertThat(nextState, Is().EqualTo(baseState+1));
      AssertThat(nextState->matchType, Is().EqualTo(NFA::Character));
      expectedChar.c[0] = 'i';
      AssertThat(nextState->matchData.c.u, Is().EqualTo(expectedChar.u));
      AssertThat(nextState->out1, Is().EqualTo((NFA::State*)0));
      nextState = nextState->out;
      AssertThat(nextState, Is().EqualTo(baseState+2));
      AssertThat(nextState->matchType, Is().EqualTo(NFA::Character));
      expectedChar.c[0] = 'm';
      AssertThat(nextState->matchData.c.u, Is().EqualTo(expectedChar.u));
      AssertThat(nextState->out1, Is().EqualTo((NFA::State*)0));
      nextState = nextState->out;
      AssertThat(nextState, Is().EqualTo(baseState+3));
      AssertThat(nextState->matchType, Is().EqualTo(NFA::Character));
      expectedChar.c[0] = 'p';
      AssertThat(nextState->matchData.c.u, Is().EqualTo(expectedChar.u));
      AssertThat(nextState->out1, Is().EqualTo((NFA::State*)0));
      nextState = nextState->out;
      AssertThat(nextState, Is().EqualTo(baseState+4));
      AssertThat(nextState->matchType, Is().EqualTo(NFA::Character));
      expectedChar.c[0] = 'l';
      AssertThat(nextState->matchData.c.u, Is().EqualTo(expectedChar.u));
      AssertThat(nextState->out1, Is().EqualTo((NFA::State*)0));
      nextState = nextState->out;
      AssertThat(nextState, Is().EqualTo(baseState+5));
      AssertThat(nextState->matchType, Is().EqualTo(NFA::Character));
      expectedChar.c[0] = 'e';
      AssertThat(nextState->matchData.c.u, Is().EqualTo(expectedChar.u));
      AssertThat(nextState->out1, Is().EqualTo((NFA::State*)0));
      nextState = nextState->out;
      AssertThat(nextState, Is().EqualTo(baseState+6));
      AssertThat(nextState->matchType, Is().EqualTo(NFA::Token));
      AssertThat(nextState->matchData.t, Is().EqualTo(0));
      AssertThat(nextState->out,  Is().EqualTo((NFA::State*)0));
      AssertThat(nextState->out1, Is().EqualTo((NFA::State*)0));
    });

    it("should build NFA for a regular expression with escape characters", [&](){
      Classifier *classifier = new Classifier();
      NFA *nfa = new NFA(classifier);
      nfa->compileRegularExpression("\\s\\(\\)\\|\\*\\+\\?\\\\");
      NFA::State *anNFAState = nfa->getNFAStartState();
      AssertThat(nfa->getNumberStates(), Is().EqualTo(9));
      NFA::State *baseState = nfa->states[nfa->curStateVector];
      AssertThat(anNFAState, Is().EqualTo(baseState));
      AssertThat(anNFAState->matchType, Is().EqualTo(NFA::Character));
      utf8Char_t expectedChar;
      expectedChar.u = 0;
      expectedChar.c[0] = 's';
      AssertThat(anNFAState->matchData.c.u, Is().EqualTo(expectedChar.u));
      AssertThat(anNFAState->out1, Is().EqualTo((NFA::State*)0));
      NFA::State *nextState = anNFAState->out;
      AssertThat(nextState, Is().EqualTo(baseState+1));
      AssertThat(nextState->matchType, Is().EqualTo(NFA::Character));
      expectedChar.c[0] = '(';
      AssertThat(nextState->matchData.c.u, Is().EqualTo(expectedChar.u));
      AssertThat(nextState->out1, Is().EqualTo((NFA::State*)0));
      nextState = nextState->out;
      AssertThat(nextState, Is().EqualTo(baseState+2));
      AssertThat(nextState->matchType, Is().EqualTo(NFA::Character));
      expectedChar.c[0] = ')';
      AssertThat(nextState->matchData.c.u, Is().EqualTo(expectedChar.u));
      AssertThat(nextState->out1, Is().EqualTo((NFA::State*)0));
      nextState = nextState->out;
      AssertThat(nextState, Is().EqualTo(baseState+3));
      AssertThat(nextState->matchType, Is().EqualTo(NFA::Character));
      expectedChar.c[0] = '|';
      AssertThat(nextState->matchData.c.u, Is().EqualTo(expectedChar.u));
      AssertThat(nextState->out1, Is().EqualTo((NFA::State*)0));
      nextState = nextState->out;
      AssertThat(nextState, Is().EqualTo(baseState+4));
      AssertThat(nextState->matchType, Is().EqualTo(NFA::Character));
      expectedChar.c[0] = '*';
      AssertThat(nextState->matchData.c.u, Is().EqualTo(expectedChar.u));
      AssertThat(nextState->out1, Is().EqualTo((NFA::State*)0));
      nextState = nextState->out;
      AssertThat(nextState, Is().EqualTo(baseState+5));
      AssertThat(nextState->matchType, Is().EqualTo(NFA::Character));
      expectedChar.c[0] = '+';
      AssertThat(nextState->matchData.c.u, Is().EqualTo(expectedChar.u));
      AssertThat(nextState->out1, Is().EqualTo((NFA::State*)0));
      nextState = nextState->out;
      AssertThat(nextState, Is().EqualTo(baseState+6));
      AssertThat(nextState->matchType, Is().EqualTo(NFA::Character));
      expectedChar.c[0] = '?';
      AssertThat(nextState->matchData.c.u, Is().EqualTo(expectedChar.u));
      AssertThat(nextState->out1, Is().EqualTo((NFA::State*)0));
      nextState = nextState->out;
      AssertThat(nextState, Is().EqualTo(baseState+7));
      AssertThat(nextState->matchType, Is().EqualTo(NFA::Character));
      expectedChar.c[0] = '\\';
      AssertThat(nextState->matchData.c.u, Is().EqualTo(expectedChar.u));
      AssertThat(nextState->out1, Is().EqualTo((NFA::State*)0));
      nextState = nextState->out;
      AssertThat(nextState, Is().EqualTo(baseState+8));
      AssertThat(nextState->matchType, Is().EqualTo(NFA::Token));
      AssertThat(nextState->matchData.t, Is().EqualTo(0));
      AssertThat(nextState->out,  Is().EqualTo((NFA::State*)0));
      AssertThat(nextState->out1, Is().EqualTo((NFA::State*)0));
    });

    it("empty regular expression throws lexer exception", [&](){
      Classifier *classifier = new Classifier();
      NFA *nfa = new NFA(classifier);
      try {
        nfa->compileRegularExpression("\\");
        AssertThat(false, Is().True());
      } catch (LexerException& e) {
        AssertThat(true, Is().True());
      }
    });

    it("mallformed regular expressions throws lexer exception", [&](){
      Classifier *classifier = new Classifier();
      NFA *nfa = new NFA(classifier);
      try {
        nfa->compileRegularExpression("*");
        AssertThat(false, Is().True());
      } catch (LexerException& e) {
        AssertThat(true, Is().True());
      }
    });

    it("trailing \\ escapes are ignored", [&](){
      Classifier *classifier = new Classifier();
      NFA *nfa = new NFA(classifier);
      try {
        nfa->compileRegularExpression("s\\");
        AssertThat(true, Is().True());
      } catch (LexerException& e) {
        AssertThat(false, Is().True());
      }
    });

    it("compile regular expression with classification", [&](){
      Classifier *classifier = new Classifier();
      classifier->registerClassSet("whitespace",1);
      classifier->classifyUtf8CharsAs(Utf8Chars::whiteSpaceChars,"whitespace");
      NFA *nfa = new NFA(classifier);
      nfa->compileRegularExpression("[!whitespace]+");
      NFA::State *anNFAState = nfa->getNFAStartState();
      AssertThat(nfa->getNumberStates(), Is().EqualTo(3));
      NFA::State *baseState = nfa->states[nfa->curStateVector];
      AssertThat(anNFAState, Is().EqualTo(baseState));
      AssertThat(anNFAState->matchType, Is().EqualTo(NFA::ClassSet));
      AssertThat(anNFAState->matchData.s, Is().EqualTo(~1L));
      AssertThat(anNFAState->out1, Is().EqualTo((NFA::State*)0));
      NFA::State *nextState = anNFAState->out;
      AssertThat(nextState, Is().EqualTo(baseState+1));
      AssertThat(nextState->matchType, Is().EqualTo(NFA::Split));
      AssertThat(nextState->out, Is().EqualTo(anNFAState));
      AssertThat(nextState->out1, Is().Not().EqualTo((NFA::State*)0));
      nextState = nextState->out1;
      AssertThat(nextState, Is().EqualTo(baseState+2));
      AssertThat(nextState->matchType, Is().EqualTo(NFA::Token));
      AssertThat(nextState->matchData.t, Is().EqualTo(0));
      AssertThat(nextState->out,  Is().EqualTo((NFA::State*)0));
      AssertThat(nextState->out1, Is().EqualTo((NFA::State*)0));
    });

  }); // regularExpression2NFA

});
