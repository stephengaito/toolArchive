#include <bandit/bandit.h>
using namespace bandit;

#include <string.h>
#include <stdio.h>

#include <hat-trie.h>

go_bandit([](){

  printf("\n----------------------------------\n");
  printf(  "Hat-Trie\n");
  printf(  "  value_t = %lu bytes (%lu bits)\n", sizeof(value_t),   sizeof(value_t)*8);
  printf(  "----------------------------------\n");

  describe("Hat-Trie", [](){

    it("hattrie_get should be able to manage overlaping keys", [&](){
      hattrie_t * hatTrie = hattrie_create();
      const char* prefixStr = "this";
      const char* fullStr   = "this is a test";
      value_t *prefix = hattrie_get(hatTrie, prefixStr, strlen(prefixStr));
      AssertThat(prefix, Is().Not().EqualTo((void*)0));
      value_t *full   = hattrie_get(hatTrie, fullStr, strlen(fullStr));
      AssertThat(full, Is().Not().EqualTo((void*)0));
      AssertThat(prefix, Is().Not().EqualTo(full));
      value_t *prefix2 = hattrie_get(hatTrie, prefixStr, strlen(prefixStr));
      AssertThat(prefix, Is().EqualTo(prefix2));
    }); // test hattrie_get

  }); // Hat-Trie

});
