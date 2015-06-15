#ifndef PDM_TRACER_H
#define PDM_TRACER_H

#include "dfa/dfa.h"

#define PDMTraceMessages(pdmTracer) \
  ((pdmTracer) && ((pdmTracer)->trace(PDMTracer::Messages)))

#define PDMTraceRestartMessages(pdmTracer) \
  ((pdmTracer) && ((pdmTracer)->trace(PDMTracer::RestartMessages)))

namespace DeterministicFiniteAutomaton {

  // forward declaration of the PushDownMachine class
  class PushDownMachine;

  /// \brief A PushDownMachine::Tracer object is used to trace a
  /// given PushDownMachine's state transitions.
  class PDMTracer {

    public:

      enum TraceConditions {
        All=~0L,
        Messages=1,
        NFAState=2,
        DFAState=4,
        AutomataStack=8,
        PDMState=16,
        CurStreamPosition=32,
        StackPushes=64,
        StackPops=128,
//        StackSwaps=256,
        CheckForRestarts=512,
        PDMRestarts=1024,
        PDMMatch=2048,
        PDMDone=4096,
        PDMFailedWithStream=8192,
        PDMNextDFAState=16384,
        PDMFailedBackTrack=32768,
        PDMBackTrack=65536,
        PDMErrorReturn=131072,
        RestartMessages=262144,
        PDMTokens=524288,
        Progress=(PDMMatch|CurStreamPosition|RestartMessages),
//        PDMStack=(StackPushes|StackPops|StackSwaps),
        PDMStack=(StackPushes|StackPops),
        SimpleState=(NFAState|DFAState|AutomataStack|PDMTokens|PDMState),
        Transitions=(PDMErrorReturn|PDMBackTrack|PDMFailedBackTrack|PDMNextDFAState|PDMFailedWithStream|PDMDone|PDMMatch)
      };

      /// \brief Create a new PushDownMachine instance.
      PDMTracer(const char *aMessage, FILE *aTraceFile = NULL) {
        pdm        = NULL;
        message    = aMessage;
        traceFile  = aTraceFile;
        conditions = 0; // trace nothing unless told to do so
      }

      /// \brief Destroy the tracer.
      ~PDMTracer(void) {
        pdm       = NULL; // we do not own the PDM
        message   = NULL; // we do not own the message
        traceFile = NULL; // we do not own the FILE
      }

      void setCondition(uint64_t someConditions) {
        conditions |= someConditions;
      }

      void clearCondition(uint64_t someConditions) {
        conditions &= ~someConditions;
      }

      bool trace(uint64_t someConditions) {
        return (conditions & someConditions);
      }

      /// \brief Sets the associated PushDownMachine
      void setPDM(PushDownMachine *aPDM) {
        pdm = aPDM;
        if (traceFile && conditions) fprintf(traceFile, "PDMTracer: %s\n", message);
      }

      void reportState(size_t indent = 0);
      void reportAutomataStack(size_t indent = 0);
      void reportNFAState(NFA::State *nfaState, size_t indent = 0);
      void reportDFAState(size_t indent = 0);
      void reportChar(utf8Char_t curChar, size_t indent = 0);
      void reportTokens(size_t indent = 0);
      void reportStreamPrefix();
      void reportStreamPostfix();
//      void swap(size_t indent = 0);
      void push(const char *message, size_t indent = 0);
      void pop(const char *message0,
               const char *message1 = "",
               size_t indent = 0);
      void checkForRestart(size_t indent = 0);
      /// \brief Trace the use of a restart state transition.
      void restart(size_t indent = 0);
      void match(NFA::State *nfaState, size_t indent = 0);
      void done(size_t indent = 0);
      void failedWithStream(size_t indent = 0);
      void nextDFAState(size_t indent = 0);
      void failedBacktrack(size_t indent = 0);
      void backtrack(size_t indent = 0);
      void errorReturn(size_t indent = 0);

    protected:

      /// \brief The currently associated PushDownMachine.
      ///
      /// Used to access the internal state of the PDM.
      PushDownMachine *pdm;

      /// \brief A message associated with this Tracer.
      const char *message;

      /// \brief Whether or not to trace the state transitions.
      FILE *traceFile;

      /// \brief The BitSet of trace conditions
      uint64_t conditions;

  }; // class Tracer

};  // namespace DeterministicFiniteAutomaton

#endif
