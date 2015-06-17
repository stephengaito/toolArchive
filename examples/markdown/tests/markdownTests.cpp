#include <bandit/bandit.h>
using namespace bandit;

#include <string.h>
#include <stdio.h>
#include <exception>

#ifndef protected
#define protected public
#endif
#include <markdown.h>

go_bandit([](){

  printf("\n----------------------------------\n");
  printf(  "markdown\n");
  printf(  "Markdown = %zu bytes (%zu bits)\n", sizeof(Markdown), sizeof(Markdown)*8);
  printf(  "----------------------------------\n");

  describe("Markdown", [](){

    it("should create a parser", [](){
      Markdown *markdown = new Markdown();
      AssertThat(markdown, Is().Not().EqualTo((void*)0));
    });

  }); // describe

}); // bandit
