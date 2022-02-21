# A Lua Make System

lms is a Lua based Make system similar to [Steve J Donovan's 
Lake](https://github.com/stevedonovan/Lake) or Ruby's 
[rake](https://github.com/ruby/rake).

Out of the box, lms knows very little, except how to *recursively* walk 
through a dependency tree looking for out of date artefacts.

For each out of date artefact, lms will either execute the contents of a 
string or will run a lua function.

lms does come with an example collection of lua functions to build 
dependencies for and compile simple ANSI-C programs.

For other tasks you will need to add corresponding dependency generators 
and compilers functions.

lms uses a simple build 
[DSL](https://martinfowler.com/bliki/DomainSpecificLanguage.html) based 
loosely upon that used by [Lake](https://github.com/stevedonovan/Lake).

## Installation

This Lua Make System expects a POSIX-like environment: it currently 
looks for its lua extension modules in the ".luaMakeSystem" directory of 
the user's "HOME" directory as specified by the environment variable 
"HOME".

To install the system on your computer, copy the file "lms" into a 
directory on your executable "path" (such as $HOME/bin) and copy the 
contents of the "lmsLibs" directory to the directory ".luaMakeSystem" 
directory in your "HOME" directory (for example: $HOME/.luaMakeSystem).

You will then be able to run the "lms" command in any directory which 
contains a "lmsfile" containing Lua Make System instructions on how to 
build the document or code.

Alternatively, you can use the "setup" BASH script to automate the 
installation of the Lua Make System.

## Parallelization

*If* you have installed the [luarock](https://luarocks.org/) 
['luv'](https://luarocks.org/modules/creationix/luv), then the lms 
command will automatically parallelize any build steps which call an 
external shell command. In particular this means that all context, gcc 
or clang compilations will potentially be performed in parallel if at 
all possible, making better use of multiple CPUs.

Alternatively if you have not install the 'luv' luarock, compilation 
will still proceed, but will be done strickly sequentially.
