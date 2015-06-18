#ifndef PDM_TRACER_H
#define PDM_TRACER_H

#include "dynUtf8Parser/dfa/dfa.h"

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

      /// \brief An enumeration of the known TraceConditions. This
      /// enumeration is used to selectively turn on/off specific trace
      /// messages.
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

      /// \brief Create a new PDMTracer instance.
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

      /// \brief Set one or more trace conditions (see TraceConditions
      /// above).
      void setCondition(uint64_t someConditions) {
        conditions |= someConditions;
      }

      /// \brief Clear one or more trace conditions (see TraceConditions
      /// above).
      void clearCondition(uint64_t someConditions) {
        conditions &= ~someConditions;
      }

      /// \brief Return true if the provided conditions match the
      /// current conditions for this tracer.
      bool trace(uint64_t someConditions) {
        return (conditions & someConditions);
      }

      /// \brief Sets the associated PushDownMachine
      void setPDM(PushDownMachine *aPDM) {
        pdm = aPDM;
        if (traceFile && conditions) fprintf(traceFile, "PDMTracer: %s\n", message);
      }

      /// \brief Report the PushDownMachine's current state.
      void reportState(size_t indent = 0);

      /// \brief Report the PushDownMachine's current stack.
      void reportAutomataStack(size_t indent = 0);

      /// \brief Report the NFA::State.
      void reportNFAState(NFA::State *nfaState, size_t indent = 0);

      /// \brief Report the DFA state (and all NFA::States in the bitset).
      void reportDFAState(size_t indent = 0);

      /// \brief Report the character provided.
      void reportChar(utf8Char_t curChar, size_t indent = 0);

      /// \brief Report the current token.
      void reportTokens(size_t indent = 0);

      /// \brief Report the string that *has* *been* read.
      void reportStreamPrefix();

      /// \brief Report the string that has *not* been read yet.
      void reportStreamPostfix();

      /// \brief Report a push.
      void push(const char *message, size_t indent = 0);

      /// \brief Report a pop.
      void pop(const char *message0,
               const char *message1 = "",
               size_t indent = 0);

      /// \brief Report that the PushDownMachine is checking for a
      /// restart state.
      void checkForRestart(size_t indent = 0);

      /// \brief Trace the use of a restart state transition.
      void restart(size_t indent = 0);

      /// \brief Report that a match has been found.
      void match(NFA::State *nfaState, size_t indent = 0);

      /// \brief Report the successfull recognition of a stream.
      void done(size_t indent = 0);

      /// \brief Report the recognition failure with some stream still
      /// unread.
      void failedWithStream(size_t indent = 0);

      /// \brief Report the transition to a new DFA state.
      void nextDFAState(size_t indent = 0);

      /// \brief Report the recognition failure with no more
      /// backtracking avaiable.
      void failedBacktrack(size_t indent = 0);

      /// \brief Report that the PushDownMachine is backtracking.
      void backtrack(size_t indent = 0);

      /// \brief Report that the PushDownMachine has failed by reaching
      /// code which should not have been reached.
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
