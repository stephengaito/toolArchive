/*
 * Regular expression implementation.
 * Supports only ( | ) * + ?.  No escapes.
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
 * Extensive modifications for use as a utf8 lexer compiled by clang
 * are
 *   Copyright (c) 2015 Stephen Gaito
 *
 * Can be distributed under the MIT license, see bottom of file.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "lexer.h"

Lexer::Lexer(void) {
  nfaStates = NULL;
  curNFAState  = NULL;
  lastNFAState = NULL;
  curNFAStateVector  = -1;
  numNFAStateVectors = 0;
}

void Lexer::preAddNFAStates(size_t reLength) {
  curNFAStateVector++;
  // Ensure there are enough NFA State vectors for this additional collection
  if (numNFAStateVectors <= curNFAStateVector) {
    NFAState **oldNFAStates = nfaStates;
    nfaStates = (NFAState**) calloc(numNFAStateVectors + 10, sizeof(NFAState*));
    if (oldNFAStates) {
      memcpy(nfaStates, oldNFAStates, numNFAStateVectors);
    }
    numNFAStateVectors += 10;
  }

  nfaStates[curNFAStateVector] = (NFAState*) calloc(2*reLength, sizeof(NFAState));
  curNFAState = nfaStates[curNFAStateVector];
  lastNFAState = curNFAState + 2*reLength;
  curNFAState--;
}

Lexer::NFAState *Lexer::addNFAState(Lexer::MatchType aMatchType,
                                    Lexer::MatchData someMatchData,
                                    Lexer::NFAState *out,
                                    Lexer::NFAState *out1)
  throw (LexerException*) {
  if (lastNFAState < curNFAState) throw new LexerException("run out of NFA states");
  curNFAState++;
  curNFAState->matchType = aMatchType;
  curNFAState->matchData = someMatchData;
  curNFAState->out       = out;
  curNFAState->out1      = out1;
  return curNFAState;
}

/*
 * Convert infix regexp re to postfix notation.
 * Insert . as explicit concatenation operator.
 * Cheesy parser, return static buffer.
 */
Lexer::NFAState* Lexer::regularExpression2NFA(const char *aUtf8RegExp)
  throw (LexerException*) {

  size_t reLen = strlen(aUtf8RegExp);
  preAddNFAStates(reLen);
  Utf8Chars *re = new Utf8Chars(aUtf8RegExp);
  int nalt, natom;
  NFAFragments *fragments = new NFAFragments(this, reLen);
  struct {
    int nalt;
    int natom;
  } paren[reLen], *p;

  p = paren;
  nalt = 0;
  natom = 0;
  utf8Char_t curChar = re->nextUtf8Char();
  while (curChar.u) {
    switch (curChar.c[0]) {
      case '(':
        if (natom > 1) {
          --natom;
          fragments->concatenate();
        }
        if (p >= paren+reLen)
          throw new LexerException("parentheses too deep");
        p->nalt = nalt;
        p->natom = natom;
        p++;
        nalt = 0;
        natom = 0;
        break;
      case '|':
        if (natom == 0)
          throw new LexerException("no previous atom found in alternation");
        while (--natom > 0) fragments->concatenate();
        nalt++;
        break;
      case ')':
        if (p == paren) throw new LexerException("mismatched parentheses");
        if (natom == 0) throw new LexerException("no previous atom found before closing paranthesis");
        while (--natom > 0) fragments->concatenate();
        for (; nalt > 0; nalt--) fragments->alternate();
        --p;
        nalt = p->nalt;
        natom = p->natom;
        natom++;
        break;
      case '*':
        if (natom == 0) throw new LexerException("no previous atom found for zero or more");
        fragments->zeroOrMore();
        break;
      case '+':
        if (natom == 0) throw new LexerException("no previous atom found for one or more");
        fragments->oneOrMore();
        break;
      case '?':
        if (natom == 0) throw new LexerException("no previous atom found for zero or one");
        fragments->zeroOrOne();
        break;
      default:
        if (natom > 1) {
          --natom;
          fragments->concatenate();
        }
        fragments->checkCharacter(curChar);
        natom++;
        break;
      }
      curChar = re->nextUtf8Char();
    }
  if (p != paren) throw new LexerException("mismatched parentheses");
  while (--natom > 0) fragments->concatenate();
  for (; nalt > 0; nalt--) fragments->alternate();
  return fragments->match();
}

#ifdef NOT_DEFINED

/* Allocate and initialize State */
State* state(int c, State *out, State *out1) {
  State *s;

  nstate++;
  s = malloc(sizeof *s);
  s->lastlist = 0;
  s->c = c;
  s->out = out;
  s->out1 = out1;
  return s;
}

typedef struct List List;
struct List
{
	State **s;
	int n;
};
List l1, l2;
static int listid;

void addstate(List*, State*);
void step(List*, int, List*);

/* Compute initial state list */
List*
startlist(State *start, List *l)
{
	l->n = 0;
	listid++;
	addstate(l, start);
	return l;
}

/* Check whether state list contains a match. */
int
ismatch(List *l)
{
	int i;

	for(i=0; i<l->n; i++)
		if(l->s[i] == &matchstate)
			return 1;
	return 0;
}

/* Add s to l, following unlabeled arrows. */
void
addstate(List *l, State *s)
{
	if(s == NULL || s->lastlist == listid)
		return;
	s->lastlist = listid;
	if(s->c == Split){
		/* follow unlabeled arrows */
		addstate(l, s->out);
		addstate(l, s->out1);
		return;
	}
	l->s[l->n++] = s;
}

/*
 * Step the NFA from the states in clist
 * past the character c,
 * to create next NFA state set nlist.
 */
void
step(List *clist, int c, List *nlist)
{
	int i;
	State *s;

	listid++;
	nlist->n = 0;
	for(i=0; i<clist->n; i++){
		s = clist->s[i];
		if(s->c == c)
			addstate(nlist, s->out);
	}
}

/*
 * Represents a DFA state: a cached NFA state list.
 */
typedef struct DState DState;
struct DState
{
	List l;
	DState *next[256];
	DState *left;
	DState *right;
};

/* Compare lists: first by length, then by members. */
static int
listcmp(List *l1, List *l2)
{
	int i;

	if(l1->n < l2->n)
		return -1;
	if(l1->n > l2->n)
		return 1;
	for(i=0; i<l1->n; i++)
		if(l1->s[i] < l2->s[i])
			return -1;
		else if(l1->s[i] > l2->s[i])
			return 1;
	return 0;
}

/* Compare pointers by address. */
static int
ptrcmp(const void *a, const void *b)
{
	if(a < b)
		return -1;
	if(a > b)
		return 1;
	return 0;
}

DState *freelist;

/* Allocate DStates from a cached list. */
DState*
allocdstate(void)
{
	DState *d;
	
	if((d = freelist) != NULL)
		freelist = d->left;
	else{
		d = malloc(sizeof *d + nstate*sizeof(State*));
		d->l.s = (State**)(d+1);
	}
	d->left = NULL;
	d->right = NULL;
	memset(d->next, 0, sizeof d->next);
	return d;
}

/* Free the tree of states rooted at d. */
void
freestates(DState *d)
{
	if(d == NULL)
		return;
	freestates(d->left);
	freestates(d->right);
	d->left = freelist;
	freelist = d;
}

static DState *alldstates;
static int nstates;

/* Throw away the cache and start over. */
void
freecache(void)
{
	freestates(alldstates);
	alldstates = NULL;
	nstates = 0;
}

/*
 * Return the cached DState for list l,
 * creating a new one if needed.
 */
int maxstates = 32;
DState*
dstate(List *l, DState **nextp)
{
	int i;
	DState **dp, *d;

	qsort(l->s, l->n, sizeof l->s[0], ptrcmp);
	dp = &alldstates;
	while((d = *dp) != NULL){
		i = listcmp(l, &d->l);
		if(i < 0)
			dp = &d->left;
		else if(i > 0)
			dp = &d->right;
		else
			return d;
	}

	if(nstates >= maxstates){
		freecache();
		dp = &alldstates;
		nextp = NULL;
	}
	
	d = allocdstate();
	memmove(d->l.s, l->s, l->n*sizeof l->s[0]);
	d->l.n = l->n;
	*dp = d;
	nstates++;
	if(nextp != NULL)
		*nextp = d;
	return d;
}

void
startnfa(State *start, List *l)
{
	l->n = 0;
	listid++;
	addstate(l, start);
}

DState*
startdstate(State *start)
{
	return dstate(startlist(start, &l1), NULL);
}

DState*
nextstate(DState *d, int c)
{
	step(&d->l, c, &l1);
	return dstate(&l1, &d->next[c]);
}

/* Run DFA to determine whether it matches s. */
int
match(DState *start, char *s)
{
	DState *d, *next;
	int c, i;
	
	d = start;
	for(; *s; s++){
		c = *s & 0xFF;
		if((next = d->next[c]) == NULL)
			next = nextstate(d, c);
		d = next;
	}
	return ismatch(&d->l);
}

int
main(int argc, char **argv)
{
	int i;
	char *post;
	State *start;

	if(argc < 3){
		fprintf(stderr, "usage: nfa regexp string...\n");
		return 1;
	}
	
	post = re2post(argv[1]);
	if(post == NULL){
		fprintf(stderr, "bad regexp %s\n", argv[1]);
		return 1;
	}

	start = post2nfa(post);
	if(start == NULL){
		fprintf(stderr, "error in post2nfa %s\n", post);
		return 1;
	}
	
	l1.s = malloc(nstate*sizeof l1.s[0]);
	l2.s = malloc(nstate*sizeof l2.s[0]);
	for(i=2; i<argc; i++)
		if(match(startdstate(start), argv[i]))
			printf("%s\n", argv[i]);
	return 0;
}
#endif

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

