#include <bandit/bandit.h>
using namespace bandit;

#include <string.h>
#include <stdio.h>
#include <exception>

#ifndef private
#define private public
#endif

#include <parseTrees.h>


go_bandit([](){

  printf("\n----------------------------------\n");
  printf(  "ParseTrees\n");
  printf(  "     Token = %zu bytes (%zu bits)\n", sizeof(ParseTrees::Token), sizeof(ParseTrees::Token)*8);
  printf(  "ParseTrees = %zu bytes (%zu bits)\n", sizeof(ParseTrees), sizeof(ParseTrees)*8);
  printf(  "----------------------------------\n");

  /// \brief We test the correctness of the ParseTrees class.
  describe("ParseTrees", [](){

    it("Should be able to create some parse trees", [](){
      ParseTrees *aForest = new ParseTrees();
      AssertThat(aForest, Is().Not().EqualTo((void*)0));
      AssertThat(aForest->tokenAllocator, Is().Not().EqualTo((void*)0));
      AssertThat(aForest->streams,        Is().Not().EqualTo((void*)0));
      delete aForest;
    });

  }); // describe ParseTrees

});
