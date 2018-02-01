-- A Lua script

-- This lms Lua script creates the targets required to build, diff or 
-- install a suite of JoyLoL CoAlgebra extensions (shared libraries). 

require 'lms.c'

joylol     = joylol     or { }
lms.joylol = lms.joylol or { }

-- no module defaults

joylol = hMerge(lms.joylol, joylol)

function joylol.targets(defaultDef, jDef)

  jDef = hMerge(defaultDef, jDef or { })

--  local buildJoylol = makePath{jDef.buildDir, 'joylol'}
--  ensurePathExists(buidlJoylol)
--  tInsert(jDef.dependencies, buildJoylol)
--  target(hMerge(jDef, {
--    dependencies = { },
--    target       = buildJoylol,
--    command      = 'mkdir -p '..buildJoylol
--  }))
  
  tInsert(c.cOpts, '-fPIC')

  for i, aCoAlg in ipairs(jDef.coAlgs) do

    local coAlgTarget = makePath{ jDef.buildDir, aCoAlg..'.so' }
    tInsert(buildTargets, coAlgTarget)
    local cDependencies = { }
    jDef.srcFiles = aAppend(jDef.cHeaderFiles, jDef.cCodeFiles)
    for j, aSrcFile in ipairs(jDef.srcFiles) do
      srcTarget = makePath{ jDef.buildDir, aSrcFile }
      local parentPath = getParentPath(srcTarget)
      if parentPath then
        ensurePathExists(parentPath)
        tInsert(jDef.dependencies, parentPath)
      end
      tInsert(cDependencies, srcTarget)
      target(hMerge(jDef, {
        target  = srcTarget,
        command = jDef.compileLitProg
      }))
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
    if jDef.cCodeLibDirs then
      pDef.linkOpts = aAppend(pDef.linkOpts, jDef.cCodeLibDirs)
    end
    if jDef.cCodeLibs then
      pDef.libs = aAppend(pDef.libs, jDef.cCodeLibs)
    end

    local coAlgDeps = c.collectCDependencies(cDependencies)
    local parentPath = getParentPath(coAlgTarget)
    if parentPath then
      ensurePathExists(parentPath)
      tInsert(coAlgDeps, parentPath)
    end
    c.shared(hMerge(pDef, {
      target = coAlgTarget,
      dependencies = coAlgDeps,
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
    local parentPath = getParentPath(installTarget)
    local installDeps = { coAlgTarget }
    if parentPath then
      ensurePathExists(parentPath)
      tInsert(installDeps, parentPath) 
    end
    tInsert(installTargets, installTarget)
    target(hMerge(jDef, {
      target = installTarget,
      dependencies = installDeps,
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
      local includeDeps = { anIncludeDep }
      local parentPath = getParentPath(installTarget)
      if parentPath then 
        ensurePathExists(parentPath)
        tInsert(includeDeps, parentPath)
      end
      tInsert(headerTargets, installTarget)
      tInsert(installTargets, installTarget)
      target(hMerge(jDef, {
        target = installTarget,
        dependencies = includeDeps,
        command = tConcat({
          'install -T',
          anIncludeDep,
          installTarget
        }, ' ')
      }))
    end
  end
  
  return jDef
end