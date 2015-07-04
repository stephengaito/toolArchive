#include <string.h>
#include <stdio.h>

#include <cUtils/specs/specs.h>

#ifndef protected
#define protected public
#endif

#include <hattrie/hat-trie.h>

/// \brief Test the existing Hat-Trie code to ensure our use patterns work.
///
/// While Hat-Trie comes with its own tests, we need to
/// ensure some of the subtle ways in which we want to use
/// a hat-trie map are supported by the existing code.
describe(HatTrie) {

  hattrie_t *hatTrieBase = hattrie_create();
  specUValue(hattrie_sizeof(hatTrieBase));
  hattrie_free(hatTrieBase);
  specSize(value_t);

  /// Ensure that a Hat-Trie map can support overlapping keys.
  ///
  /// The DFA part of the Lexer, makes use of structured keys. Each key
  /// is formed of the DFA state plus either a UTF8 character or a
  /// classification. We need to ensure we register a *single* unique
  /// representation of a DFA state as well as the next DFA state given
  /// the current DFA state and either a character or a character
  /// classification. These structured keys are overlapping.
  ///
  /// This test ensures the current Hat-Trie code supports overlapping
  /// keys.
  it("hattrie_get should be able to manage overlaping keys") {
    hattrie_t * hatTrie = hattrie_create();
    const char* prefixStr = "this";
    const char* fullStr   = "this is a test";
    value_t *prefix = hattrie_get(hatTrie, prefixStr, strlen(prefixStr));
    shouldNotBeNULL(prefix);
    value_t *full   = hattrie_get(hatTrie, fullStr, strlen(fullStr));
    shouldNotBeNULL(full);
    shouldNotBeEqual(prefix, full);
    value_t *prefix2 = hattrie_get(hatTrie, prefixStr, strlen(prefixStr));
    shouldBeEqual(prefix, prefix2);
    hattrie_free(hatTrie);
  } endIt();

} endDescribe(HatTrie);
