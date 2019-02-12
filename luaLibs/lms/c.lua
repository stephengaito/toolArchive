-- A lua script

-- At the moment we potentially over compile... since we use *all* of the 
-- expressed dependencies... we *could* instead use gcc/clang -MD/-MF or 
-- -MMD to compute the exact dependencies. 

c     = c     or { }
lms.c = lms.c or { }

local cDefaults = {
  cc       = 'gcc',
  cOpts    = { '-c -O2 -Wall -Werror' },
  cIncs    = { '-I.' },
  cExts    = { '.h', '.c'},
  needs    = { },
  libs     = { },
  linkOpts = { }
}

c = hMerge(cDefaults, lms.c, c)

function c.collectCDependencies(someDependencies)
  local cDependencies = { }
  for i, aDependency in ipairs(someDependencies) do 
    for j, aCExt in ipairs(c.cExts) do
      if aDependency:match('%'..aCExt..'$') then
        tInsert(cDependencies, aDependency)
      end
    end
  end
  return cDependencies
end

local function removeDuplicates(anArray)
  local hasValue = {}
  local newArray = {}

  for i,aValue in ipairs(anArray) do
     if (not hasValue[aValue]) then
         tInsert(newArray, aValue) 
         hasValue[aValue] = true
     end
  end
  return newArray
end

local function collectCSrc(cDef)
  cDef.cSrc   = { }
  cDef.oFiles = { }
  for i, aDependency in ipairs(cDef.dependencies) do
    if aDependency:match('%.c$') then
      tInsert(cDef.cSrc,   aDependency)
      local anOFile = aDependency:gsub('%.c$', '.o')
      tInsert(cDef.oFiles, anOFile)
    end
  end
end

local function cCompile(cDef)
  local cFile = cDef.target:gsub('%.o$', '.c')
  local cmd = {
    cDef.cc,
    tConcat(cDef.cOpts, ' '),
    tConcat(cDef.cIncs, ' '),
    cFile,
    '-o',
    cDef.target
  }
  return executeCmd(tConcat(cmd, ' '))
end

local function cLink(cDef)
  local cmd = {
    cDef.cc,
    tConcat(removeDuplicates(cDef.oFiles),   ' '),
    tConcat(removeDuplicates(cDef.libs),     ' '),
    tConcat(cDef.linkOpts, ' '),
    '-o',
    cDef.target
  }
  return executeCmd(tConcat(cmd, ' '))
end

local function computeNeeds(cDef)
  for i, aNeed in ipairs(cDef.needs) do
    local cFlags = getOutputFromCmd('pkg-config --cflags '..aNeed)
    if 0 < #cFlags then
      tInsert(cDef.cIncs, cFlags)
    end
    local libs = getOutputFromCmd('pkg-config --libs '..aNeed)
    if 0 < #libs then
      tInsert(cDef.libs, libs)
    end
  end
end

function c.program(cDef)
  if 0 < #cDef then cDef.target       = tRemove(cDef, 1) end
  if 0 < #cDef then cDef.dependencies = tRemove(cDef, 1) end
  cDef = hMerge(c, cDef)
  computeNeeds(cDef)
  collectCSrc(cDef)
  tInsert(cDef.cIncs, 1, '-I'..cDef.buildDir)
  for i, anOFile in ipairs(cDef.oFiles) do
    target(hMerge(cDef, {
      target = anOFile,
      command = cCompile
    }))
    tInsert(cleanTargets, nameCleanTarget(anOFile))
  end
  target(hMerge(cDef, { 
    dependencies = cDef.oFiles,
    command = cLink    
  }))
  tInsert(clobberTargets, nameClobberTarget(cDef.target))
end

function c.shared(cDef)
  cDef = hMerge(c, cDef)
  tInsert(cDef.cOpts,       '-fpic')
  tInsert(cDef.linkOpts, 1, '-shared')
  c.program(cDef)
end

function c.targets(defaultDef, cDef)

  cDef = hMerge(defaultDef, cDef or { })

  cDef.dependencies = { }
  tInsert(cDef.docFiles, 1, cDef.mainDoc)
  for i, aDocFile in ipairs(cDef.docFiles) do
    tInsert(cDef.dependencies, makePath{ cDef.docDir, aDocFile })
  end

  for i, anInclude in ipairs(cDef.cHeaderFiles) do
    if anInclude:match('%.h$') and
      not anInclude:match('-private%.') then
      local anIncludeDep = makePath {
        cDef.buildDir,
        anInclude
      }
      local includeDeps = { anIncludeDep }
      tInsert(headerTargets, anIncludeDep)
      target(hMerge(cDef, {
        target  = anIncludeDep,
        command = cDef.compileLitProg
      }))
    end
  end

  for i, aProgram in ipairs(cDef.programs) do
  
    local srcTarget = makePath{ cDef.buildDir, aProgram..'.c' }
    target(hMerge(cDef, {
      target  = srcTarget,
      command = cDef.compileLitProg
    }))
    
    local programTarget = makePath{ cDef.buildDir, aProgram }
    local cDependencies = { }
    cDef.srcFiles = aAppend(cDef.cHeaderFiles, cDef.cCodeFiles)
    tInsert(cDependencies, srcTarget)
    for j, aSrcFile in ipairs(cDef.srcFiles) do
      local aSrcPath = makePath{ cDef.buildDir, aSrcFile }
      local aParentPath = getParentPath(aSrcPath)
      ensurePathExists(aParentPath)
      addDependency(makePath{cDef.docDir, cDef.mainDoc}, aParentPath)
      addDependency(aSrcPath, aParentPath)
      tInsert(cDependencies, aSrcPath)
    end

    local pDef = hMerge(c, cDef)
    c.program(hMerge(pDef, {
      target       = programTarget,
      dependencies = c.collectCDependencies(cDependencies),
      needs        = { },
    }))
    tInsert(buildTargets, programTarget)
    
    tInsert(cleanTargets, nameCleanTarget(srcTarget))
  end
  
end

