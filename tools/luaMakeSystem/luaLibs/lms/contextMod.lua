-- A Lua script

-- This lms Lua script creates the targets required to build, diff or 
-- install a ConTeXt module. 

contextMod     = contextMod     or { }
lms.contextMod = lms.contextMod or { }

contextMod = hMerge(lms.contextMod, contextMod)

function contextMod.targets(defaultDef, ctxModDef)

  ctxModDef = hMerge(defaultDef, ctxModDef or { })
  ctxModDef.creator = 'contextMod-targets'
  print("contextMod.targets in ["..lfs.currentdir().."]")
  print(prettyPrint(ctxModDef))
  
  ctxModDef.moduleDir = ctxModDef.moduleDir or 'installDir'
  replaceEnvironmentVarsInPath(ctxModDef.moduleDir)
  ctxModDef.moduleDir = makePath{
    dirPrefix,
    ctxModDef.moduleDir
  }
  ensurePathExists(ctxModDef.moduleDir)
  aInsertOnce(ctxModDef.dependencies, ctxModDef.moduleDir)

  ctxModDef.ctxModuleFiles = ctxModDef.ctxModuleFiles or { }
  local moduleFiles     = ctxModDef.ctxModuleFiles
  for i, aSrcFile in ipairs(moduleFiles) do
    ctxModDef.installTarget(ctxModDef, ctxModDef.moduleDir, aSrcFile)
    ctxModDef.diffTarget(ctxModDef, ctxModDef.moduleDir, aSrcFile)
    local builtSrcPath = makePath {
      ctxModDef.buildDir,
      aSrcFile
    }
    ctxModDef.setupPublishCodeFile(ctxModDef, aSrcFile, builtSrcPath)
  end


end
