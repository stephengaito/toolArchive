#ifndef NFA_H
#define NFA_H

#include <exception>
#include <cUtils/blockAllocator.h>
#include "dynUtf8Parser/classifier.h"
#include "dynUtf8Parser/tokens.h"

/// \brief ParserExceptions provide simple messages detailing why the
/// Parser can not proceed.
///
/// ParserExceptions are thrown whenever a regular expression
/// is malformed, or the DFA state used to recognize a token is corrupted.
class ParserException: public std::exception {
  public:
    /// \brief Initialize a ParserException structure.
    ParserException(const char* aMessage) { message = aMessage; };

    /// \brief The message associated with this ParserException
    /// instance.
    const char* message;
};


/// \brief [Nondeterministic Finite
/// Automata](http://en.wikipedia.org/wiki/Nondeterministic_finite_automaton)
/// (NFA) are used to recognize [Regular
/// Expressions](http://en.wikipedia.org/wiki/Regular_expression).
///
///  We have explicitly extended these NFAs and Regular expressions with
/// the ability to restart recognition at different start states in the
/// NFA. This turns the NFA into a [Push Down
/// Machine](https://en.wikipedia.org/wiki/Pushdown_automaton) which is
/// more powerful than a standard NFA.
///
/// Since these NFAs will be used as part of a Parser with multiple
/// tokens, (or 'rules'), to be recognized, our NFAs have a specific
/// structure. The NFA start state(s) consist of what is essentially a
/// linked list of NFA::Split states whose out pointer points to the
/// (sub)NFA to recognize a given token and whose out1 pointer points
/// to the next (sub)NFA/token in the linked list. Each (sub)NFA which
/// recognizes a given token, will end with an NFA::Token state whose
/// matchData stores the token ID associated with the recognized token.
///
/// The internals of the NFA class have been heavily inspired by [Russ
/// Cox's Regular Exprssion code](https://swtch.com/~rsc/regexp/). See the
/// NFABuilder class for details and license.
///
/// This class uses the [Hat-Trie
/// library](https://github.com/dcjones/hat-trie).
class NFA {

  public:
    /// \brief Create a new NFA which uses the provided Classifier
    /// to classify UTF8 character classes.
    NFA(Classifier *aUTF8Classifier);

    /// \brief Destroy the NFA.
    ~NFA(void);

    /// \brief The NFA::MatchType enumeration provides a classifiacation
    /// of a given NFA::State structure.
    ///
    /// A given NFA::State can match a Character, a (character)
    /// class set, represent a (recursive) ReStart state, a Token, or
    /// be an (internal) Split state. Correctly formed NFA::State(s)
    /// should never be Empty.
    enum MatchType {
      Empty     = 0,
      Character = 1,
      ClassSet  = 2,
      ReStart   = 3,
      Split     = 4,
      Token     = 5
    };

    /// \brief A StartStateId represents a starting state for the NFA.
    typedef value_t StartStateId;

    /// \brief The NFA::MatchData union provides the data required to
    /// match a given NFA::State,
    ///
    /// The NFA::MatchType enumeration provides an explicit marker to
    /// determine which of the utf8Char_t, classSet_t or token_t structures
    /// should be used to provide the match data for a given NFA::State.
    typedef union MatchData {
        /// \brief The utf8Char_t structure used to explicitly match a
        /// character.
        utf8Char_t c;
        /// \brief The classSet_t structure used to implicitly match a
        /// class of UTF8 characters using a given Classifier.
        Classifier::classSet_t s;
        /// \brief The token ID associated to a given matched (sub)NFA.
        Token::TokenId  t;
        /// \brief The StartState ID associated to a given (recursive)
        /// push down state.
        StartStateId r;
      } MatchData;

    /// \brief Every NFA is a graph of NFA::State stuctures which is
    /// walked over to recognize one or more tokens.
    typedef struct State {

      /// \brief The NFA::MatchType used to successfully match this
      /// NFA::State.
      MatchType matchType;

      /// \brief The NFA::MatchData corresponding to the NFA::MatchType
      /// used to determine a successful match of this NFA::State.
      MatchData matchData;

      /// \brief One of two possible next NFA::State(s).
      ///
      /// Unless this NFA::State is a NFA::Token state, this out pointer
      /// always points at the next state to transition to if the
      /// current character matches either the utf8Chat_t or classSet_t
      /// structures.
      State *out;

      /// \brief An alternate possiblee next NFA::State(s).
      ///
      /// The out1 pointer is only ever used by an NFA::Split state.
      /// It is used to enable alternate successor states for, for example,
      /// ZeroOrMore, OneOrMore, or ZeorOrOne decision points.
      State *out1;

      const char *message;
    } State;

    /// \brief Get the Classifier associated with this NFA.
    Classifier *getClassifier(void) { return utf8Classifier; }

    /// \brief Find the class set associated with the given className.
    Classifier::classSet_t findClassSet(const char *className) {
      return utf8Classifier->findClassSet(className);
    }

    /// \brief (pre)Register the name of a StartState that might be
    /// used in one or more regular expressions.
    void registerStartState(const char *startStateName) {
      StartStateId *startStateId = hattrie_get(startStateIds,
                                               startStateName,
                                               strlen(startStateName));
      ASSERT(startStateId); // corrupted startStateIds Hat-Trie
      if (!*startStateId) {
        // we need to allocate a new startStateId
        // to be able to interact with the above check for a newly
        // allocated startStateName Hat-Trie, the *startStateId
        // MUST be one-relative rather than the usual zero-relative.
        *startStateId = startState.getNumItems() + 1;
        startState.pushItem(NULL);
      }
    }

    /// \brief (pre)Register the name of a StartState that might be
    /// used in one or more regular expressions.
    void registerStartState(State *startStatePtr) {
      StartStateId *startStateId = hattrie_get(startStateIds,
                                               (char*)startStatePtr,
                                               sizeof(State*));
      ASSERT(startStateId); // corrupted startStateIds Hat-Trie
      if (!*startStateId) {
        // we need to allocate a new startStateId
        // to be able to interact with the above check for a newly
        // allocated startStateName Hat-Trie, the *startStateId
        // MUST be one-relative rather than the usual zero-relative.
        *startStateId = startState.getNumItems() + 1;
        startState.pushItem(NULL);
      }
    }

    /// \brief Find the StartState Id associated with the given StartStateName.
    StartStateId findStartStateId(const char *startStateName) {
      StartStateId *startStateId = hattrie_tryget(startStateIds,
                                                  startStateName,
                                                  strlen(startStateName));
      if (!startStateId) return -1L;
      // internally *startStateId is 1-relative
      // so we convert it to 0-relative externally.
      return *startStateId - 1; 
    }

    /// \brief Find the StartState Id associated with the given StartStateName.
    StartStateId findStartStateId(State *startStatePtr) {
      StartStateId *startStateId = hattrie_tryget(startStateIds,
                                                  (char*)startStatePtr,
                                                  sizeof(State*));
      if (!startStateId) return -1L;
      // internally *startStateId is 1-relative
      // so we convert it to 0-relative externally.
      return *startStateId - 1;
    }

    /// \brief Get the named start state.
    State *getStartState(const char *startStateName) {
      return getStartState(findStartStateId(startStateName));
    }

    /// \brief Get the start state associated to the StartStateId.
    State *getStartState(StartStateId startStateId) {
      return startState.getItem(startStateId, NULL);
    }

    /// \brief Append the (sub)NFA to the current start state.
    void appendNFAToStartState(const char *startStateName,
                               State *subNFAStartState);

    /// \brief Add a new NFA state.
    State *addState(MatchType aMatchType, MatchData someMatchData,
                    State *out, State *out1, const char *message);

    /// \brief Clean out the given state and all of its substates.
    void deleteState(State *aState);

    /// \brief Get the current number of NFA::States.
    size_t getNumberStates(void) {
      return numKnownStates;
    }

    /// \brief Get the number of registered start states.
    size_t getNumberStartStates(void) {
      return startState.getNumItems();
    }

    /// \brief Print the NFA::State state on the FILE
    /// filePtr together with the message message.
    void printStateOnWithMessage(FILE *filePtr,
                                 const char *message,
                                 State *state);

  protected:

    /// \brief A BlockAllocator which allocates new NFA::States.
    BlockAllocator *stateAllocator;

    /// \brief The Hat-Trie of startStateNames to startStateIds.
    hattrie_t *startStateIds;

    /// \brief The array of known start states for this NFA
    VarArray<State*> startState;

    /// \brief The number of NFA::States added to this NFA.
    size_t numKnownStates;

    /// \brief The Classifier used by this NFA to classify UTF8 characters.
    Classifier *utf8Classifier;
};


#endif
