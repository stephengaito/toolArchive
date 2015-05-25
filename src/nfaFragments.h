#ifndef LEXER_H
#define LEXER_H

#include "nfa.h"

/// \brief NFAFragments represents a partial specified NFA containing
/// one or more NFA::States.
///
/// In particular NFAFragments::Frag structures represent an NFA which
/// has yet to be defined output connections. Otherwise all NFA::States
/// are fully defined.
///
/// NFAFragments::Frag structures are pushed and poped from a stack.
class NFAFragments {

  public:

    /// \brief Create an instance of NFAFragments.
    NFAFragments(NFA *nfa, size_t reLen);

    /// \brief Destroy an instance of NFAFragments.
    ~NFAFragments();

    /// \brief Push an NFAFragments::Frag structure containing an
    /// NFA::State suitable to check a given UTF8 character.
    void checkCharacter(utf8Char_t aChar) throw (LexerException);

    /// \brief Push an NFAFragments::Frag structure containing an
    /// NFA::State suitable to check that the current UTF8 character
    /// is a member of a given Classifier::classSet_t.
    void checkClassification(Classifier::classSet_t aClass)
    throw (LexerException);

    /// \brief Push an NFAFragments::Frag structure containing an
    /// NFA::State suitable to check one successor NFA::State.
    void concatenate(void) throw (LexerException);

    /// \brief Push an NFAFragments::Frag structure containing an
    /// NFA::State suitable to check two possible successor NFA::States.
    void alternate(void) throw (LexerException);

    /// \brief Push an NFAFragments::Frag structure containing an
    /// NFA::State suitable to check zero or one previous NFA::State.
    void zeroOrOne(void) throw (LexerException);

    /// \brief Push an NFAFragments::Frag structure containing an
    /// NFA::State suitable to check zero or more previous NFA::State.
    void zeroOrMore(void) throw (LexerException);

    /// \brief Push an NFAFragments::Frag structure containing an
    /// NFA::State suitable to check one or more previous NFA::State.
    void oneOrMore(void) throw (LexerException);

    /// \brief Push an NFAFragments::Frag structure containing an
    /// NFA::State which represents a terminal state which recognizes a
    /// token with id, aTokenId.
    NFA::State *match(NFA::TokenId aTokenId) throw (LexerException);

  private:
    /// \brief a Ptrlist is a linked list of NFA::State structures
    /// which the NFAFragments::patch method should patck to provide
    /// a fully specified NFA structure.
    typedef union Ptrlist {
      /// \brief Since the NFA::State out pointers in the Ptrlist are
      /// always uninitialized, we use the pointers themselves
      /// as storage for the Ptrlists.
      Ptrlist *next;
      /// \brief The underlying NFA::State in this Ptrlist.
      NFA::State *s;
    } Ptrlist;

    /// \brief A partially built NFA without the matching nfaState
    /// filled in.
    ///
    /// The NFAFragments::patch method takes a Frag structure and
    /// patches the Frag.out list to the given NFA::State.
    typedef struct Frag {
      /// \brief Frag.start points at the start NFA::State.
      NFA::State *start;
      /// \brief Frag.out is a list of places that need to be set to the
      /// next nfaState for this fragment.
      Ptrlist *out;
    } Frag;

    /// \brief Initialize new Frag structure.
    Frag frag(NFA::State *start, Ptrlist *out);

    /// \brief Create singleton Ptrlist containing just outp.
    Ptrlist* list1(NFA::State **outp);

    /// \brief Patch the Ptrlist of NFA::States at out to point to
    /// the start state of the NFA::State s.
    void patch(Ptrlist *l, NFA::State *s);

    /// \brief Join the two Ptrlists l1 and l2, returning the
    /// combination.
    Ptrlist* append(Ptrlist *l1, Ptrlist *l2);

    /// \brief Push the Frag onto the NFAFragments stack of partial NFA
    /// fragments.
    void push(Frag aFrag) throw (LexerException);

    /// \brief Pop a Frag off the NFAFragments stack of partial NFA
    /// fragments.
    Frag pop(void) throw (LexerException);

  private:
    /// \brief The NFA for which these NFAFragments are being constructed.
    NFA *nfa;

    /// \brief The stack of partially constructed NFAFragments.
    ///
    /// A given regular expression is parsed directly into an impliclit
    /// reverse polish structure using this stack.
    Frag *stack;

    /// \brief The current top of the NFAFragments stack.
    Frag *stackPtr;

    /// \brief The absolute top of the allocated NFAFragments stack.
    Frag *stackEnd;

    /// \brief An instance of NFA::MatchData initialized to represent
    /// no/empty/null match data.
    NFA::MatchData noMatchData;
};


#endif
