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

#include "nfaFragments.h"

NFA::NFA(Classifier *aUTF8Classifier) {
  states            = NULL;
  curState          = NULL;
  lastState         = NULL;
  nfaStartState     = NULL;
  nfaLastStartState = NULL;
  curStateVector    = -1;
  numStateVectors   = 0;
  utf8Classifier    = aUTF8Classifier;
}

NFA::~NFA(void) {
  if (states) {
    for (size_t i = 0; i < numStateVectors; i++) {
      if (states[i]) free(states[i]);
      states[i] = NULL;
    }
    free(states);
    states = NULL;
  }
  curState          = NULL;
  lastState         = NULL;
  nfaStartState     = NULL;
  nfaLastStartState = NULL;
  curStateVector    = 0;
  numStateVectors   = 0;
  utf8Classifier    = NULL; // classifier is not "owned" by the NFA instance
}

void NFA::preAddStates(size_t reLength) {
  curStateVector++;
  // Ensure there are enough NFA State vectors for this additional collection
  if (numStateVectors <= curStateVector) {
    State **oldStates = states;
    states = (State**) calloc(numStateVectors + 10, sizeof(State*));
    if (oldStates) {
      memcpy(states, oldStates, numStateVectors*sizeof(State*));
    }
    numStateVectors += 10;
  }

  states[curStateVector] = (State*) calloc(2*reLength, sizeof(State));
  curState = states[curStateVector];
  lastState = curState + 2*reLength;
  curState--;
}

NFA::State *NFA::addState(NFA::MatchType aMatchType,
                          NFA::MatchData someMatchData,
                          NFA::State *out,
                          NFA::State *out1)
  throw (LexerException) {
  if (lastState < curState) throw LexerException("run out of NFA states");
  curState++;
  curState->matchType = aMatchType;
  curState->matchData = someMatchData;
  curState->out       = out;
  curState->out1      = out1;
  return curState;
}

/*
 * Convert infix regexp re to postfix notation.
 * Insert . as explicit concatenation operator.
 * Cheesy parser, return static buffer.
 */
void NFA::addRegularExpressionForToken(const char *aUtf8RegExp,
                                       token_t aTokenId)
                                       throw (LexerException) {

  size_t reLen = strlen(aUtf8RegExp);
  preAddStates(reLen);
  Utf8Chars *re = new Utf8Chars(aUtf8RegExp);
  int nalt, natom;
  NFAFragments *fragments = new NFAFragments(this, reLen);
  struct {
    int nalt;
    int natom;
  } paren[reLen], *p;
  size_t classNameBufSize = 255;
  char classNameBuf[classNameBufSize+1];
  char *className;
  bool classNegated;
  Classifier::classSet_t classSet;
  MatchData noMatchData;
  noMatchData.c.u = 0;
  State *baseSplitState = addState(Split, noMatchData, NULL, NULL);

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
          throw LexerException("parentheses too deep");
        p->nalt = nalt;
        p->natom = natom;
        p++;
        nalt = 0;
        natom = 0;
        break;
      case '|':
        if (natom == 0)
          throw LexerException("no previous atom found in alternation");
        while (--natom > 0) fragments->concatenate();
        nalt++;
        break;
      case ')':
        if (p == paren) throw LexerException("mismatched parentheses");
        if (natom == 0) throw LexerException("no previous atom found before closing paranthesis");
        while (--natom > 0) fragments->concatenate();
        for (; nalt > 0; nalt--) fragments->alternate();
        --p;
        nalt = p->nalt;
        natom = p->natom;
        natom++;
        break;
      case '*':
        if (natom == 0) throw LexerException("no previous atom found for zero or more");
        fragments->zeroOrMore();
        break;
      case '+':
        if (natom == 0) throw LexerException("no previous atom found for one or more");
        fragments->oneOrMore();
        break;
      case '?':
        if (natom == 0) throw LexerException("no previous atom found for zero or one");
        fragments->zeroOrOne();
        break;
      case '[':
        // parse out the className
        className = classNameBuf;
        className[classNameBufSize] = 0;
        for (size_t i = 0; i < classNameBufSize; i++) {
          className[i] = re->getNextByte();
          if (className[i] == ']') { className[i] = 0; break; }
          if (className[i] == 0) break;
        }
        // determine if this is a negated class
        classNegated = false;
        if (*className == '!') {
          classNegated = true;
          className++;
        }
        // find the class set for this className
        if (className[0] == 0) throw LexerException("mallformed classification specifier");
        classSet = utf8Classifier->findClassSet(className);
        // negate the class if needed
        if (classNegated) classSet = ~classSet;
        // now repeat the natom manipulate done for checkCharacter
        if (natom > 1) {
          --natom;
          fragments->concatenate();
        }
        fragments->checkClassification(classSet);
        natom++;
        break;
      case '\\': // escape character.... ignore and use next character instead
        curChar = re->nextUtf8Char();
        if (!curChar.u) continue;
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
  if (p != paren) throw LexerException("mismatched parentheses");
  while (--natom > 0) fragments->concatenate();
  for (; nalt > 0; nalt--) fragments->alternate();
  baseSplitState->out = fragments->match(aTokenId);
  if (nfaLastStartState) {
    nfaLastStartState->out1 = baseSplitState;
    nfaLastStartState       = baseSplitState;
  } else {
    nfaStartState     = baseSplitState;
    nfaLastStartState = nfaStartState;
  }
}

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

