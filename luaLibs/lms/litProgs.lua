-- A Lua script

-- This lms Lua script creates the targets required to build, diff or 
-- install a literate programming project. 

litProgs     = litProgs     or { }
lms.litProgs = lms.litProgs or { }

-- no module defaults

litProgs = hMerge(lms.litProgs, litProgs)

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

local function installAndDiff(aDef, aFile)
  local buildTarget = makePath{ aDef.buildDir, 'lmsfile' }
  tInsert(buildTargets, buildTarget)
  target(hMerge(aDef, {
    target  = buildTarget,
    command = compileLitProg
  }))
      
  local diffTarget      = 'diff-'..aFile
  local installedTarget = 'lmsfile'
  tInsert(diffTargets, diffTarget)
  target(hMerge(aDef, {
    target       = diffTarget,
    dependencies = { buildTarget },
    command      = 'diff '..buildTarget..' '..installedTarget
  }))

  tInsert(installTargets, installedTarget)
  target(hMerge(aDef, {
    target       = installedTarget,
    dependencies = { buildTarget },
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

  installAndDiff(lpDef, 'lmsfile')
    
  return lpDef
end
