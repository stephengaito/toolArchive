#include <stdio.h>

#include "lexer.h"

Lexer::NFAFragments::NFAFragments(Lexer *aLexer, size_t reLength) {
  lexer = aLexer;
  stack = (Frag*) calloc(2*reLength, sizeof(Frag));
  stackPtr = stack;
  stackEnd = stack + (2*reLength);
  noMatchData.c.u = 0;
}

Lexer::NFAFragments::Frag Lexer::NFAFragments::frag(Lexer::NFAState *start,
  Lexer::NFAFragments::Ptrlist *out) {
  Frag n = { start, out };
  return n;
}

Lexer::NFAFragments::Ptrlist* Lexer::NFAFragments::list1(Lexer::NFAState **outp) {
  Ptrlist *l;
  l = (Ptrlist*)outp;
  l->next = NULL;
  return l;
}

void Lexer::NFAFragments::patch(Lexer::NFAFragments::Ptrlist *l,
                                Lexer::NFAState *s) {
  Ptrlist *next;
  for (; l; l=next) {
    next = l->next;
    l->s = s;
  }
}

Lexer::NFAFragments::Ptrlist* Lexer::NFAFragments::append(
  Lexer::NFAFragments::Ptrlist *l1, Lexer::NFAFragments::Ptrlist *l2) {
  Ptrlist *oldl1;
  oldl1 = l1;
  while (l1->next) l1 = l1->next;
  l1->next = l2;
  return oldl1;
}

void Lexer::NFAFragments::push(Lexer::NFAFragments::Frag aFrag)
  throw (LexerException*) {
  if (stackEnd < stackPtr)
    throw LexerException("regular expression is too complex");
  *stackPtr = aFrag;
  stackPtr++;
};

Lexer::NFAFragments::Frag Lexer::NFAFragments::pop(void)
  throw (LexerException*) {
  --stackPtr;
  if (stackPtr < stack)
    throw LexerException("regular expression is too complex");
  return *stackPtr;
};

void Lexer::NFAFragments::checkCharacter(utf8Char_t aChar) {
  MatchData someMatchData;
  someMatchData.c = aChar;
  NFAState *s = lexer->addNFAState(Character, someMatchData, NULL, NULL);
  push(frag(s, list1(&s->out)));
}

void Lexer::NFAFragments::concatenate(void) {
  Frag e2 = pop();
  Frag e1 = pop();
  patch(e1.out, e2.start);
  push(frag(e1.start, e2.out));
};

void Lexer::NFAFragments::alternate(void) {
  Frag e2 = pop();
  Frag e1 = pop();
  NFAState *s = lexer->addNFAState(Split, noMatchData, e1.start, e2.start);
  push(frag(s, append(e1.out, e2.out)));
};

void Lexer::NFAFragments::zeroOrOne(void) {
  Frag   e = pop();
  NFAState *s = lexer->addNFAState(Split, noMatchData, e.start, NULL);
  push(frag(s, append(e.out, list1(&s->out1))));
};

void Lexer::NFAFragments::zeroOrMore(void) {
  Frag   e = pop();
  NFAState *s = lexer->addNFAState(Split, noMatchData, e.start, NULL);
  patch(e.out, s);
  push(frag(s, list1(&s->out1)));
};

void Lexer::NFAFragments::oneOrMore(void) {
  Frag   e = pop();
  NFAState *s = lexer->addNFAState(Split, noMatchData, e.start, NULL);
  patch(e.out, s);
  push(frag(e.start, list1(&s->out1)));
};

Lexer::NFAState *Lexer::NFAFragments::match(void) throw (LexerException*) {
  Frag e = pop();
  if (stackPtr != stack)
    throw new LexerException("Mismatched NFA fragment stack");
  MatchData tokenData = noMatchData;
  patch(e.out, lexer->addNFAState(Token, tokenData, NULL, NULL));
  return e.start;
};

