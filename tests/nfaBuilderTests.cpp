#include <bandit/bandit.h>
using namespace bandit;

#include <string.h>
#include <stdio.h>
#include <exception>

#ifndef protected
#define protected public
#endif

#include <dynUtf8Parser/nfaBuilder.h>

go_bandit([](){

  /// \brief Test the NFAFragments class.
  ///
  /// There are no explicit NFAFragments tests.
  ///
  /// The NFAFragments class is (currently) tested by the NFA_Tests.
  describe("NFABuilder", [](){
  });

});
