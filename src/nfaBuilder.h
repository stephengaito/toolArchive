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
/// NFABuilder::Frag structures are pushed and poped from a stack in
/// the given NFABuilder instance.
///

/// The implementation of this NFABuilder class has heavily modified
/// from code taken from Russ Cox's [Implementing Regular
/// Expressions](https://swtch.com/~rsc/regexp/) page. His code was in
/// turn inspired by the paper by Thompson, Ken.  Regular Expression
/// Search Algorithm, Communications of the ACM 11(6) (June 1968), pp.
/// 419-422.
///
/// This code is
///
///> Copyright (c) 2007 Russ Cox.
///
/// Extensive modifications for use as a utf8 parser compiled by clang
/// are
///
///> Copyright (c) 2015 Stephen Gaito
///
/// This code can be distributed under the MIT license:
///
///> Permission is hereby granted, free of charge, to any person
///> obtaining a copy of this software and associated
///> documentation files (the "Software"), to deal in the
///> Software without restriction, including without limitation
///> the rights to use, copy, modify, merge, publish, distribute,
///> sublicense, and/or sell copies of the Software, and to
///> permit persons to whom the Software is furnished to do so,
///> subject to the following conditions:
///>
///> The above copyright notice and this permission notice shall
///> be included in all copies or substantial portions of the
///> Software.
///>
///> THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
///> KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
///> WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
///> PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS
///> OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
///> OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
///> OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
///> SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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
    /// token with id, aTokenId. This token will be matched but not
    /// inserted into the parse tree if ignoreToken is true.
    NFA::State *match(Token::TokenId aTokenId,
                      const char *startStateName,
                      bool ignoreToken = false);

    /// \brief Compile the Regular Expression into a (sub)NFA used to
    /// recognize a Token with Token ID aTokenId with the given
    /// startStateName. The matched token will not be inserted into the
    /// parse tree if ignoreToken if false.
    ///
    /// This regular expression implementation supports the standard
    /// operators '(' '|' ')' '*' '+' '?', as well as the local
    /// extensions:
    ///* '[' <className> ']' to specify a UTF8 character class which
    ///  has been registered with the Classifier associated with the NFA.
    ///  (The class set is complemented if the class name is prepended
    ///  with an '!').
    ///* '{' <startStateName> '}' to specify the name of a rule's
    ///  startState at which recognition should restart in the
    ///  associated PushDownMachine.
    ///* '\' (or '\\\\' inside double quotes) are used to escape the next
    ///  character.
    ///
    /// Any other character (including a ' ' space) is assumed to be
    /// part of the regular expression to be recognized.
    ///
    void compileRegularExpressionForTokenId(const char *startStateName,
                                            const char *re,
                                            Token::TokenId aTokenId,
                                            bool ignoreToken = false)
                                            throw (ParserException);

  protected:

    /// \brief a Ptrlist is a linked list of NFA::State structures
    /// which the NFABuilder::patch method should patch to provide
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

  protected:

    /// \brief The NFA for which this NFABuilder is being constructed.
    NFA *nfa;

    /// \brief The stack of partially constructed NFABuilder.
    ///
    /// A given regular expression is parsed directly into an impliclit
    /// reverse polish structure using this stack.
    VarArray<Frag> stack;

    /// \brief An instance of NFA::MatchData initialized to represent
    /// no/empty/null match data.
    NFA::MatchData noMatchData;
};


#endif
