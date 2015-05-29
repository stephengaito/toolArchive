#ifndef DFA_DFA_H
#define DFA_DFA_H

#include "dfa/nextStateMapping.h"

/// \brief The DFA namespace collects the various parts of the DFA
/// interpretor into one logical collection.
namespace DeterministicFiniteAutomaton {

  /// \brief The DFA class is used to interpret a given NFA.
  ///
  /// Directly inrepreting a given NFA typically requires backtracking
  /// when a given NFA::State is found which has not currently valid
  /// successor state for a given string.
  ///
  /// Directly interpreting a given DFA does not require any backtracking.
  /// If a given DFA::DState has no currently valid successor states, then
  /// the DFA as a whole does not recognize the give string.
  ///
  /// Every NFA can be compiled into a corresponding DFA.
  ///
  /// This DFA class provides the methods to compile the associated NFA
  /// into a DFA on the fly. Essentially the next DFA::DState is a set of
  /// NFA::States which could be successor states of the current set of
  /// NFA::States represented as the current DFA::DState.
  class DFA {
    public:

      /// \brief Create a DFA object corresponding to a given NFA.
      DFA(NFA *anNFA);

      /// \brief Destroy the DFA object.
      ~DFA(void);

      /// \brief Add the NFA::State to the DFA::DState bit set by
      /// following unlabeled (NFA::Split) transitions.
      void addNFAStateToDFAState(State *dfaState, NFA::State *nfaState);

      /// \brief Compute the initial DFA::DState bit set which represents
      /// the start state of this DFA.
      ///
      /// The DFA::DState start state is the bit set of all NFA::State(s)
      /// reachable from the NFA::nfaStartState by following unlabeled
      /// (NFA::Split) transitions.
      void computeDFAStartState(void);

      /// \brief Find or compute the next DFA::DState given a
      /// utf8Char_t character or a Classifier::classSet_t.
      ///
      /// Start by trying to find the nextDFAStateMap find the next
      /// DFA::DState corresponding to either a specific
      /// DFA::DState/utf8Char_t or generic
      /// DFA::DState/Classifier::classSet_t combination.
      ///
      /// If no such (pre-compiled) next DFA::State can be found in the
      /// existing mapping, step the NFA from the states in the
      /// DFA::DState, oldState, bit set using the transitions across
      /// either the UTF8 character, c, or the Classifier::classSet_t,
      /// classifiactionSet, creating and registering a new DFA::DState
      /// bit set.
      State *computeNextDFAState(State *oldState,
                                  utf8Char_t c,
                                  Classifier::classSet_t classificationSet);

      /// \brief Run the DFA until the next token is recognized.
      NFA::TokenId getNextTokenId(Utf8Chars *utf8Stream);

    private:
      /// \brief The NFA associated to this DFA.
      NFA *nfa;

      /// \brief The DFA::StateAllocator for this DFA interpretor.
      StateAllocator *allocator;

      /// \brief The DFA::NextStateMapping for this DFA interpretor.
      NextStateMapping *nextStateMapping;

      /// \brief The initial starting state for this DFA.
      State *dfaStartState;

      /// \brief The bit set of all known NFA::State(s) which are
      /// NFA::token recognizing states.
      ///
      /// This bit set is used to determine if/when a token has been
      /// recognized.
      State *tokensState;

  }; // class DFA
};  // namespace DeterministicFiniteAutomaton

#endif
