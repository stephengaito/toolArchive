#include <stdlib.h>
#include "dfa/pushDownMachine.h"

using namespace DeterministicFiniteAutomaton;

static const char *indents[] = {
  "", //0
  "  ", //1
  "    ", //2
  "      ", //3
  "        ", //4
  "          ", //5
  "            ", //6
  "              ", //7
  "                " //8
};

void PDMTracer::reportNFAState(NFA::State *nfaState,
                                             size_t indent) {
  fprintf(traceFile, "%s%s\n",indents[indent], nfaState->message);
}

void PDMTracer::reportDFAState(size_t indent) {
  NFAStateIterator iterator =
    pdm->allocator->newIteratorOn(pdm->curState.dState);
  fprintf(traceFile, "%sCurrent state: %s\n",
    indents[indent], pdm->curState.message);
  fprintf(traceFile, "%sNFA states:\n",
          indents[indent]);
  while (NFA::State *nfaState = iterator.nextState()) {
    reportNFAState(nfaState, indent+1);
  }
}

void PDMTracer::reportAutomataStack(size_t indent) {
  fprintf(traceFile, "-------------------------------------\n");
  fprintf(traceFile, "%sAutomataStack:\n", indents[indent]);
  for (size_t i = 0; i < pdm->stack.getNumItems(); i++) {
    fprintf(traceFile, "%s%s\n",
      indents[indent+1],
      pdm->stack.getItem(i, PushDownMachine::AutomataState()).message);
  }
  fprintf(traceFile, "-------------------------------------\n");
}

void PDMTracer::reportState(size_t indent) {
  reportAutomataStack(indent);
  fprintf(traceFile, "%s", indents[indent]);
  reportStreamPrefix();
  reportStreamPostfix();
  fprintf(traceFile, "\n");
  reportDFAState(indent);
}

void PDMTracer::reportStreamPrefix(void) {
  char *prefix = pdm->curState.stream->getCopyOfTextRead();
  fprintf(traceFile, "[%s]", prefix);
  free(prefix);
}

void PDMTracer::reportStreamPostfix(void) {
  char *postfix = pdm->curState.stream->getCopyOfTextToRead(30);
  fprintf(traceFile, "[%s]", postfix);
  free(postfix);
}

void PDMTracer::reportChar(utf8Char_t curChar, size_t indent) {
  fprintf(traceFile, "%s%s:", indents[indent], pdm->curState.message);
  reportStreamPrefix();
  fprintf(traceFile, "%s(%lu)", curChar.c, curChar.u);
  reportStreamPostfix();
  fprintf(traceFile, "\n");
}

void PDMTracer::push(size_t indent) {
  fprintf(traceFile, "%spush::%s \n", indents[indent], pdm->curState.message);
}

void PDMTracer::pop(bool keepStream, size_t indent) {
  fprintf(traceFile, "%spop::%s (%s stream)\n", indents[indent],
          pdm->curState.message,
          (keepStream ? "keep" : "pop"));
}

void PDMTracer::swap(size_t indent) {
    fprintf(traceFile, "swap\n");
}

void PDMTracer::checkForRestart(size_t indent) {
    fprintf(traceFile, "checkForRestart\n");
}

/// \brief Trace the use of a restart state transition.
void PDMTracer::restart(NFA::State *nfaState, size_t indent) {
  fprintf(traceFile, "%sreStart", indents[indent]);
  if (nfaState->matchType == NFA::ReStart) {
    NFA::State *reStartState = pdm->nfa->getStartState(nfaState->matchData.r);
    fprintf(traceFile, "{%s} ", reStartState->message);
  }
  reportStreamPrefix();
  reportStreamPostfix();
  fprintf(traceFile, "\n");
}

void PDMTracer::match(NFA::State *nfaState, size_t indent) {
  size_t textSize = pdm->curState.stream->getNumberOfBytesRead()+10;
  char text[textSize];
  memset(text, 0, textSize);
  strncpy(text, pdm->curState.stream->getStart(),
          pdm->curState.stream->getNumberOfBytesRead());
  fprintf(traceFile, "%s [%s](%zu)\n",
          nfaState->message, text, pdm->curState.stream->getNumberOfBytesRead());
}


void PDMTracer::done(size_t indent) {
  fprintf(traceFile, "done\n");
}

void PDMTracer::failedWithStream(size_t indent) {
  fprintf(traceFile, "failed with stream\n");
}

void PDMTracer::nextDFAState(size_t indent) {
  fprintf(traceFile, "nextDFAState\n");
}

void PDMTracer::failedBacktrack(size_t indent) {
  fprintf(traceFile, "failedBackTrack\n");
}

void PDMTracer::backtrack(size_t indent) {
  fprintf(traceFile, "backtrack\n");
}

void PDMTracer::error(size_t indent) {
  fprintf(traceFile, "error!\n");
}




