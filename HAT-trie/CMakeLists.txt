# This is the cmake subproject description of how to build a llvm based 
# HAT-Trie library.

cmake_minimum_required (VERSION 2.6)
enable_testing()

project(HATTrie)

defineLibrary(   hattrie  "src" "src")

# We need to describe each test individually
function(hatTrieTest testName testSource)
  add_executable(${testName} 
    "${CMAKE_SOURCE_DIR}/HAT-trie/test/${testSource}" 
    "${CMAKE_SOURCE_DIR}/HAT-trie/test/str_map.h" 
    "${CMAKE_SOURCE_DIR}/HAT-trie/test/str_map.c"
  )
  target_compile_options(${testName} PRIVATE "-I${CMAKE_SOURCE_DIR}/HAT-trie/src")
  target_link_libraries(${testName} "hattrie")
  add_test(${testName} ${testName})
endfunction(hatTrieTest)

hatTrieTest("bench_sorted_iter" "bench_sorted_iter.c")
hatTrieTest("check_ahtable"     "check_ahtable.c")
hatTrieTest("check_hattrie"     "check_hattrie.c")
