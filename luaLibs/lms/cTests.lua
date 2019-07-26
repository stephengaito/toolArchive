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
  cDef.creator = 'cTests-targets'
  
  cDef.dependencies = cDef.dependencies or { }

  for i, aTestExec in ipairs(cDef.testExecs) do

    local cDependencies = { }

    local testSrcTarget = makePath{ cDef.buildDir, aTestExec..'.c' }
    tInsert(cDependencies, testSrcTarget)
    aInsertOnce(codeTargets, testSrcTarget)
    aInsertOnce(buildTargets, testSrcTarget)
    target(hMerge(cDef, {
      target      = testSrcTarget,
      command     = cDef.compileLitProg,
      commandName = 'cDef::compileLitProg ('..cDef.commandName..')'
    }))
    aInsertOnce(cleanTargets, nameCleanTarget(testSrcTarget))
    
    local testExecTarget = makePath{ cDef.buildDir, aTestExec }
    local mainCFile = (cDef.mainDoc):gsub('%.tex', '.c')
    for j, aSrcFile in ipairs(cDef.cSrcFiles) do
      if not aSrcFile:match(mainCFile) then
        tInsert(cDependencies, makePath{ cDef.buildDir, aSrcFile })
      end
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
        -- aLibDir = aLibDir:gsub('<HOME>', getEnv('HOME')) -- OLD
        aLibDir = replaceEnvironmentVarsInPath(aLibDir)
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
      command      = testExecTarget,
      commandName  = 'CTests::testExecTarget'
    }))
  end
  
end

