# This is the cmake description of how to build the dynamicUTF8Parser 
# tests as a subproject.

project(dynUtf8ParserTests)

defineTestsFor( dynUtf8Parser "lib;HAT-trie/src" "dynUtf8Parser;hattrie" )
add_dependencies(tests dynUtf8ParserTests)

