-- A Lua script

-- This lms Lua script creates the targets required to build, diff or 
-- install a ConTeXt module. 

contextMod     = contextMod     or { }
lms.contextMod = lms.contextMod or { }

contextMod = hMerge(lms.contextMod, contextMod)

function contextMod.targets(defaultDef, ctxModDef)

  ctxModDef = hMerge(defaultDef, ctxModDef or { })
  ctxModDef.creator = 'contextMod-targets'

  ctxModDef.moduleDir = ctxModDef.moduleDir or 'installDir'
  ctxModDef.moduleDir =
    ctxModDef.moduleDir:gsub('<HOME>', os.getenv('HOME'))
  ensurePathExists(ctxModDef.moduleDir)
  tInsert(ctxModDef.dependencies, ctxModDef.moduleDir)
  target(hMerge(ctxModDef, {
    dependencies = { },
    target       = ctxModDef.moduleDir,
    command      = 'mkdir -p '..ctxModDef.moduleDir
  }))
  
  for i, aSrcFile in ipairs(ctxModDef.moduleFiles) do
    
    local srcTarget = makePath{ ctxModDef.buildDir, aSrcFile }
    local parentPath = getParentPath(srcTarget)
    if parentPath then
      ensurePathExists(parentPath)
      tInsert(ctxModDef.dependencies, parentPath)
    end
    tInsert(buildTargets, srcTarget)
    target(hMerge(ctxModDef, {
      target  = srcTarget,
      command = ctxModDef.compileLitProg
    }))

    local diffTarget   = 'diff-'..aSrcFile
    local moduleTarget = makePath{ ctxModDef.moduleDir, aSrcFile }
    tInsert(diffTargets, diffTarget)
    target(hMerge(ctxModDef, {
      target       = diffTarget,
      dependencies = { srcTarget },
      command      = 'diff '..srcTarget..' '..moduleTarget
    }))

    tInsert(installTargets, moduleTarget)
    local installDep = { srcTarget }
    local parentPath = getParentPath(moduleTarget)
    if parentPath then
      ensurePathExists(parentPath)
      tInsert(installDep, parentPath)
    end
    target(hMerge(ctxModDef, {
      target       = moduleTarget,
      dependencies = installDep,
      command      = 'cp '..srcTarget..' '..moduleTarget
    }))
    
    tInsert(cleanTargets, nameCleanTarget(srcTarget))
  end


end
