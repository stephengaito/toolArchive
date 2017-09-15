-- A Lua script

-- This lms Lua script creates the targets required to build, diff or 
-- install a suite of Contests based unit tests. 

require 'lms.c'

contests = contests or { }

local contestsDefaults = {
  contestsIncDir =
    getEnv('HOME')..'/texmf/t-contests/tex/context/third/contests',
}

contests = hMerge(contestsDefaults, contests)

docTargets     = docTargets     or { }
buildTargets   = buildTargets   or { }
installTargets = installTargets or { }
diffTargets    = diffTargets    or { }
testTargets    = testTargets    or { }

function contests.targets(cDef)

  cDef.dependencies = { }
  tInsert(cDef.docFiles, 1, cDef.mainDoc)
  for i, aDocFile in ipairs(cDef.docFiles) do
    tInsert(cDef.dependencies, cDef.docDir..'/'..aDocFile)
  end

  for i, aTestExec in ipairs(cDef.testExecs) do
  
    local srcTarget = cDef.buildDir..'/'..aTestExec..'.c'
    tInsert(buildTargets, srcTarget)
    target(hMerge(cDef, {
      target  = srcTarget,
      command = litProgs.compileLitProg
    }))
    
    local testExecTarget = cDef.buildDir..'/'..aTestExec
    local cDependencies = { }
    tInsert(cDependencies, srcTarget)
    for j, aSrcFile in ipairs(cDef.srcFiles) do
      tInsert(cDependencies, cDef.buildDir..'/'..aSrcFile)
    end
    local pDef = hMerge(c, cDef)
    tInsert(pDef.cIncs, '-I'..contests.contestsIncDir)
    c.program(hMerge(pDef, {
      target       = testExecTarget,
      dependencies = c.collectCDependencies(cDependencies),
      needs        = { 'lua5.2' }
    }))
    
    local testTarget = cDef.buildDir..'/'..aTestExec..'-results.lua'
    tInsert(testTargets, testTarget)
    target(hMerge(cDef, {
      target       = testTarget,
      dependencies = { testExecTarget },
      command      = testExecTarget
    }))
  end
  
end

