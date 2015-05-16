#ifndef DFA_H
#define DFA_H

#include "nfa.h"

class DFA {
  public:
    /*
     * Represents a DFA state: a cached NFA state list.
     */
    typedef char DState;

    DFA(NFA *anNFA, Classifier *aUTF8Classifier);
    ~DFA(void);

  private:
    NFA *nfa;
    Classifier  *utf8Classifier;
    hattrie_t   *nfaStatePtr2int;
    NFA::State **int2nfaStatePtr;
    size_t numKnownNFAStates;

    hattrie_t   *knownDFAStatesMap; // consider folding these two into one
    hattrie_t   *nextDFAStateMap;

    size_t dfaStateSize;
    size_t dfaStateProbeSize;
    char  *dfaStateProbe;

    DState **dStates;
    DState *dfaStartState;
    DState *tokensDState;
    DState *curAllocatedDState;
    DState *lastDState;
    size_t curDStateVector;
    size_t numDStateVectors;


//    List l1, l2;

    void allocateANewDState(void);
    void unallocateLastAllocatedDState(void);

    void registerDState(DState *dfaState);

    void emptyDState(DState *d);
    bool notEqualDStates(DState *d1, DState *d2);
    void mergeDStates(DState *mergeInto, DState *other);

    typedef struct NFAStateNumber {
      size_t  stateByte;
      uint8_t stateBit;
    } NFAStateNumber;

    NFAStateNumber getNFAStateNumber(NFA::State *nfaState);

    /* Check whether state list contains a match. */
    bool matchesToken(DState *dState);

    /* Add s to l, following unlabeled arrows. */
    void addNFAStateToDFAState(DState *dfaState, NFA::State *nfaState);

    void assembleDFAStateProbe(DState *dfaState);
    void assembleDFAStateCharacterProbe(DState *dfaState,
                                        utf8Char_t curChar);
    void assembleDFAStateClassificationProbe(DState *dfaState,
                                             classSet_t classification);

    /*
     * Step the NFA from the states in clist
     * past the character c,
     * to create next NFA state set nlist.
     */
    DState *computeNextDFAState(DState *oldState,
                                utf8Char_t c,
                                classSet_t classificationSet);

    /* Free the tree of states rooted at d. */
//    void freestates(DState *d);

    /* Throw away the cache and start over. */
    void freecache(void);

    /*
     * Return the cached DState for list l,
     * creating a new one if needed.
     */
    DState* addDState(DState *l, DState **nextp);

    void startNFA(NFA::State *nfaStartState, DState *dfaState);

    /* Compute initial state list */
    DState* startList(NFA::State *start, DState *l);

    void computeDFAStartState(void);

    DState* nextstate(DState *d, utf8Char_t c);

    /* Run DFA to determine whether it matches s. */
    bool match(DState *start, const char *s);
};

#endif
