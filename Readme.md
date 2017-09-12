# A Lua Make System

lms is a Lua based Make system similar to [Steve J Donovan's 
Lake](https://github.com/stevedonovan/Lake) or Ruby's 
[rake](https://github.com/ruby/rake).

Out of the box, lms knows very little, except how to *recursively* walk 
through a dependency tree looking for out of date artifacts.

For each out of date artifact, lms will either execute the contents of a 
string or will run a lua function.

lms does come with an example collection of lua functions to build 
dependencies for and compile simple ANSI-C programs.

For other tasks you will need to add corresponding dependency generators 
and compilers functions.

lms uses a simple build 
[DSL](https://martinfowler.com/bliki/DomainSpecificLanguage.html) based 
loosely upon that used by [Lake](https://github.com/stevedonovan/Lake).
