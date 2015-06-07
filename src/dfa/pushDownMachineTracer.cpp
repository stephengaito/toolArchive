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

void PushDownMachine::Tracer::reportNFAState(NFA::State *nfaState,
                                             size_t indent) {
  fprintf(traceFile, "%s%s\n",indents[indent], nfaState->message);
}


void PushDownMachine::Tracer::reportDFAState(size_t indent) {
  NFAStateIterator iterator =
    pdm->allocator->newIteratorOn(pdm->curState.dState);
  fprintf(traceFile, "-------------------------------------\n%sNFA states:\n",
          indents[indent]);
  while (NFA::State *nfaState = iterator.nextState()) {
    reportNFAState(nfaState, indent+1);
  }
}

void PushDownMachine::Tracer::reportStreamPrefix(void) {
  char *prefix = pdm->curState.stream->getCopyOfTextRead();
  fprintf(traceFile, "[%s]", prefix);
  free(prefix);
}

void PushDownMachine::Tracer::reportStreamPostfix(void) {
  char *postfix = pdm->curState.stream->getCopyOfTextToRead(30);
  fprintf(traceFile, "[%s]", postfix);
  free(postfix);
}

void PushDownMachine::Tracer::reportChar(utf8Char_t curChar, size_t indent) {
  fprintf(traceFile, "%s%s:", indents[indent], pdm->curState.message);
  reportStreamPrefix();
  fprintf(traceFile, "%s(%lu)", curChar.c, curChar.u);
  reportStreamPostfix();
  fprintf(traceFile, "\n");
}

void PushDownMachine::Tracer::push(size_t indent) {
  fprintf(traceFile, "%spush::%s \n", indents[indent], pdm->curState.message);
}

void PushDownMachine::Tracer::pop(bool keepStream, size_t indent) {
  fprintf(traceFile, "%spop::%s (%s stream)\n", indents[indent],
          pdm->curState.message,
          (keepStream ? "keep" : "pop"));
}

/// \brief Trace the use of a restart state transition.
void PushDownMachine::Tracer::restart(NFA::State *nfaState, size_t indent) {
  fprintf(traceFile, "%sreStart", indents[indent]);
  if (nfaState->matchType == NFA::ReStart) {
    NFA::State *reStartState = pdm->nfa->getStartState(nfaState->matchData.r);
    fprintf(traceFile, "{%s} ", reStartState->message);
  }
  reportStreamPrefix();
  reportStreamPostfix();
  fprintf(traceFile, "\n");
}

void PushDownMachine::Tracer::match(NFA::State *nfaState, size_t indent) {
  size_t textSize = pdm->curState.stream->getNumberOfBytesRead()+10;
  char text[textSize];
  memset(text, 0, textSize);
  strncpy(text, pdm->curState.stream->getStart(),
          pdm->curState.stream->getNumberOfBytesRead());
  fprintf(traceFile, "%s [%s](%zu)\n",
          nfaState->message, text, pdm->curState.stream->getNumberOfBytesRead());
}


void PushDownMachine::Tracer::done(size_t indent) {
  fprintf(traceFile, "done\n");
}

void PushDownMachine::Tracer::failedWithStream(size_t indent) {
  fprintf(traceFile, "failed with stream\n");
}

void PushDownMachine::Tracer::nextDFAState(size_t indent) {
  fprintf(traceFile, "nextDFAState\n");
}

void PushDownMachine::Tracer::failedBacktrack(size_t indent) {
  fprintf(traceFile, "failedBackTrack\n");
}

void PushDownMachine::Tracer::backtrack(size_t indent) {
  fprintf(traceFile, "backtrack\n");
}

void PushDownMachine::Tracer::error(size_t indent) {
  fprintf(traceFile, "error!\n");
}




