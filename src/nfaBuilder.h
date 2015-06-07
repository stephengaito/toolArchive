#ifndef NFA_BUILDER_H
#define NFA_BUILDER_H

#include "nfa.h"

/// \brief An NFABuilder represents a partially specified NFA
/// containing one or more NFA::State(s).
///
/// In particular NFABuilder::Frag structures represent an NFA which
/// has yet to be defined output connections. Otherwise all NFA::State(s)
/// are fully defined.
///
/// NFABuilder::Frag structures are pushed and poped from a stack.
class NFABuilder {

  public:

    /// \brief Create an instance of NFABuilder.
    NFABuilder(NFA *nfa);

    /// \brief Destroy an instance of NFABuilder.
    ~NFABuilder();

    /// \brief Push an NFABuilder::Frag structure containing an
    /// NFA::State suitable to check a given UTF8 character.
    void checkCharacter(utf8Char_t aChar);

    /// \brief Push an NFABuilder::Frag structure containing an
    /// NFA::State suitable to check that the current UTF8 character
    /// is a member of a given Classifier::classSet_t.
    void checkClassification(Classifier::classSet_t aClass,
                             const char *className);

    /// \brief Push an NFABuilder::Frag structure containing an
    /// NFA::State suitable to recursively (re)start the NFA
    /// at a new start state, returning to the originally pushed down
    /// state when the NFA has recognized the recursive start state.
    void reStart(NFA::StartStateId aStartStateId,
                 const char *reStartStateName);

    /// \brief Push an NFABuilder::Frag structure containing an
    /// NFA::State suitable to check one successor NFA::State.
    void concatenate(void);

    /// \brief Push an NFABuilder::Frag structure containing an
    /// NFA::State suitable to check two possible successor NFA::State(s).
    void alternate(void);

    /// \brief Push an NFABuilder::Frag structure containing an
    /// NFA::State suitable to check zero or one instances of the
    /// previous NFA::State.
    void zeroOrOne(void);

    /// \brief Push an NFABuilder::Frag structure containing an
    /// NFA::State suitable to check zero or more instances of the
    /// previous NFA::State.
    void zeroOrMore(void);

    /// \brief Push an NFABuilder::Frag structure containing an
    /// NFA::State suitable to check one or more instances of the
    /// previous NFA::State.
    void oneOrMore(void);

    /// \brief Push an NFABuilder::Frag structure containing an
    /// NFA::State which represents a terminal state which recognizes a
    /// token with id, aTokenId.
    NFA::State *match(ParseTrees::TokenId aTokenId,
                      const char *startStateName,
                      bool ignoreToken = false);

    /// \brief Compile the Regular Expression into a (sub)NFA used to
    /// recognize a Token with Token ID aTokenId with the given
    /// startStateName.
    void compileRegularExpressionForTokenId(const char *startStateName,
                                            const char *re,
                                            ParseTrees::TokenId aTokenId,
                                            bool ignoreToken = false)
                                            throw (LexerException);

  private:
    /// \brief a Ptrlist is a linked list of NFA::State structures
    /// which the NFABuilder::patch method should patck to provide
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
    /// The NFABuilder::patch method takes a Frag structure and
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

    /// \brief Create a singleton Ptrlist containing just outp.
    Ptrlist* list1(NFA::State **outp);

    /// \brief Patch the Ptrlist of NFA::State(s) at out to point to
    /// the start state of the NFA::State s.
    void patch(Ptrlist *l, NFA::State *s);

    /// \brief Join the two Ptrlists l1 and l2, returning the
    /// combination.
    Ptrlist* append(Ptrlist *l1, Ptrlist *l2);

    /// \brief Push the Frag onto the NFABuilder stack of partial NFA
    /// fragments.
    void push(Frag aFrag) {
      stack.pushItem(aFrag);
    }

    /// \brief Pop a Frag off the NFABuilder stack of partial NFA
    /// fragments.
    Frag pop(void) {
      ASSERT(stack.getNumItems()); // incorrectly matched push/pop
      return stack.popItem();
    }

  private:
    /// \brief The NFA for which this NFABuilder is being constructed.
    NFA *nfa;

    /// \brief The stack of partially constructed NFABuilder.
    ///
    /// A given regular expression is parsed directly into an impliclit
    /// reverse polish structure using this stack.
    VarArray<Frag> stack;
//    Frag *stack;

    /// \brief The current top of the NFABuilder stack.
//    size_t stackTop;

    /// \brief The size of the allocated NFABuilder stack.
//    size_t stackSize;

    /// \brief An instance of NFA::MatchData initialized to represent
    /// no/empty/null match data.
    NFA::MatchData noMatchData;
};


#endif
