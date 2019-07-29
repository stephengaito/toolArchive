-- A lua script

-- Deal with arbitrary src files (and simply copy them into the buildDir)

src     = src or { }
lms.src = lms.src or { }

local srcDefaults = {
  buildDir     = 'buildDir',
  srcDir       = 'src',
  srcFiles     = { },
  cHeaderFiles = { },
  cCodeFiles   = { }
}

src = hMerge(srcDefaults, lms.src, src)

local function cpSrc(srcDef, onExit)
  executeCmd('cp '..srcDef.srcPath..' '..srcDef.target, onExit)
end

function src.targets(defaultDef, srcDef)

  srcDef = hMerge(src, defaultDef, srcDef)
  srcDef.creator = 'src-targets'
  
  srcDef.srcFiles = aAppend(
    srcDef.cHeaderFiles or { },
    srcDef.cCodeFiles or { },
    srcDef.srcFiles
  )

  for i, aSrcFile in ipairs(srcDef.srcFiles) do
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
  
  return srcDef
end

