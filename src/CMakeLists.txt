SET(ENV{LLVM_DIR} "/usr/local/llvm/3.6.0")

SET(ENV{CC} "/usr/local/llvm/3.6.0/bin/clang")
SET(ENV{CXX} "/usr/local/llvm/3.6.0/bin/clang++")

SET(CMAKE_INSTALL_PREFIX $ENV{LLVM_DIR} 
    CACHE PATH "Local correction to the install prefix" FORCE)

SET(CMAKE_SYSTEM_INSTALL_PATHS $ENV{LLVM_DIR}
    CACHE STRING "CMAKE LIST of SYSTEM intall directories" FORCE)

# Load local utilities
include("$ENV{HOME}/.cmake/CMakeUtils.txt")


