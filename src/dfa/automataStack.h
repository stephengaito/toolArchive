#ifndef AUTOMATA_STACK_H
#define AUTOMATA_STACK_H

#include "automataState.h"

namespace DeterministicFiniteAutomaton {

  class AutomataStack : public VarArray<AutomataState> {

    public:

      bool invariant(void) const {
        if (!VarArray<AutomataState>::invariant())
          throw AssertionFailure("AutomataState varArray failed invariant");

        for (size_t i = 0; i < numItems; i++) {
          if (!itemArray[i].invariant())
            throw AssertionFailure("AutomataState entry failed invariant");
        }
        return true;
      }
  };
};


#endif
