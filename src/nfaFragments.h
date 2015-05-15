#ifndef LEXER_H
#define LEXER_H

#include "nfa.h"

class NFAFragments {
  public:
    NFAFragments(NFA *nfa, size_t reLen);
    ~NFAFragments();

    void checkCharacter(utf8Char_t aChar);

    void concatenate(void);

    void alternate(void);

    void zeroOrOne(void);

    void zeroOrMore(void);

    void oneOrMore(void);

    NFA::State *match(void) throw (LexerException*);

  private:
    /*
     * Since the out pointers in the list are always
     * uninitialized, we use the pointers themselves
     * as storage for the Ptrlists.
     */
    typedef union Ptrlist {
      Ptrlist *next;
      NFA::State *s;
    } Ptrlist;

    /*
     * A partially built NFA without the matching nfaState filled in.
     * Frag.start points at the start nfaState.
     * Frag.out is a list of places that need to be set to the
     * next nfaState for this fragment.
     */
    typedef struct Frag {
      NFA::State *start;
      Ptrlist *out;
    } Frag;

    /* Initialize Frag struct. */
    Frag frag(NFA::State *start, Ptrlist *out);

    /* Create singleton list containing just outp. */
    Ptrlist* list1(NFA::State **outp);

    /* Patch the list of nfaStates at out to point to start. */
    void patch(Ptrlist *l, NFA::State *s);

    /* Join the two lists l1 and l2, returning the combination. */
    Ptrlist* append(Ptrlist *l1, Ptrlist *l2);

    void push(Frag aFrag) throw (LexerException*) ;

    Frag pop(void) throw (LexerException*) ;

  private:
    NFA *nfa;
    Frag *stack;
    Frag *stackPtr;
    Frag *stackEnd;
    NFA::MatchData noMatchData;
};


#endif
