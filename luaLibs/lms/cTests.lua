-- A Lua script

-- This lms Lua script creates the targets required to build, diff or 
-- install a suite of Contest cTest based unit tests. 

require 'lms.c'

cTests     = cTests     or { }
lms.cTests = lms.cTests or { }

local cTestsDefaults = {
  cTestsIncDir =
    makePath{
      getEnv('HOME'), 'texmf', 't-contests', 'tex',
      'context', 'third', 'contests'
    },
  cTestsStartup = 't-contests-cTests.lua'
}

cTests = hMerge(cTestsDefaults, lms.cTests, cTests)

function cTests.targets(defaultDef, cDef)

  cDef = hMerge(defaultDef, cDef or { })

  cDef.dependencies = { }
  tInsert(cDef.docFiles, 1, cDef.mainDoc)
  for i, aDocFile in ipairs(cDef.docFiles) do
    tInsert(cDef.dependencies, makePath{ cDef.docDir, aDocFile })
  end

  local mainCFile = (cDef.mainDoc):gsub('%.tex', '.c')
  local mainCFileIndex = -1
  for i, aCFile in ipairs(cDef.cCodeFiles) do
    if mainCFile == aCFile then mainCFileIndex = i end
  end
  if 0 <= mainCFileIndex then tRemove(cDef.cCodeFiles, mainCFileIndex) end

  for i, aTestExec in ipairs(cDef.testExecs) do
  
    local srcTarget = makePath{ cDef.buildDir, aTestExec..'.c' }
    tInsert(buildTargets, srcTarget)
    target(hMerge(cDef, {
      target  = srcTarget,
      command = litProgs.compileLitProg
    }))
    
    local testExecTarget = makePath{ cDef.buildDir, aTestExec }
    local cDependencies = { }
    cDef.srcFiles = aAppend(cDef.cHeaderFiles, cDef.cCodeFiles)
    tInsert(cDependencies, srcTarget)
    for j, aSrcFile in ipairs(cDef.srcFiles) do
      tInsert(cDependencies, makePath{ cDef.buildDir, aSrcFile })
    end
    local pDef = hMerge(c, cDef)
    tInsert(pDef.cOpts, tConcat({
      '-DCTESTS_STARTUP=\\"',
      makePath{
        cTests.cTestsIncDir,
        cTests.cTestsStartup
      },
      '\\"'
    }, ''))
    tInsert(pDef.cIncs, 1, '-I'..cTests.cTestsIncDir)
    tInsert(pDef.cIncs, 1, tConcat{
      '-I',
      makePath{
        getEnv('HOME'),
        '.joylol',
        'include'
      }
    })
    if cDef.testLibDirs then
      for j, aLibDir in ipairs(cDef.testLibDirs) do
        aLibDir = aLibDir:gsub('<HOME>', getEnv('HOME'))
        tInsert(pDef.libs, tConcat({
          '-L',
          aLibDir
        }, ''))
      end
    end
    if cDef.testLibs then
      for j, aLib in ipairs(cDef.testLibs) do
        aLib = aLib:gsub('<HOME>', getEnv('HOME'))
        if aLib:match('^-l') then
          tInsert(pDef.libs, aLib)
        else
          tInsert(pDef.libs, aLib..'.so')
        end
      end
    end
    c.program(hMerge(pDef, {
      target       = testExecTarget,
      dependencies = c.collectCDependencies(cDependencies),
      needs        = { 'lua5.2' },
    }))
    
    local testTarget = makePath{ cDef.buildDir, aTestExec..'-results.lua' }
    tInsert(testTargets, testTarget)
    target(hMerge(cDef, {
      target       = testTarget,
      dependencies = { testExecTarget },
      command      = testExecTarget
    }))
    tInsert(cleanTargets, nameCleanTarget(srcTarget))
  end
  
end

