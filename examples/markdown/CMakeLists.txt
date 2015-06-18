# This is the cmake description of how to build a dynUtf8Parser based 
# [Markdown](http://daringfireball.net/projects/markdown/syntax) 
# parser.

cmake_minimum_required (VERSION 2.6)
enable_testing()

# Load any user configuration
include("$ENV{HOME}/.cmake/CMakeLists.txt")

setDefaultBuildType("DEBUG" "-DDEBUG")

project (MarkdownParser)
loadLLVM()

setupStandardTargets()

configureDoxygen()
transformDoxygen()

add_subdirectory(lib EXCLUDE_FROM_ALL)
add_subdirectory(bin EXCLUDE_FROM_ALL)
add_subdirectory(tests EXCLUDE_FROM_ALL)
