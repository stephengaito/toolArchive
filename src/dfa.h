#ifndef DFA_H
#define DFA_H

#include <stdio.h>
#include "nfa.h"

/// \brief The DFA class is used to interpret a given NFA.
///
/// Directly inrepreting a given NFA typically requires backtracking
/// when a given NFA::State is found which has not currently valid
/// successor state for a given string.
///
/// Directly interpreting a given DFA does not require any backtracking.
/// If a given DFA::DState has no currently valid successor states, then
/// the DFA as a whole does not recognize the give string.
///
/// Every NFA can be compiled into a corresponding DFA.
///
/// This DFA class provides the methods to compile the associated NFA
/// into a DFA on the fly. Essentially the next DFA::DState is a set of
/// NFA::States which could be successor states of the current set of
/// NFA::States represented as the current DFA::DState.
class DFA {
  public:

    typedef struct Token {
      NFA::tokenId_t tokenId;
      Utf8Chars *baseStream;
      utf8Char_t *startChar;
      size_t     length;
    } Token;

    static Token *nullToken;

    /// \brief Represents a DFA state as a bit set of NFA states.
    ///
    /// For a given NFA, the number of NFA::States is known, so that
    /// the required bit sets have known sizes.
    typedef char DState;

    /// \brief Create a DFA object corresponding to a given NFA.
    DFA(NFA *anNFA);

    /// \brief Destroy the DFA object.
    ~DFA(void);

    /// \brief Run the DFA until the next token is recognized.
    Token *getNextToken(Utf8Chars *utf8Stream);

  private:
    /// \brief The NFA associated to this DFA.
    NFA *nfa;

    /// \brief A vector of known NFA::States.
    ///
    /// This vector provides an integer to NFA::State mapping.
    ///
    /// Note that for a given NFA, the number of NFA::States is fixed,
    /// so the length of this vector if fixed when the DFA is created.
    NFA::State **int2nfaStatePtr;

    /// \brief The size of the int2nfaStatePtr vector.
    size_t int2nfaStatePtrSize;

    /// \brief The number of *currently* "known" NFA::State(s).
    ///
    /// The "known" NFA::State(s) are those that have been reached,
    /// while compiling the DFA on the fly, from the NFA start state.
    size_t numKnownNFAStates;

    /// \brief The Hat-Trie based nfaStatePtr to integer mapping.
    ///
    /// This is the inverse mapping to the int2nfaStatePtr mapping.
    hattrie_t   *nfaStatePtr2int;

    /// \brief The Hat-Trie based next DFA::DState mapping.
    ///
    /// This mapping is used both to register the known DFA::DState(s),
    /// as well as record any successor DFA::DState(s) for a given
    /// DFA::DState + {character | classSet_t} combination.
    hattrie_t   *nextDFAStateMap;

    /// \brief The number of bytes in a DFA::DState.
    ///
    /// For a given NFA, this is a fixed number, computed when
    /// the DFA is created.
    size_t dfaStateSize;

    /// \brief The maximum size of a probe into the nextDFAStateMap
    /// mapping.
    size_t dfaStateProbeSize;

    /// \brief Some (central-pre-DFA) storage for probing the
    /// nextDFAStateMap.
    char  *dfaStateProbe;

    /// \brief The vector of blocks of pre-allocated DFA::DState(s).
    DState **dStates;

    /// \brief The initial starting state for this DFA.
    DState *dfaStartState;

    /// \brief The bit set of all known NFA::State(s) which are
    /// NFA::token recognizing states.
    ///
    /// This bit set is used to determine if/when a token has been
    /// recognized.
    DState *tokensDState;

    /// \brief The currently unassigned DFA::DState in the current
    /// block of DFA::DState(s).
    DState *curAllocatedDState;

    /// \brief The last allocatable DFA::DState inthe current block of
    /// DFA::DState(s).
    DState *lastDState;

    /// \brief One of three allocated but currently unused DFA::DState(s).
    ///
    /// Since this DFA::DState is unused it can be re-allocated as needed.
    DState *allocatedUnusedDState0;

    /// \brief One of three allocated but currently unused DFA::DState(s).
    ///
    /// Since this DFA::DState is unused it can be re-allocated as needed.
    DState *allocatedUnusedDState1;

    /// \brief One of three allocated but currently unused DFA::DState(s).
    ///
    /// Since this DFA::DState is unused it can be re-allocated as needed.
    DState *allocatedUnusedDState2;

    /// \brief The index in the vector of DFA::DState blocks which is
    /// currently being allocated.
    size_t curDStateVector;

    /// \brief The number of possible DFA::DState blocks which the current
    /// vector of DFA::DState blocks can hold.
    size_t numDStateVectors;

    /// \brief The number of bytes in a any DFA::DState block.
    size_t dStateVectorSize;

    /// \brief Allocate a DFA::DState.
    ///
    /// This DFA::DState may come from either one of the allocated but
    /// but unused DFA::DState(s) *or* from one of the currently unallocated
    /// DFA::DState in the current block of DFA::DState(s).
    DState *allocateANewDState(void);

    /// \brief Mark an allocated DFA::DState as unused and store it in
    /// one of the allocated but unused DFA::DState(s).
    void unallocateADState(DState *aDFAState);

    /// \brief Ensure that all bits in the DFA::DState bit set are
    /// turned *off*.
    void emptyDState(DState *d);

    /// \brief Return true if the DFA::DState bit set is empty.
    bool isEmptyDState(DState *d);

    /// \brief Return true if the DFA::DState bit set d1 is a *subset*
    /// of the DFA::DState bit set d2.
    bool isSubDState(DState *d1, DState *d2); // d1 subsetOf d2

    /// \brief Return the union of the two DFA::DState bit sets,
    /// mergeInto and other, in the DFA::DState bit set mergeInto.
    void mergeDStates(DState *mergeInto, DState *other);

    /// \brief Print the bytes of the DFA::DState d on the FILE filePtr
    /// together with the message message.
    void printDState(FILE *filePtr, const char* message, DState *d);

    /// \brief Return true if the DFA::DState contains a token
    /// recognizing NFA::State.
    bool matchesToken(DState *dState);

    /// \brief The NFAStateNumber structure represents a single
    /// bit in the DFA::DState bit set.
    ///
    /// The bit speficied by an NFAStateNumber, corresponds to a given
    /// NFA::State in the nfaStatePtr2int mapping.
    typedef struct NFAStateNumber {
      /// \brief The byte which contains this NFA::State bit.
      size_t  stateByte;
      /// \brief The bit which represents this NFA::State.
      uint8_t stateBit;
    } NFAStateNumber;

    /// \brief Return the NFAStateNumber corresponding to the given
    /// NFA::State.
    ///
    /// This method uses the nfaStatePtr2int mapping.
    NFAStateNumber getNFAStateNumber(NFA::State *nfaState)
      throw (LexerException);

    /// \brief Add the NFA::State to the DFA::DState bit set by
    /// following unlabeled (NFA::Split) transitions.
    void addNFAStateToDFAState(DState *dfaState, NFA::State *nfaState);

    /// \brief Copy the DFA::DState bytes into the dfaStateProbe array.
    void assembleDFAStateProbe(DState *dfaState);

    /// \brief Copy the DFA::DState bytes followed by the bytes in
    /// the utf8Char_t structure into the dfaStateProbe array.
    void assembleDFAStateCharacterProbe(DState *dfaState,
                                        utf8Char_t curChar);

    /// \brief Copy the DFA::DState bytes followed by the bytes in
    /// the Classifier::classSet_t structure into the dfaStateProbe
    /// array.
    void assembleDFAStateClassificationProbe(DState *dfaState,
      Classifier::classSet_t classification);

    /// \brief Register the DFA::DState to ensure all DFA::DState bit
    /// sets use the *same* in memory object.
    ///
    /// This registration process makes use of the nextDFAStateMap.
    DState *registerDState(DState *dfaState);

    /// \brief Compute the initial DFA::DState bit set which represents
    /// the start state of this DFA.
    ///
    /// The DFA::DState start state is the bit set of all NFA::State(s)
    /// reachable from the NFA::nfaStartState by following unlabeled
    /// (NFA::Split) transitions.
    void computeDFAStartState(void);

    /// \brief Find or compute the next DFA::DState given a
    /// utf8Char_t character or a Classifier::classSet_t.
    ///
    /// Start by trying to find the nextDFAStateMap find the next
    /// DFA::DState corresponding to either a specific
    /// DFA::DState/utf8Char_t or generic
    /// DFA::DState/Classifier::classSet_t combination.
    ///
    /// If no such (pre-compiled) next DFA::State can be found in the
    /// existing mapping, step the NFA from the states in the
    /// DFA::DState, oldState, bit set using the transitions across
    /// either the UTF8 character, c, or the Classifier::classSet_t,
    /// classifiactionSet, creating and registering a new DFA::DState
    /// bit set.
    DState *computeNextDFAState(DState *oldState,
                                utf8Char_t c,
                                Classifier::classSet_t classificationSet);

};

#endif
