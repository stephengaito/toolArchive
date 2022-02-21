# Parallel ConTeXt

A very crude, proof of concept, attempt at type setting large 
[ConTeXt](https://www.contextgarden.net) documents in parallel. 

## [Lua](https://www.lua.org/)

While ConTeXt embedds a copy of Lua version 5.4, at the moment we need Lua 
v5.4 to be installed as part of the operating system.

Since we are explicitly running multiple copies of ConTeXt in parallel in 
our `typesetParallelDocs` Lua script, we need the [LuaPosix 
extension](https://github.com/luaposix/luaposix) which is not currently 
included in the Lua embedded in ConTeXt. 

Our `compileParallelTUC` and `typesetParallelDocs` Lua scripts require the 
following packages: 

1. Lua (version 5.4)
2. LuaFileSystem
3. LuaPosix
4. LuaPPrint

To download and install these packages, you will need wget, 
sudo/administrative privilages, an ANSI-C compiler as well as a `make` 
command line tool. 

You can install a copy of the required Lua extensions by typing:

```
  ./installingLua/instalLua
```
