#ifndef DFA_DFA_H
#define DFA_DFA_H

#include "dynUtf8Parser/dfa/nextStateMapping.h"

/// \brief The DFA namespace collects the various parts of the DFA
/// interpreter into one logical collection.
namespace DeterministicFiniteAutomaton {

  /// \brief The DFA class is used to interpret a given NFA.
  ///
  /// Directly inrepreting a given NFA typically requires backtracking
  /// when a given NFA::State is found which has not currently valid
  /// successor state for a given string.
  ///
  /// Directly interpreting a given DFA, which does not use the push
  /// down stack, does not require any backtracking. If a given
  /// DFA::State has no currently valid successor states, then the DFA
  /// as a whole does not recognize the give string.
  ///
  /// Every NFA, which does not use the push down stack, can be
  /// compiled into a corresponding DFA which also does not use the
  /// push down stack.
  ///
  /// This DFA class provides the methods to compile the associated NFA
  /// into a DFA on the fly. Essentially the next DFA::State is a set of
  /// NFA::States which could be successor states of the current set of
  /// NFA::States represented as the current DFA::State.
  ///
  /// The ideas required to do this compilation on the fly have been
  /// inspired by [Russ Cox's implementation of Regular
  /// Expressions](https://swtch.com/~rsc/regexp/)
  class DFA {
    public:

      /// \brief Create a DFA object corresponding to a given NFA.
      DFA(NFA *anNFA);

      /// \brief Destroy the DFA object.
      ~DFA(void);

      /// \brief Add the NFA::State to the DFA::State bit set by
      /// following unlabeled (NFA::Split) transitions.
      void addNFAStateToDFAState(State *dfaState, NFA::State *nfaState);

      /// \brief Compute the initial DFA::State bit set for the NFA
      /// start state associated with the given startStateName.
      ///
      /// The DFA::State start state is the bit set of all NFA::State(s)
      /// reachable from the give NFA StartState by following unlabeled
      /// (NFA::Split) transitions.
      State *getDFAStartState(const char *startStateName) {
        return getDFAStartState(nfa->findStartStateId(startStateName));
      }

      /// \brief Compute the initial DFA::State bit set for the NFA
      /// start state.
      ///
      /// The DFA::State start state is the bit set of all NFA::State(s)
      /// reachable from the give NFA StartState by following unlabeled
      /// (NFA::Split) transitions.
      State *getDFAStartState(NFA::State *startStatePtr) {
        return getDFAStartState(nfa->findStartStateId(startStatePtr));
      }

      /// \brief Compute the initial DFA::State bit set for the NFA
      /// start state NFA::StartStateId, startStateId.
      ///
      /// The DFA::State start state is the bit set of all NFA::State(s)
      /// reachable from the give NFA StartState by following unlabeled
      /// (NFA::Split) transitions.
      State *getDFAStartState(NFA::StartStateId startStateId);

      /// \brief Return the DFA::State which represents the single
      /// NFA::State provided.
      State *getDFAStateFromNFAState(NFA::State *nfaState) {
        State *dfaState = allocator->allocateANewState();
        addNFAStateToDFAState(dfaState, nfaState->out);
        addNFAStateToDFAState(dfaState, nfaState->out1);
        return dfaState;
      }

      /// \brief Find or compute the next DFA::State given a
      /// utf8Char_t character or a Classifier::classSet_t.
      ///
      /// Start by trying to use the nextDFAStateMap to find the next
      /// DFA::State corresponding to either a specific
      /// DFA::State/utf8Char_t or generic
      /// DFA::State/Classifier::classSet_t combination.
      ///
      /// If no such (pre-compiled) next DFA::State can be found in the
      /// existing mapping, step the NFA from the states in the
      /// DFA::State, oldState, bit set using the transitions across
      /// either the UTF8 character, c, or the Classifier::classSet_t,
      /// classifiactionSet, creating and registering a new DFA::State
      /// bit set.
      State *computeNextDFAState(State *oldState,
                                  utf8Char_t c,
                                  Classifier::classSet_t classificationSet);

      /// \brief Return the next DFA::State (if any) given the current
      /// character.
      ///
      /// Returns NULL is there is no viable next state.
      State *getNextDFAState(State *curState,
                            utf8Char_t curChar);


      /// \brief Returns a DFA::State which represents the currently
      /// knonw NFA::State which are tokens.
      State *getTokensState(void) {
        return tokensState;
      }

      /// \brief Return the NFA associated with this DFA.
      NFA *getNFA(void) {
        return nfa;
      }

      /// \brief Return the StateAllocator associated with this DFA.
      StateAllocator *getStateAllocator(void) {
        return allocator;
      }

    protected:
      /// \brief The NFA associated to this DFA.
      NFA *nfa;

      /// \brief The DFA::StateAllocator for this DFA interpretor.
      StateAllocator *allocator;

      /// \brief The DFA::NextStateMapping for this DFA interpretor.
      NextStateMapping *nextStateMapping;

      /// \brief The bit set of all known NFA::State(s) which are
      /// NFA::token recognizing states.
      ///
      /// This bit set is used to determine if/when a token has been
      /// recognized.
      State *tokensState;

      /// \brief The array of DFA::State(s) corresponding to the
      /// NFA startStates indexed by the NFA::StartStateId.
      State **startState;

      /// \brief The total number of start states.
      size_t numStartStates;

  }; // class DFA
};  // namespace DeterministicFiniteAutomaton

#endif
