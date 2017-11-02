-- A Lua script

-- This lms Lua script creates the targets required to build, diff or 
-- install a suite of JoyLoL CoAlgebra extensions (shared libraries). 

require 'lms.c'

joylol     = joylol     or { }
lms.joylol = lms.joylol or { }

-- no module defaults

joylol = hMerge(lms.joylol, joylol)

function joylol.targets(jDef)

--  -- update the required libraries for ALL C compilations
--  for j, aCoAlgLib in ipairs(jDef.coAlgLibs) do
--    tInsert(c.libs, makePath{
--      getEnv('HOME'),
--      '.joylol',
--      'joylol',
--      aCoAlgLib..'.so'
--    })
--  end
  tInsert(c.cOpts, '-fPIC')

  for i, aCoAlg in ipairs(jDef.coAlgs) do

    local coAlgTarget = makePath{ jDef.buildDir, aCoAlg..'.so' }
    local cDependencies = { }
    for j, aSrcFile in ipairs(jDef.srcFiles) do
      tInsert(cDependencies, makePath{ jDef.buildDir, aSrcFile })
    end
    local pDef = hMerge(c, jDef)
    tInsert(pDef.cIncs, 1, tConcat{
      '-I',
      makePath{
        getEnv('HOME'),
        '.joylol',
        'include'
      }
    })
    c.shared(hMerge(pDef, {
      target = coAlgTarget,
      dependencies = c.collectCDependencies(cDependencies),
      needs = { 'lua5.2' },
    }))

    local installPath   = makePath{
      getEnv('HOME'),
      '.joylol'
    }
    local installTarget = makePath{
      installPath,
      'joylol',
      aCoAlg..'.so'
    }
    lfs.mkdir(installPath)
    tInsert(installTargets, installTarget)
    target(hMerge(jDef, {
      target = installTarget,
      dependencies = { coAlgTarget },
      command = tConcat({
        'install -T',
        coAlgTarget,
        installTarget
      }, ' ')
    }))
  end
  
  for i, anInclude in ipairs(jDef.srcFiles) do
    if anInclude:match('%.h$') and
      not anInclude:match('-private%.') then
      local anIncludeDep = makePath {
        jDef.buildDir,
        anInclude
      }
      local installPath = makePath{
        getEnv('HOME'),
        '.joylol',
        'include'
      }
      local installTarget = makePath{
        installPath,
        anInclude
      }
      lfs.mkdir(installPath)
      tInsert(headerTargets, installTarget)
      tInsert(installTargets, installTarget)
      target(hMerge(jDef, {
        target = installTarget,
        dependencies = { anIncludeDep },
        command = tConcat({
          'install -T',
          anIncludeDep,
          installTarget
        }, ' ')
      }))
    end
  end
end