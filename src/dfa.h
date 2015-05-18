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

    /// Run the DFA until the next token is recognized.
    bool getNextToken(Utf8Chars *utf8Stream);

  private:
    NFA *nfa;
    Classifier  *utf8Classifier;
    hattrie_t   *nfaStatePtr2int;
    NFA::State **int2nfaStatePtr;
    size_t numKnownNFAStates;

    hattrie_t   *nextDFAStateMap;

    size_t dfaStateSize;
    size_t dfaStateProbeSize;
    char  *dfaStateProbe;

    DState **dStates;
    DState *dfaStartState;
    DState *tokensDState;
    DState *curAllocatedDState;
    DState *lastDState;
    DState *allocatedUnusedDState0;
    DState *allocatedUnusedDState1;
    DState *allocatedUnusedDState2;
    size_t curDStateVector;
    size_t numDStateVectors;
    size_t dStateVectorSize;

    DState *allocateANewDState(void);
    void unallocateADState(DState *aDFAState);

    void emptyDState(DState *d);
    bool isEmptyDState(DState *d);
    bool notEqualDStates(DState *d1, DState *d2);
    void mergeDStates(DState *mergeInto, DState *other);

    /// Check whether DFA state contains a token.
    bool matchesToken(DState *dState);

    typedef struct NFAStateNumber {
      size_t  stateByte;
      uint8_t stateBit;
    } NFAStateNumber;

    NFAStateNumber getNFAStateNumber(NFA::State *nfaState);

    /* Add s to l, following unlabeled arrows. */
    void addNFAStateToDFAState(DState *dfaState, NFA::State *nfaState);

    void assembleDFAStateProbe(DState *dfaState);
    void assembleDFAStateCharacterProbe(DState *dfaState,
                                        utf8Char_t curChar);
    void assembleDFAStateClassificationProbe(DState *dfaState,
                                             classSet_t classification);
    DState *registerDState(DState *dfaState);

    void computeDFAStartState(void);

    /*
     * Step the NFA from the states in clist
     * past the character c,
     * to create next NFA state set nlist.
     */
    DState *computeNextDFAState(DState *oldState,
                                utf8Char_t c,
                                classSet_t classificationSet);

};

#endif
