-- A Lua script

-- This lms Lua script creates the targets required to build, diff or 
-- install a suite of Contest joylolTest based unit tests. 

joylolTests     = joylolTests     or { }
lms.joylolTests = lms.joylolTests or { }

local joylolTestsDefaults = {
  joylolTestsIncDir =
    makePath{
      getEnv('HOME'), 'texmf', 't-contests', 'tex',
      'context', 'third', 'contests'
    },
  joylolTestsStartup = 't-contests-joylolTests.lua'
}

joylolTests = hMerge(joylolTestsDefaults, lms.joylolTests, joylolTests)

function joylolTests.targets(defaultDef, jtDef)

  jtDef = hMerge(defaultDef, jtDef or { })
  jtDef.creator = 'joylolTests-targets'

  jtDef.dependencies = { }
  tInsert(jtDef.docFiles, 1, jtDef.mainDoc)
  for i, aDocFile in ipairs(jtDef.docFiles) do
    tInsert(jtDef.dependencies, makePath{ jtDef.docDir, aDocFile })
  end

  for i, aTestExec in ipairs(jtDef.testExecs) do
  
    local testExecTarget = makePath{ jtDef.buildDir, aTestExec }  
    local testExecSrc    = testExecTarget..'.joy'
    
    target(hMerge(jtDef, {
      target      = testExecSrc,
      command     = jtDef.compileLitProg,
      commandName = 'jtDef::compileLitProg ('..jtDef.commandName..')'
      }))
    
    local testTarget = makePath{ jtDef.buildDir, aTestExec..'-results.lua' }
    tInsert(testTargets, testTarget)
    target(hMerge(jtDef, {
      target       = testTarget,
      dependencies = { testExecSrc },
      command      = "joylol --quiet "..testExecTarget,
      commandName  = 'JoyLoLTests::testTarget (joylol)'
    }))
    tInsert(clobberTargets, nameClobberTarget(testExecSrc))
  end
  
end

