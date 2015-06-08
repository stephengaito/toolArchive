#ifndef PDM_TRACER_H
#define PDM_TRACER_H

#include "dfa/dfa.h"

namespace DeterministicFiniteAutomaton {

  // forward declaration of the PushDownMachine class
  class PushDownMachine;

  /// \brief A PushDownMachine::Tracer object is used to trace a
  /// given PushDownMachine's state transitions.
  class PDMTracer {

    public:

      /// \brief Create a new PushDownMachine instance.
      PDMTracer(const char *aMessage, FILE *aTraceFile = NULL) {
        pdm       = NULL;
        message   = aMessage;
        traceFile = aTraceFile;
      }

      /// \brief Destroy the tracer.
      ~PDMTracer(void) {
        pdm       = NULL; // we do not own the PDM
        message   = NULL; // we do not own the message
        traceFile = NULL; // we do not own the FILE
      }

      /// \brief Sets the associated PushDownMachine
      void setPDM(PushDownMachine *aPDM) {
        pdm = aPDM;
        if (traceFile) fprintf(traceFile, "PDMTracer: %s\n", message);
      }

      void reportState(size_t indent = 0);
      void reportAutomataStack(size_t indent = 0);
      void reportNFAState(NFA::State *nfaState, size_t indent = 0);
      void reportDFAState(size_t indent = 0);
      void reportChar(utf8Char_t curChar, size_t indent = 0);
      void reportStreamPrefix();
      void reportStreamPostfix();
      void swap(size_t indent = 0);
      void push(size_t indent = 0);
      void pop(bool keepStream, size_t indent = 0);
      void checkForRestart(size_t indent = 0);
      /// \brief Trace the use of a restart state transition.
      void restart(NFA::State *nfaState, size_t indent = 0);
      void match(NFA::State *nfaState, size_t indent = 0);
      void done(size_t indent = 0);
      void failedWithStream(size_t indent = 0);
      void nextDFAState(size_t indent = 0);
      void failedBacktrack(size_t indent = 0);
      void backtrack(size_t indent = 0);
      void error(size_t indent = 0);

    private:

      /// \brief The currently associated PushDownMachine.
      ///
      /// Used to access the internal state of the PDM.
      PushDownMachine *pdm;

      /// \brief A message associated with this Tracer.
      const char *message;

      /// \brief Whether or not to trace the state transitions.
      FILE *traceFile;

  }; // class Tracer

};  // namespace DeterministicFiniteAutomaton

#endif
