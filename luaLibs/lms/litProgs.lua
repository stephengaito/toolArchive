-- A Lua script

-- This lms Lua script creates the targets required to build, diff or 
-- install a literate programming project. 

litProgs     = litProgs     or { }
lms.litProgs = lms.litProgs or { }

-- no module defaults

litProgs = hMerge(lms.litProgs, litProgs)

local function compileLitProg(lpDef, onExit)
  local curDir = lfs.currentdir()
  chDir(lpDef.docDir)
  --
  -- build the litProg output usning simple one context pass...
  --
  executeCmd('context --silent=all --once '..lpDef.mainDoc, function(code, signal)
    --
    -- remove the PDF file since we only want the litProg output
    --
    os.remove(lpDef.mainDoc:gsub('%.tex$', '.pdf')) 
    --
    chDir(curDir)
    onExit(code, signal)
  end)
end

local function installAndDiff(aDef, installDir, aFile)
  local buildTarget = makePath{ aDef.buildDir, aFile }
  local parentPath  = getParentPath(buildTarget)
  if parentPath then
    ensurePathExists(parentPath)
    tInsert(aDef.dependencies, parentPath)
  end
  aInsertOnce(buildTargets, buildTarget)
  target(hMerge(aDef, {
    target  = buildTarget,
    command = compileLitProg
  }))
      
  local diffTarget      = 'diff-'..aFile
  local installedTarget = aFile
  if installDir then
    installedTarget = makePath{ installDir, aFile }
  end
  aInsertOnce(diffTargets, diffTarget)
  target(hMerge(aDef, {
    target       = diffTarget,
    dependencies = { buildTarget },
    command      = 'diff '..buildTarget..' '..installedTarget
  }))

  local installDep = { buildTarget }
  parentPath = getParentPath(installedTarget)
  if parentPath then
    ensurePathExists(parentPath)
    aInsertOnce(installDep, parentPath)
  end
  aInsertOnce(installTargets, installedTarget)
  target(hMerge(aDef, {
    target       = installedTarget,
    dependencies = installDep,
    command      = 'cp '..buildTarget..' '..installedTarget
  }))
  
  tInsert(cleanTargets, nameCleanTarget(buildTarget))
end

function litProgs.targets(defaultDef, lpDef)

  lpDef = hMerge(defaultDef, lpDef or { })
  lpDef.creator = 'litProgs-targets'
  
  findSubDirs(lpDef)
  findDocuments(lpDef)

  lpDef.compileLitProg = compileLitProg
  lpDef.installAndDiff = installAndDiff
  
  lpDef.dependencies = lpDef.dependencies or { }

  installAndDiff(lpDef, nil, 'lmsfile')
    
  return lpDef
end
