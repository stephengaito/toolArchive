#include <stdio.h>

#include "nfaBuilder.h"

NFABuilder::NFABuilder(NFA *anNFA) {
  nfa             = anNFA;
  noMatchData.c.u = 0;
}

NFABuilder::~NFABuilder(void) {
  nfa             = NULL;
  noMatchData.c.u = 0;
}

NFABuilder::Frag NFABuilder::frag(NFA::State *start,
  NFABuilder::Ptrlist *out) {
  Frag n = { start, out };
  return n;
}

NFABuilder::Ptrlist* NFABuilder::list1(NFA::State **outp) {
  Ptrlist *l;
  l = (Ptrlist*)outp;
  l->next = NULL;
  return l;
}

void NFABuilder::patch(NFABuilder::Ptrlist *l,
                         NFA::State *s) {
  Ptrlist *next;
  for (; l; l=next) {
    next = l->next;
    l->s = s;
  }
}

NFABuilder::Ptrlist* NFABuilder::append(
  NFABuilder::Ptrlist *l1, NFABuilder::Ptrlist *l2) {
  Ptrlist *oldl1;
  oldl1 = l1;
  while (l1->next) l1 = l1->next;
  l1->next = l2;
  return oldl1;
}

void NFABuilder::checkCharacter(utf8Char_t aChar) {
  merge3(message, "checkCharacter[", aChar.c, "]");
  NFA::MatchData someMatchData;
  someMatchData.c = aChar;
  NFA::State *s =
    nfa->addState(NFA::Character, someMatchData, NULL, NULL, message);
  push(frag(s, list1(&s->out)));
}

void NFABuilder::checkClassification(Classifier::classSet_t aClass,
                                     const char *className) {
  merge3(message, "checkClass[", className, "]");
  NFA::MatchData someMatchData;
  someMatchData.s = aClass;
  NFA::State *s =
    nfa->addState(NFA::ClassSet, someMatchData, NULL, NULL, message);
  push(frag(s, list1(&s->out)));
}

void NFABuilder::reStart(NFA::StartStateId pushDownStartStateId,
                         const char *reStartStateName) {
  merge3(message, "reStart{", reStartStateName, "}");
  NFA::MatchData someMatchData;
  someMatchData.r = pushDownStartStateId;
  NFA::State *s =
    nfa->addState(NFA::ReStart, someMatchData, NULL, NULL, message);
  push(frag(s, list1(&s->out)));
}

void NFABuilder::concatenate(void) {
  Frag e2 = pop();
  Frag e1 = pop();
  patch(e1.out, e2.start);
  push(frag(e1.start, e2.out));
};

void NFABuilder::alternate(void) {
  Frag e2 = pop();
  Frag e1 = pop();
  NFA::State *s =
    nfa->addState(NFA::Split, noMatchData, e1.start, e2.start, "alternate");
  push(frag(s, append(e1.out, e2.out)));
};

void NFABuilder::zeroOrOne(void) {
  Frag   e = pop();
  NFA::State *s =
    nfa->addState(NFA::Split, noMatchData, e.start, NULL, "zeroOrOne");
  push(frag(s, append(e.out, list1(&s->out1))));
};

void NFABuilder::zeroOrMore(void) {
  Frag   e = pop();
  NFA::State *s =
    nfa->addState(NFA::Split, noMatchData, e.start, NULL, "zeroOrMore");
  patch(e.out, s);
  push(frag(s, list1(&s->out1)));
};

void NFABuilder::oneOrMore(void) {
  Frag   e = pop();
  NFA::State *s =
    nfa->addState(NFA::Split, noMatchData, e.start, NULL, "oneOrMore");
  patch(e.out, s);
  push(frag(e.start, list1(&s->out1)));
};

NFA::State *NFABuilder::match(ParseTrees::TokenId aTokenId,
                              const char *startStateName,
                              bool ignoreToken) {
  merge3(message, "match[", startStateName, "]");
  Frag e = pop();
  NFA::MatchData tokenData;
  tokenData.c.u = 0;
  tokenData.t = ParseTrees::wrapToken(aTokenId, ignoreToken);
  patch(e.out,
    nfa->addState(NFA::Token, tokenData, NULL, NULL, message));
  return e.start;
};

