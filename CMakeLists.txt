# This is the cmake description of how to build the llvm based 
# dynamicUTF8Parser

# need to implement something like:
# http://stackoverflow.com/questions/5609692/building-tests-with-cmake-while-not-using-ctest?rq=1
#
# and the following for installation:
# http://www.cmake.org/cmake/help/v2.8.12/cmake.html#command:install


cmake_minimum_required (VERSION 2.6)
enable_testing()

# Load any user configuration
include("$ENV{HOME}/.cmake/CMakeLists.txt")

# Load our utilities
include("CMakeUtils.txt")

setDefaultBuildType("DEBUG" "-DDEBUG")

project (DynamicUTF8Parser)
loadLLVM()
configureDoxygen()
transformDoxygen()

add_custom_target(src)
add_custom_target(tests COMMAND dynUtf8ParserTests)

add_subdirectory(HAT-trie EXCLUDE_FROM_ALL)
add_subdirectory(src EXCLUDE_FROM_ALL)
add_subdirectory(tests EXCLUDE_FROM_ALL)
