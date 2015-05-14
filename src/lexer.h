#ifndef LEXER_H
#define LEXER_H

#include "classifier.h"

class LexerException {
  public:
    LexerException(const char* aMessage) { message = aMessage; };
    const char* message;
};

class Lexer {

  public:
    Lexer(void);
    /*
     * Represents an NFA state plus zero or one or two arrows exiting.
     * if c == Match, no arrows out; matching state.
     * If c == Split, unlabeled arrows to out and out1 (if != NULL).
     * If c < 256, labeled arrow with character c to out.
     */
    enum MatchType { Character, ClassSet, Split, Token };
    typedef  union MatchData {
        utf8Char_t c;
        classSet_t s;
        value_t    t;
      } MatchData;
    typedef struct NFAState {
      MatchType matchType;
      MatchData matchData;
      NFAState *out;
      NFAState *out1;
      //int lastlist;
    } NFAState;

    NFAState *regularExpression2NFA(const char *re) throw (LexerException*) ;

  private:
    NFAState **nfaStates;
    NFAState *curNFAState;
    NFAState *lastNFAState;
    size_t curNFAStateVector;
    size_t numNFAStateVectors;

    void preAddNFAStates(size_t reLength);
    size_t getNumberCurNFAStates() {
      return curNFAState - nfaStates[curNFAStateVector] + 1;
    }

    NFAState *addNFAState(MatchType aMatchType, MatchData someMatchData,
                          NFAState *out, NFAState *out1)
                          throw (LexerException*);

    class NFAFragments {
      public:
        /*
         * Since the out pointers in the list are always
         * uninitialized, we use the pointers themselves
         * as storage for the Ptrlists.
         */
        typedef union Ptrlist {
          Ptrlist *next;
          NFAState *s;
        } Ptrlist;

        /*
         * A partially built NFA without the matching nfaState filled in.
         * Frag.start points at the start nfaState.
         * Frag.out is a list of places that need to be set to the
         * next nfaState for this fragment.
         */
        typedef struct Frag {
          NFAState *start;
          Ptrlist *out;
        } Frag;

        Lexer *lexer;
        Frag *stack;
        Frag *stackPtr;
        Frag *stackEnd;
        MatchData noMatchData;

        NFAFragments(Lexer *lexer, size_t reLen);
        /* Initialize Frag struct. */
        Frag frag(NFAState *start, Ptrlist *out);
        /* Create singleton list containing just outp. */
        Ptrlist* list1(NFAState **outp);
        /* Patch the list of nfaStates at out to point to start. */
        void patch(Ptrlist *l, NFAState *s);
        /* Join the two lists l1 and l2, returning the combination. */
        Ptrlist* append(Ptrlist *l1, Ptrlist *l2);
        void push(Frag aFrag) throw (LexerException*) ;
        Frag pop(void) throw (LexerException*) ;
        void checkCharacter(utf8Char_t aChar);
        void concatenate(void);
        void alternate(void);
        void zeroOrOne(void);
        void zeroOrMore(void);
        void oneOrMore(void);
        NFAState *match(void) throw (LexerException*);
    };

};


#endif
