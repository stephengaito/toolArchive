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
  if (!traceFile || !trace(NFAState)) return;
  fprintf(traceFile, "%s%s\n",indents[indent], nfaState->message);
}

void PDMTracer::reportDFAState(size_t indent) {
  if (!traceFile || !trace(DFAState)) return;
   NFAStateIterator iterator =
    pdm->allocator->newIteratorOn(pdm->curState.dState);
  fprintf(traceFile, "%sCurrent state: %s\n",
    indents[indent], pdm->curState.getStartStateMessage());
  fprintf(traceFile, "%sNFA states:\n",
          indents[indent]);
  while (NFA::State *nfaState = iterator.nextState()) {
    reportNFAState(nfaState, indent+1);
  }
}

void PDMTracer::reportAutomataStack(size_t indent) {
  if (!traceFile || !trace(AutomataStack)) return;
  fprintf(traceFile, "-------------------------------------\n");
  fprintf(traceFile, "%sAutomataStack (%zu):\n", indents[indent],
          pdm->stack.getNumItems());
  for (size_t i = 0; i < pdm->stack.getNumItems(); i++) {
    fprintf(traceFile, "%s%zu: %s(%s)\n",
      indents[indent+1], i,
      pdm->stack.getItem(i, AutomataState()).getStateTypeMessage(),
      pdm->stack.getItem(i, AutomataState()).getStartStateMessage());
  }
  fprintf(traceFile, "-------------------------------------\n");
}

void PDMTracer::reportState(size_t indent) {
  if (!traceFile || !trace(PDMState)) return;
  reportAutomataStack(indent);
  fprintf(traceFile, "%s", indents[indent]);
  reportStreamPrefix();
  reportStreamPostfix();
  fprintf(traceFile, "\n");
  reportTokens(indent);
  reportDFAState(indent);
  ASSERT(pdm->curState.dState == pdm->curState.iterator->origDState);
}

void PDMTracer::reportStreamPrefix(void) {
  if (!traceFile) return;
  char *prefix = pdm->curState.stream->getCopyOfTextRead();
  fprintf(traceFile, "[%s]", prefix);
  free(prefix);
}

void PDMTracer::reportStreamPostfix(void) {
  if (!traceFile) return;
  char *postfix = pdm->curState.stream->getCopyOfTextToRead(30);
  fprintf(traceFile, "[%s]", postfix);
  free(postfix);
}

void PDMTracer::reportChar(utf8Char_t curChar, size_t indent) {
  if (!traceFile || !trace(CurStreamPosition)) return;
  fprintf(traceFile, "%s%s:", indents[indent],
          pdm->curState.getStartStateMessage());
  reportStreamPrefix();
  fprintf(traceFile, "%s(%lu)", curChar.c, curChar.u);
  reportStreamPostfix();
  fprintf(traceFile, "\n");
}

void PDMTracer::reportTokens(size_t indent) {
  if (!traceFile || !trace(PDMTokens)) return;
  fprintf(traceFile, "----------tokens---------------------\n");
    pdm->curState.token->printOn(traceFile, indent+1);
  fprintf(traceFile, "-------------------------------------\n");
}

void PDMTracer::push(const char *message, size_t indent) {
  if (!traceFile || !trace(StackPushes)) return;
  fprintf(traceFile, "%spush::%s(%s)<%s> \n",
          indents[indent], pdm->curState.getStateTypeMessage(),
          pdm->curState.getStartStateMessage(), message);
}

void PDMTracer::pop(const char *message, size_t indent) {
  if (!traceFile || !trace(StackPops)) return;
  fprintf(traceFile, "%spop::%s (%s)\n", indents[indent],
          pdm->curState.getStartStateMessage(), message);
}

void PDMTracer::swap(size_t indent) {
  if (!traceFile || !trace(StackSwaps)) return;
    fprintf(traceFile, "swap\n");
}

void PDMTracer::checkForRestart(size_t indent) {
  if (!traceFile || !trace(CheckForRestarts)) return;
    fprintf(traceFile, "checkForRestart\n");
}

/// \brief Trace the use of a restart state transition.
void PDMTracer::restart(size_t indent) {
  if (!traceFile || !trace(PDMRestarts)) return;
  fprintf(traceFile, "%sreStart{%s} ", indents[indent],
          pdm->curState.getStartStateMessage());
  reportStreamPrefix();
  reportStreamPostfix();
  fprintf(traceFile, "\n");
}

void PDMTracer::match(NFA::State *nfaState, size_t indent) {
  if (!traceFile || !trace(PDMMatch)) return;
  size_t textSize = pdm->curState.stream->getNumberOfBytesRead()+10;
  char text[textSize];
  memset(text, 0, textSize);
  strncpy(text, pdm->curState.stream->getStart(),
          pdm->curState.stream->getNumberOfBytesRead());
  fprintf(traceFile, "%s [%s](%zu)\n",
          nfaState->message, text, pdm->curState.stream->getNumberOfBytesRead());
}


void PDMTracer::done(size_t indent) {
  if (!traceFile || !trace(PDMDone)) return;
  fprintf(traceFile, "done\n");
}

void PDMTracer::failedWithStream(size_t indent) {
  if (!traceFile || !trace(PDMFailedWithStream)) return;
  fprintf(traceFile, "failed with stream\n");
}

void PDMTracer::nextDFAState(size_t indent) {
  if (!traceFile || !trace(PDMNextDFAState)) return;
  fprintf(traceFile, "nextDFAState\n");
}

void PDMTracer::failedBacktrack(size_t indent) {
  if (!traceFile || !trace(PDMFailedBackTrack)) return;
  fprintf(traceFile, "failedBackTrack\n");
}

void PDMTracer::backtrack(size_t indent) {
  if (!traceFile || !trace(PDMBackTrack)) return;
  fprintf(traceFile, "backtrack\n");
}

void PDMTracer::errorReturn(size_t indent) {
  if (!traceFile || !trace(PDMErrorReturn)) return;
  fprintf(traceFile, "error!\n");
}




