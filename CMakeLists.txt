# This is the cmake description of how to build the llvm based 
# dynamicUTF8Parser

cmake_minimum_required (VERSION 2.6)
enable_testing()

# Load any user configuration
include("$ENV{HOME}/.cmake/CMakeLists.txt")

# Load our utilities
include("CMakeUtils.txt")

project (DynamicUTF8Parser)

loadLLVM()

add_subdirectory(HAT-trie)
