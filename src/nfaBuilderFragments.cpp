#include <stdio.h>

#include "nfaBuilder.h"

#ifndef NFA_BUILDER_STACK_INCREMENT
#define NFA_BUILDER_STACK_INCREMENT 10
#endif

NFABuilder::NFABuilder(NFA *anNFA) {
  nfa             = anNFA;
  stack           = NULL;
  stackTop        = 0;
  stackSize       = 0;
  noMatchData.c.u = 0;
}

NFABuilder::~NFABuilder(void) {
  nfa             = NULL;
  if (stack) free(stack);
  stack           = NULL;
  stackTop        = 0;
  stackSize       = 0;
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

void NFABuilder::push(NFABuilder::Frag aFrag)
  throw (LexerException) {
  if (stackSize <= stackTop) {
    // we need to increase the size of the stack
    Frag *oldStack = stack;
    stack = (Frag*) calloc(stackSize+NFA_BUILDER_STACK_INCREMENT, sizeof(Frag));
    if (oldStack) {
      memcpy(stack, oldStack, stackSize);
      free(oldStack);
    }
    stackSize += NFA_BUILDER_STACK_INCREMENT;
  }
  stack[stackTop] = aFrag;
  stackTop++;
};

NFABuilder::Frag NFABuilder::pop(void)
  throw (LexerException) {
  if (0 < stackTop) {
    --stackTop;
  } else {
    throw LexerException("malformed regular expression missing atom");
  }
  return stack[stackTop];
};

void NFABuilder::checkCharacter(utf8Char_t aChar)
  throw (LexerException) {
  NFA::MatchData someMatchData;
  someMatchData.c = aChar;
  NFA::State *s = nfa->addState(NFA::Character, someMatchData, NULL, NULL);
  push(frag(s, list1(&s->out)));
}

void NFABuilder::checkClassification(Classifier::classSet_t aClass)
  throw (LexerException) {
  NFA::MatchData someMatchData;
  someMatchData.s = aClass;
  NFA::State *s = nfa->addState(NFA::ClassSet, someMatchData, NULL, NULL);
  push(frag(s, list1(&s->out)));
}

void NFABuilder::concatenate(void)
  throw (LexerException) {
  Frag e2 = pop();
  Frag e1 = pop();
  patch(e1.out, e2.start);
  push(frag(e1.start, e2.out));
};

void NFABuilder::alternate(void)
  throw (LexerException) {
  Frag e2 = pop();
  Frag e1 = pop();
  NFA::State *s = nfa->addState(NFA::Split, noMatchData, e1.start, e2.start);
  push(frag(s, append(e1.out, e2.out)));
};

void NFABuilder::zeroOrOne(void)
  throw (LexerException) {
  Frag   e = pop();
  NFA::State *s = nfa->addState(NFA::Split, noMatchData, e.start, NULL);
  push(frag(s, append(e.out, list1(&s->out1))));
};

void NFABuilder::zeroOrMore(void)
  throw (LexerException) {
  Frag   e = pop();
  NFA::State *s = nfa->addState(NFA::Split, noMatchData, e.start, NULL);
  patch(e.out, s);
  push(frag(s, list1(&s->out1)));
};

void NFABuilder::oneOrMore(void)
  throw (LexerException) {
  Frag   e = pop();
  NFA::State *s = nfa->addState(NFA::Split, noMatchData, e.start, NULL);
  patch(e.out, s);
  push(frag(e.start, list1(&s->out1)));
};

NFA::State *NFABuilder::match(NFA::TokenId aTokenId)
  throw (LexerException) {
  Frag e = pop();
  NFA::MatchData tokenData;
  tokenData.c.u = 0;
  tokenData.t = aTokenId;
  patch(e.out, nfa->addState(NFA::Token, tokenData, NULL, NULL));
  return e.start;
};

