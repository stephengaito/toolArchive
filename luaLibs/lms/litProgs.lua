-- A Lua script

-- This lms Lua script creates the targets required to build, diff or 
-- install a literate programming project. 

litProgs     = litProgs     or { }
lms.litProgs = lms.litProgs or { }

-- no module defaults

litProgs = hMerge(lms.litProgs, litProgs)

local function compileDocument(lpDef)
  local curDir = lfs.currentdir()
  chDir(lpDef.docDir)
  --
  -- build the complete context document
  --
  local result = executeCmd('context '..lpDef.mainDoc)
  --
  chDir(curDir)
  return result
end

local function compileLitProg(lpDef)
  local curDir = lfs.currentdir()
  chDir(lpDef.docDir)
  --
  -- build the litProg output usning simple one context pass...
  --
  local result = executeCmd('context --once '..lpDef.mainDoc)
  --
  -- remove the PDF file since we only want the litProg output
  --
  os.remove(lpDef.mainDoc:gsub('%.tex$', '.pdf')) 
  --
  chDir(curDir)
  return result
end

function litProgs.targets(lpDef)
  
  lpDef.dependencies = { }
  tInsert(lpDef.docFiles, 1, lpDef.mainDoc)
  for i, aDocFile in ipairs(lpDef.docFiles) do
    tInsert(lpDef.dependencies, makePath{ lpDef.docDir, aDocFile })
  end
  
  for i, aSrcFile in ipairs(lpDef.srcFiles) do
    
    local srcTarget = makePath{ lpDef.buildDir, aSrcFile }
    tInsert(buildTargets, srcTarget)
    target(hMerge(lpDef, {
      target  = srcTarget,
      command = compileLitProg
    }))

    local diffTarget   = 'diff-'..aSrcFile
    local moduleTarget = makePath{ lpDef.moduleDir, aSrcFile }
    tInsert(diffTargets, diffTarget)
    target(hMerge(lpDef, {
      target       = diffTarget,
      dependencies = { srcTarget, moduleTarget },
      command      = 'diff '..srcTarget..' '..moduleTarget
    }))

    local installTarget = 'install-'..aSrcFile
    tInsert(installTargets, installTarget)
    target(hMerge(lpDef, {
      target       = installTarget,
      dependencies = { srcTarget },
      command      = 'cp '..srcTarget..' '..moduleTarget
    }))
    
    tInsert(cleanTargets, nameCleanTarget(srcTarget))
  end
  
  local srcTarget = makePath{ lpDef.buildDir, 'lmsfile' }
  tInsert(buildTargets, srcTarget)
  target(hMerge(lpDef, {
    target  = srcTarget,
    command = compileLitProg
  }))
      
  local diffTarget   = 'diff-lmsfile'
  local moduleTarget = 'lmsfile'
  tInsert(diffTargets, diffTarget)
  target(hMerge(lpDef, {
    target       = diffTarget,
    dependencies = { srcTarget, moduleTarget },
    command      = 'diff '..srcTarget..' '..moduleTarget
  }))

  local installTarget = 'install-lmsfile'
  tInsert(installTargets, installTarget)
  target(hMerge(lpDef, {
    target       = installTarget,
    dependencies = { srcTarget },
    command      = 'cp '..srcTarget..' '..moduleTarget
  }))
  
  tInsert(cleanTargets, nameCleanTarget(srcTarget))

  local pdfMainDoc = lpDef.mainDoc:gsub('%.tex$', '.pdf')
  local docTarget = makePath{ lpDef.docDir, pdfMainDoc }
  tInsert(docTargets, docTarget)
  target(hMerge(lpDef, {
    target  = docTarget,
    command = compileDocument
  }))
  
  if lpDef.cCodeLibDirs then
    lms.c          = lms.c or { }
    lms.c.linkOpts = lpDef.cCodeLibDirs
  end
  
  if lpDef.cCodeLibs then
    lms.c      = lms.c or { }
    lms.c.libs = lpDef.cCodeLibs
  end
  
  tInsert(clobberTargets, nameClobberTarget(docTarget))
  
  tInsert(cleanTargets, 
    nameCleanTarget(docTarget:gsub('%.pdf', '.log')))
  tInsert(cleanTargets, 
    nameCleanTarget(docTarget:gsub('%.pdf$', '.tuc')))
end