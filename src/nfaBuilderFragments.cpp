/*
 * Regular expression implementation.
 * Supports only ( | ) * + ?, as well as the local extensions:
 *
 *> [] to specify a UTF8 character class (negeated if the class name is
 *> prepended with an '!'.
 *>
 *> {} to specify the name of a rule to restart recognition in a push
 *> down machine.
 *>
 *> \ (or "\\" inside double quotes) are used to escape the next
 *> character.
 *
 * Any other character (including ' ' spaces) are assumed to be part of
 * the regular expression to be recognized.
 *
 * Compiles to NFA and then simulates NFA
 * using Thompson's algorithm.
 * Caches steps of Thompson's algorithm to
 * build DFA on the fly, as in Aho's egrep.
 *
 * See also http://swtch.com/~rsc/regexp/ and
 * Thompson, Ken.  Regular Expression Search Algorithm,
 * Communications of the ACM 11(6) (June 1968), pp. 419-422.
 *
 * Copyright (c) 2007 Russ Cox.
 *
 * Extensive modifications for use as a utf8 parser compiled by clang
 * are
 *   Copyright (c) 2015 Stephen Gaito
 *
 * Can be distributed under the MIT license, see bottom of file.
 */
#include <stdio.h>

#include "nfaBuilder.h"

#define merge3(bufferName, str0, str1, str2)                  \
  char bufferName[strlen(str0)+strlen(str1)+strlen(str2)+10]; \
  strcpy(bufferName, (str0));                                 \
  strcat(bufferName, (str1));                                 \
  strcat(bufferName, (str2));

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

NFA::State *NFABuilder::match(Token::TokenId aTokenId,
                              const char *startStateName,
                              bool ignoreToken) {
  merge3(message, "match[", startStateName, "]");
  Frag e = pop();
  NFA::MatchData tokenData;
  tokenData.c.u = 0;
  tokenData.t = Token::wrapTokenId(aTokenId, ignoreToken);
  patch(e.out,
    nfa->addState(NFA::Token, tokenData, NULL, NULL, message));
  return e.start;
};

/*
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the
 * Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall
 * be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
 * KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS
 * OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

