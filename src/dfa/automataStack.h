#ifndef AUTOMATA_STACK_H
#define AUTOMATA_STACK_H

#include "automataState.h"

namespace DeterministicFiniteAutomaton {

  class AutomataStack : public VarArray<AutomataState> {

    public:

      bool invariant(void) const {
        if (!VarArray<AutomataState>::invariant()) return false;

        for (size_t i = 0; i < numItems; i++) {
          if (!itemArray[i].invariant()) return false;
        }
        return true;
      }
  };
};


#endif
