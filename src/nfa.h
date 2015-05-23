#ifndef NFA_H
#define NFA_H

#include <exception>
#include "classifier.h"

/// \brief LexerExceptions provide simple messages detailing why the
/// Lexer can not proceed.
///
/// LexerExceptions are thrown when ever a regular expression
/// is malformed, or the DFA state used to recognize a token is corrupted.
class LexerException: public std::exception {
  public:
    /// \brief Initialize a LexerException structure.
    LexerException(const char* aMessage) { message = aMessage; };

    /// \brief The message associated with this LexerException
    /// instance.
    const char* message;
};


/// \brief [Nondeterministic Finite
/// Automata](http://en.wikipedia.org/wiki/Nondeterministic_finite_automaton)
/// (NFA) are used to recognize [Regular
/// Expressions](http://en.wikipedia.org/wiki/Regular_expression).
///
/// Since these NFAs will be used as part of a Lexer with multiple
/// tokens to be recognized, our NFAs have a specific structure.
/// The NFA start state(s) consist of
/// what is essentially a linked list of NFA::Split states whose
/// out pointer points to the (sub)NFA to recognize a given token
/// and whose out1 pointer points to the next (sub)NFA/token in
/// the linked list. Each (sub)NFA which recognizes a given token, will
/// end with an NFA::Token state
/// whose matchData stores the token ID associated with the recognized
/// token.
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
    /// class set, a Token, or be an (internal) Split state. NFA::States
    /// should never be Empty.
    enum MatchType {
      Empty     = 0,
      Character = 1,
      ClassSet  = 2,
      Split     = 3,
      Token     = 4
    };

    /// \brief A token_t represents a given token ID.
    typedef value_t token_t;

    /// \brief The NFA::MatchData union provides the data required to
    /// mathc a given NFA::State,
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
        token_t    t;
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

      /// \brief One of two possible next NFA::States.
      ///
      /// Unless this NFA::State is a NFA::Token state, this out pointer
      /// always points at the next state to transition to if the
      /// current character matches either the utf8Chat_t or classSet_t
      /// structures.
      State *out;

      /// \brief An alternate possiblee next NFA::States.
      ///
      /// The out1 pointer is only ever used by an NFA::Split state.
      /// It is used to enable alternate successor states for, for example,
      /// ZeroOrMore, OneOrMore, or ZeorOrOne decision points.
      State *out1;
    } State;

    /// \brief Get the Classifier associated with this NFA.
    Classifier *getClassifier(void) { return utf8Classifier; }

    /// \brief Add the Regular Expression used to recognize a Token
    /// with Token ID aTokenId.
    void  addRegularExpressionForToken(const char *re, token_t aTokenId)
                                       throw (LexerException);

    /// \brief Add a new NFA state.
    State *addState(MatchType aMatchType, MatchData someMatchData,
                    State *out, State *out1)
                    throw (LexerException);

    /// \brief (pre)Add a collection of empty NFA states.
    ///
    /// To ensure that NFA::States are managed in the heap as
    /// runs of continguous memory locations, to enhance performance,
    /// we need to allocate contiguous memory for a collection of states.
    void preAddStates(size_t reLength);

    /// \brief Get the current number of NFA::States.
    size_t getNumberStates() {
      return curState - states[curStateVector] + 1;
    }

    /// \brief Return the initial state for this NFA.
    ///
    /// This initial state is the begining of a linked list
    /// of NFA::Split states which each individualy point to
    /// their associated (sub)NFA used to recognize a single Token.
    State *getNFAStartState() { return nfaStartState; }

  private:
    /// \brief The vector of blocks of pre allocated NFA::State(s).
    State **states;

    /// \brief The start of the linked list of start states of the
    /// various token recognizing (sub)NFA(s).
    ///
    /// The first NFA::State in this linked list is the start state
    /// of the whole NFA.
    State *nfaStartState;

    /// \brief The last start state in the linked list of NFA::State(s).
    State *nfaLastStartState;

    /// \brief The currently unassigned NFA::State in the current block
    /// of NFA::State(s).
    State *curState;

    /// \brief The last allocatable NFA::State in the current block of
    /// NFA::State(s).
    State *lastState;

    /// \brief The index in the vector of NFA::State blocks which is
    /// currently being allocated.
    size_t curStateVector;

    /// \brief The number of possible NFA::State blocks which the current
    /// vector of blocks, states, can hold.
    size_t numStateVectors;

    /// \brief The Classifier used by this NFA to classify UTF8 characters.
    Classifier *utf8Classifier;
};


#endif
