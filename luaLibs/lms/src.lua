-- A lua script

-- Deal with arbitrary src files (and simply copy them into the buildDir)

src     = src or { }
lms.src = lms.src or { }

local srcDefaults = {
  buildDir      = 'buildDir',
  srcDir        = 'src',
  handGenerated = { },
}

src = hMerge(srcDefaults, lms.src, src)

local function cpSrc(srcDef, onExit)
  executeLocalCmd(
    srcDef.target,
    'cp '..srcDef.srcPath..' '..srcDef.target,
    onExit
  )
end

function src.targets(defaultDef, srcDef)

  srcDef = hMerge(src, defaultDef, srcDef)
  srcDef.creator = 'src-targets'
  
  print('scrDef: '..prettyPrint(srcDef))
  
  srcDef.handGenerated = srcDef.handGenerated or { }
  local handGenerated = srcDef.handGenerated
  print('srcDef.handGenerated: '..prettyPrint(handGenerated))
  for aSrcType, someSrcFiles in pairs(handGenerated) do
    srcDef[aSrcType] = srcDef[aSrcType] or { }
    local srcDefSrcType = srcDef[aSrcType]
    for i, aSrcFile in ipairs(someSrcFiles) do
      aInsertOnce(srcDefSrcType, aSrcFile)
      local aBuildPath = makePath{ srcDef.buildDir, aSrcFile }
      local aSrcPath   = makePath{ srcDef.srcDir,   aSrcFile }
      target(hMerge(srcDef, {
        target       = aBuildPath,
        srcPath      = aSrcPath,
        dependencies = { aSrcPath },
        command      = cpSrc,
        commandName  = 'Src::cpSrc'
      }))      
      local aParentPath = getParentPath(aBuildPath)
      ensurePathExists(aParentPath)
      addDependency(aBuildPath, aParentPath)
    end
  end

  return srcDef
end

