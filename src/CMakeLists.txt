# This is the cmake description of how to build the dynamicUTF8Parser 
# sources as a subproject.

project(dynUtf8Parser)

defineLibrary(dynUtf8Parser "src" "HAT-trie/src" "src")
add_dependencies(src dynUtf8Parser hattrie)
