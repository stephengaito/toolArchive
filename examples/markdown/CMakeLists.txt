# This is the cmake description of how to build a dynUtf8Parser based 
# [Markdown](http://daringfireball.net/projects/markdown/syntax) 
# parser.

cmake_minimum_required (VERSION 2.6)
enable_testing()

# Load any user configuration
include("$ENV{HOME}/.cmake/CMakeLists.txt")

# Load our utilities
include("../../CMakeUtils.txt")

setDefaultBuildType("DEBUG" "-DDEBUG")

project (MarkdownParser)
loadLLVM()
configureDoxygen()
transformDoxygen()

add_custom_target(bin)
add_custom_target(src)
add_custom_target(tests COMMAND utf8MarkdownTests)
add_custom_target(release)
add_dependencies(release tests)
add_dependencies(bin src)

add_subdirectory(lib EXCLUDE_FROM_ALL)
add_subdirectory(src EXCLUDE_FROM_ALL)
add_subdirectory(tests EXCLUDE_FROM_ALL)
