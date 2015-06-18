# This is the cmake description of how to build the dynamicUTF8Parser 
# sources as a subproject.

project(dynUtf8Parser)

defineLibrary(dynUtf8Parser "lib" "HAT-trie/src" "lib")
add_dependencies(lib hattrie)
