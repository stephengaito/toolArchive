-- A Lua script

-- This lms Lua script creates the targets required to build, diff or 
-- install a ConTeXt module. 

contextMod     = contextMod     or { }
lms.contextMod = lms.contextMod or { }

contextMod = hMerge(lms.contextMod, contextMod)

function contextMod.targets(defaultDef, ctxModDef)

  ctxModDef = hMerge(defaultDef, ctxModDef or { })

  for i, aSrcFile in ipairs(ctxModDef.moduleFiles) do
    
    local srcTarget = makePath{ ctxModDef.buildDir, aSrcFile }
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
      dependencies = { srcTarget, moduleTarget },
      command      = 'diff '..srcTarget..' '..moduleTarget
    }))

    local installTarget = 'install-'..aSrcFile
    tInsert(installTargets, installTarget)
    target(hMerge(ctxModDef, {
      target       = installTarget,
      dependencies = { srcTarget },
      command      = 'cp '..srcTarget..' '..moduleTarget
    }))
    
    tInsert(cleanTargets, nameCleanTarget(srcTarget))
  end


end