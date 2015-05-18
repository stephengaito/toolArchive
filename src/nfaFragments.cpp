#include <stdio.h>

#include "nfaFragments.h"

NFAFragments::NFAFragments(NFA *anNFA, size_t reLength) {
  nfa = anNFA;
  stack = (Frag*) calloc(2*reLength, sizeof(Frag));
  stackPtr = stack;
  stackEnd = stack + (2*reLength);
  noMatchData.c.u = 0;
}

NFAFragments::~NFAFragments(void) {
  if (stack) free(stack);
  stack = NULL;
  stackPtr = NULL;
  stackEnd = NULL;
  noMatchData.c.u = 0;
}

NFAFragments::Frag NFAFragments::frag(NFA::State *start,
  NFAFragments::Ptrlist *out) {
  Frag n = { start, out };
  return n;
}

NFAFragments::Ptrlist* NFAFragments::list1(NFA::State **outp) {
  Ptrlist *l;
  l = (Ptrlist*)outp;
  l->next = NULL;
  return l;
}

void NFAFragments::patch(NFAFragments::Ptrlist *l,
                         NFA::State *s) {
  Ptrlist *next;
  for (; l; l=next) {
    next = l->next;
    l->s = s;
  }
}

NFAFragments::Ptrlist* NFAFragments::append(
  NFAFragments::Ptrlist *l1, NFAFragments::Ptrlist *l2) {
  Ptrlist *oldl1;
  oldl1 = l1;
  while (l1->next) l1 = l1->next;
  l1->next = l2;
  return oldl1;
}

void NFAFragments::push(NFAFragments::Frag aFrag)
  throw (LexerException) {
  if (stackEnd < stackPtr)
    throw LexerException("regular expression is too complex");
  *stackPtr = aFrag;
  stackPtr++;
};

NFAFragments::Frag NFAFragments::pop(void)
  throw (LexerException) {
  --stackPtr;
  if (stackPtr < stack)
    throw LexerException("mallformed regular expression missing atom");
  return *stackPtr;
};

void NFAFragments::checkCharacter(utf8Char_t aChar)
  throw (LexerException) {
  NFA::MatchData someMatchData;
  someMatchData.c = aChar;
  NFA::State *s = nfa->addState(NFA::Character, someMatchData, NULL, NULL);
  push(frag(s, list1(&s->out)));
}

void NFAFragments::checkClassification(classSet_t aClass)
  throw (LexerException) {
  NFA::MatchData someMatchData;
  someMatchData.s = aClass;
  NFA::State *s = nfa->addState(NFA::ClassSet, someMatchData, NULL, NULL);
  push(frag(s, list1(&s->out)));
}

void NFAFragments::concatenate(void)
  throw (LexerException) {
  Frag e2 = pop();
  Frag e1 = pop();
  patch(e1.out, e2.start);
  push(frag(e1.start, e2.out));
};

void NFAFragments::alternate(void)
  throw (LexerException) {
  Frag e2 = pop();
  Frag e1 = pop();
  NFA::State *s = nfa->addState(NFA::Split, noMatchData, e1.start, e2.start);
  push(frag(s, append(e1.out, e2.out)));
};

void NFAFragments::zeroOrOne(void)
  throw (LexerException) {
  Frag   e = pop();
  NFA::State *s = nfa->addState(NFA::Split, noMatchData, e.start, NULL);
  push(frag(s, append(e.out, list1(&s->out1))));
};

void NFAFragments::zeroOrMore(void)
  throw (LexerException) {
  Frag   e = pop();
  NFA::State *s = nfa->addState(NFA::Split, noMatchData, e.start, NULL);
  patch(e.out, s);
  push(frag(s, list1(&s->out1)));
};

void NFAFragments::oneOrMore(void)
  throw (LexerException) {
  Frag   e = pop();
  NFA::State *s = nfa->addState(NFA::Split, noMatchData, e.start, NULL);
  patch(e.out, s);
  push(frag(e.start, list1(&s->out1)));
};

NFA::State *NFAFragments::match(NFA::token_t aTokenId)
  throw (LexerException) {
  Frag e = pop();
  NFA::MatchData tokenData;
  tokenData.c.u = 0;
  tokenData.t = aTokenId;
  patch(e.out, nfa->addState(NFA::Token, tokenData, NULL, NULL));
  return e.start;
};

