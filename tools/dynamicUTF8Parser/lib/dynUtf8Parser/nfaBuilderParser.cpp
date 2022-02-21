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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "dynUtf8Parser/nfaBuilder.h"

void NFABuilder::compileRegularExpressionForTokenId(
  const char *startStateName,
  const char *aUtf8RegExp,
  Token::TokenId aTokenId,
  bool ignoreToken)
  throw (ParserException) {

  // wrap the ignoreToken boolean into the lowest order bit of
  // aTokenId for subsequent retrieval
  Token::TokenId maxTokenId = (~0L)>>1;
  if (maxTokenId < aTokenId) throw ParserException("TokenId too large");

  nfa->registerStartState(startStateName);
  size_t reLen = strlen(aUtf8RegExp);
  Utf8Chars *re = new Utf8Chars(aUtf8RegExp);
  int nalt, natom;
  struct {
    int nalt;
    int natom;
  } paren[reLen], *p;
  size_t nameBufSize = 255;
  char nameBuf[nameBufSize+1];
  char *className;
  bool classNegated;
  Classifier::classSet_t classSet;
  char *reStartStateName;
  NFA::StartStateId reStartStateId;
  NFA::MatchData noMatchData;
  noMatchData.c.u = 0;
  NFA::State *baseSplitState =
    nfa->addState(NFA::Split, noMatchData, NULL, NULL, startStateName);

  p = paren;
  nalt = 0;
  natom = 0;
  utf8Char_t curChar = re->nextUtf8Char();
  while (curChar.u) {
    switch (curChar.c[0]) {
      case '(':
        if (natom > 1) {
          --natom;
          concatenate();
        }
        p->nalt = nalt;
        p->natom = natom;
        p++;
        nalt = 0;
        natom = 0;
        break;
      case '|':
        if (natom == 0) {
          delete re;
          nfa->deleteState(baseSplitState);
          throw ParserException("no previous atom found in alternation");
        }
        while (--natom > 0) concatenate();
        nalt++;
        break;
      case ')':
        if (p == paren) {
          delete re;
          nfa->deleteState(baseSplitState);
          throw ParserException("mismatched parentheses");
        }
        if (natom == 0) {
          delete re;
          nfa->deleteState(baseSplitState);
          throw ParserException("no previous atom found before closing paranthesis");
        }
        while (--natom > 0) concatenate();
        for (; nalt > 0; nalt--) alternate();
        --p;
        nalt = p->nalt;
        natom = p->natom;
        natom++;
        break;
      case '*':
        if (natom == 0) {
          delete re;
          nfa->deleteState(baseSplitState);
          throw ParserException("no previous atom found for zero or more");
        }
        zeroOrMore();
        break;
      case '+':
        if (natom == 0) {
          delete re;
          nfa->deleteState(baseSplitState);
          throw ParserException("no previous atom found for one or more");
        }
        oneOrMore();
        break;
      case '?':
        if (natom == 0) {
          delete re;
          nfa->deleteState(baseSplitState);
          throw ParserException("no previous atom found for zero or one");
        }
        zeroOrOne();
        break;
      case '[':
        // parse out the className
        className = nameBuf;
        className[nameBufSize] = 0;
        for (size_t i = 0; i < nameBufSize; i++) {
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
        if (className[0] == 0) {
          delete re;
          nfa->deleteState(baseSplitState);
          throw ParserException("mallformed classification specifier");
        }
        classSet = nfa->findClassSet(className);
        // negate the class if needed
        if (classNegated) {
          classSet = ~classSet;
          className--; // put the initial ! back for debugging purposes
        }
        // now repeat the natom manipulate done for checkCharacter
        if (natom > 1) {
          --natom;
          concatenate();
        }
        checkClassification(classSet, className);
        natom++;
        break;
      case '{':
        // parse out the reStartStateName
        reStartStateName = nameBuf;
        reStartStateName[nameBufSize] = 0;
        for (size_t i = 0; i < nameBufSize; i++) {
          reStartStateName[i] = re->getNextByte();
          if (reStartStateName[i] == '}') { reStartStateName[i] = 0; break; }
          if (reStartStateName[i] == 0) break;
        }
        // find the reStartId for this reStartStateName
        if (reStartStateName[0] == 0) {
          delete re;
          nfa->deleteState(baseSplitState);
          throw ParserException("mallformed reStart name");
        }
        reStartStateId = nfa->findStartStateId(reStartStateName);
        if (reStartStateId == -1L) {
          delete re;
          nfa->deleteState(baseSplitState);
          throw ParserException("unregistered reStartStateId");
        }
        // now repeat the natom manipulate done for checkCharacter
        if (natom > 1) {
          --natom;
          concatenate();
        }
        reStart(reStartStateId, reStartStateName);
        natom++;
        break;
      case '\\': // escape character.... ignore and use next character instead
        curChar = re->nextUtf8Char();
        if (!curChar.u) continue;
      default:
        if (natom > 1) {
          --natom;
          concatenate();
        }
        checkCharacter(curChar);
        natom++;
        break;
      }
      curChar = re->nextUtf8Char();
    }
  delete re;
  if (p != paren) {
    nfa->deleteState(baseSplitState);
    throw ParserException("mismatched parentheses");
  }
  while (--natom > 0) concatenate();
  for (; nalt > 0; nalt--) alternate();
  if (!stack.getNumItems()) {
    nfa->deleteState(baseSplitState);
    throw ParserException("empty regular expression - nothing to match");
  }
  baseSplitState->out = match(aTokenId, startStateName, ignoreToken);
  nfa->appendNFAToStartState(startStateName, baseSplitState);
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

