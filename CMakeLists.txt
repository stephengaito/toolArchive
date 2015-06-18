# This is the cmake description of how to build the llvm based 
# dynamicUTF8Parser

cmake_minimum_required (VERSION 2.6)
enable_testing()

# Load any user configuration
include("$ENV{HOME}/.cmake/CMakeLists.txt")

setDefaultBuildType("DEBUG" "-DDEBUG")

project (DynamicUTF8Parser)
loadLLVM()

setupStandardTargets()

configureDoxygen()
transformDoxygen()

add_subdirectory(HAT-trie EXCLUDE_FROM_ALL)
add_subdirectory(src EXCLUDE_FROM_ALL)
add_subdirectory(tests EXCLUDE_FROM_ALL)
