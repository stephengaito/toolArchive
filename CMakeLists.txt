SET(ENV{LLVM_DIR} "/usr/local/llvm/3.6.0")
SET(CMAKE_INSTALL_PREFIX $ENV{LLVM_DIR} CACHE PATH "my install prefix" FORCE)
SET(ENV{CC} "/usr/local/llvm/3.6.0/bin/clang")
SET(ENV{CXX} "/usr/local/llvm/3.6.0/bin/clang++")

SET(CMAKE_SYSTEM_INSTALL_PATHS $ENV{LLVM_DIR} CACHE STRING "CMAKE LIST of SYSTEM intall directories" FORCE)


# Some additional macros:

# Create a list of subdirectories
#
# param: <name> of the variable in which to return the result.
#
# param: <value> of the directory whose subdirectories you are 
#        interested in.
#
# see: [CMake : How to get the name of all subdirectories of a 
# directory?](http://stackoverflow.com/a/7788165)
#
# NOTES: you MUST NOT use commas in the macro call!!!!
#
#MACRO(SUBDIRLIST result curdir)
#  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
#  SET(dirlist "")
#  FOREACH(child ${children})
#    IF(IS_DIRECTORY ${curdir}/${child})
#        LIST(APPEND dirlist " ${curdir}/${child}")
#    ENDIF()
#  ENDFOREACH()
#  SET(${result} ${dirlist})
#ENDMACRO()
