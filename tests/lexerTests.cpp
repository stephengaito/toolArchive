#include <bandit/bandit.h>
using namespace bandit;

#include <string.h>
#include <stdio.h>

#ifndef private
#define private public
#endif

#include <lexer.h>

go_bandit([](){

  printf("\n----------------------------------\n");
  printf(  "lexer\n");
  printf(  "MatchType = %lu bytes (%lu bits)\n", sizeof(Lexer::MatchType), sizeof(Lexer::MatchType)*8);
  printf(  "MatchData = %lu bytes (%lu bits)\n", sizeof(Lexer::MatchData), sizeof(Lexer::MatchData)*8);
  printf(  "NFAState* = %lu bytes (%lu bits)\n", sizeof(Lexer::NFAState*), sizeof(Lexer::NFAState*)*8);
  printf(  " NFAState = %lu bytes (%lu bits)\n", sizeof(Lexer::NFAState),  sizeof(Lexer::NFAState)*8);
  printf(  "----------------------------------\n");

  describe("regularExpression2NFA", [](){

    it("should build NFA for a simple regular expression", [&](){
      Lexer *lexer = new Lexer();
      Lexer::NFAState *anNFA = lexer->regularExpression2NFA("simple");
      AssertThat(lexer->getNumberCurNFAStates(), Is().EqualTo(7));
      Lexer::NFAState *baseState = lexer->nfaStates[lexer->curNFAStateVector];
      AssertThat(anNFA, Is().EqualTo(baseState));
      AssertThat(anNFA->matchType, Is().EqualTo(Lexer::Character));
      utf8Char_t expectedChar;
      expectedChar.u = 0;
      expectedChar.c[0] = 's';
      AssertThat(anNFA->matchData.c.u, Is().EqualTo(expectedChar.u));
      AssertThat(anNFA->out1, Is().EqualTo((Lexer::NFAState*)0));
      Lexer::NFAState *nextState = anNFA->out;
      AssertThat(nextState, Is().EqualTo(baseState+1));
      AssertThat(nextState->matchType, Is().EqualTo(Lexer::Character));
      expectedChar.c[0] = 'i';
      AssertThat(nextState->matchData.c.u, Is().EqualTo(expectedChar.u));
      AssertThat(nextState->out1, Is().EqualTo((Lexer::NFAState*)0));
      nextState = nextState->out;
      AssertThat(nextState, Is().EqualTo(baseState+2));
      AssertThat(nextState->matchType, Is().EqualTo(Lexer::Character));
      expectedChar.c[0] = 'm';
      AssertThat(nextState->matchData.c.u, Is().EqualTo(expectedChar.u));
      AssertThat(nextState->out1, Is().EqualTo((Lexer::NFAState*)0));
      nextState = nextState->out;
      AssertThat(nextState, Is().EqualTo(baseState+3));
      AssertThat(nextState->matchType, Is().EqualTo(Lexer::Character));
      expectedChar.c[0] = 'p';
      AssertThat(nextState->matchData.c.u, Is().EqualTo(expectedChar.u));
      AssertThat(nextState->out1, Is().EqualTo((Lexer::NFAState*)0));
      nextState = nextState->out;
      AssertThat(nextState, Is().EqualTo(baseState+4));
      AssertThat(nextState->matchType, Is().EqualTo(Lexer::Character));
      expectedChar.c[0] = 'l';
      AssertThat(nextState->matchData.c.u, Is().EqualTo(expectedChar.u));
      AssertThat(nextState->out1, Is().EqualTo((Lexer::NFAState*)0));
      nextState = nextState->out;
      AssertThat(nextState, Is().EqualTo(baseState+5));
      AssertThat(nextState->matchType, Is().EqualTo(Lexer::Character));
      expectedChar.c[0] = 'e';
      AssertThat(nextState->matchData.c.u, Is().EqualTo(expectedChar.u));
      AssertThat(nextState->out1, Is().EqualTo((Lexer::NFAState*)0));
      nextState = nextState->out;
      AssertThat(nextState, Is().EqualTo(baseState+6));
      AssertThat(nextState->matchType, Is().EqualTo(Lexer::Token));
      AssertThat(nextState->matchData.t, Is().EqualTo(0));
      AssertThat(nextState->out,  Is().EqualTo((Lexer::NFAState*)0));
      AssertThat(nextState->out1, Is().EqualTo((Lexer::NFAState*)0));
    });

  }); // regularExpression2NFA

});
