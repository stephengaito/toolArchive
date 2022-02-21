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

function src.targets(defaultDef, srcDef)

  srcDef = hMerge(src, defaultDef, srcDef)
  srcDef.creator = 'src-targets'

  srcDef.handGenerated = srcDef.handGenerated or { }
  local handGenerated = srcDef.handGenerated
  
  for aSrcType, someSrcFiles in pairs(handGenerated) do
    srcDef[aSrcType] = srcDef[aSrcType] or { }
    local srcDefSrcType = srcDef[aSrcType]
    for i, aSrcFile in ipairs(someSrcFiles) do
      aInsertOnce(srcDefSrcType, aSrcFile)
      local aBuildPath = makePath{ srcDef.buildDir, aSrcFile }
      local aSrcPath   = makePath{ srcDef.srcDir,   aSrcFile }
      target(hMerge(srcDef, {
        target       = aBuildPath,
        dependencies = { aSrcPath },
        command      = 'cp '..aSrcPath..' '..aBuildPath,
        commandName  = 'Src::cpSrc'
      }))      
      local aParentPath = getParentPath(aBuildPath)
      ensurePathExists(aParentPath)
      addDependency(aBuildPath, aParentPath)
    end
  end

  return srcDef
end

