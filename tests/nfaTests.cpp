#include <bandit/bandit.h>
using namespace bandit;

#include <string.h>
#include <stdio.h>

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
      NFA *nfa = new NFA();
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

  }); // regularExpression2NFA

});
