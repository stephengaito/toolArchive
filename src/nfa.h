#ifndef NFA_H
#define NFA_H

#include <exception>
#include "classifier.h"

class LexerException: public std::exception {
  public:
    LexerException(const char* aMessage) { message = aMessage; };
    const char* message;
};

class NFA {

  public:
    NFA(Classifier *aUTF8Classifier);
    ~NFA(void);
    /*
     * Represents an NFA state plus zero or one or two arrows exiting.
     * if c == Match, no arrows out; matching state.
     * If c == Split, unlabeled arrows to out and out1 (if != NULL).
     * If c < 256, labeled arrow with character c to out.
     */
    enum MatchType {
      Empty     = 0,
      Character = 1,
      ClassSet  = 2,
      Split     = 3,
      Token     = 4
    };
    typedef  union MatchData {
        utf8Char_t c;
        classSet_t s;
        value_t    t;
      } MatchData;
    typedef struct State {
      MatchType matchType;
      MatchData matchData;
      State *out;
      State *out1;
    } State;

    Classifier *getClassifier(void) { return utf8Classifier; }

    void  compileRegularExpression(const char *re) throw (LexerException) ;
    State *addState(MatchType aMatchType, MatchData someMatchData,
                          State *out, State *out1)
                          throw (LexerException);
    void preAddStates(size_t reLength);
    size_t getNumberStates() {
      return curState - states[curStateVector] + 1;
    }
    State *getNFAStartState() { return nfaStartState; }


  private:
    State **states;
    State *nfaStartState;
    State *curState;
    State *lastState;
    size_t curStateVector;
    size_t numStateVectors;
    Classifier *utf8Classifier;
};


#endif
