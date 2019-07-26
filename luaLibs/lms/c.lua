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

local function cCompile(cDef, onExit)
  local cFile = cDef.target:gsub('%.o$', '.c')
  local cmd = {
    cDef.cc,
    tConcat(cDef.cOpts, ' '),
    tConcat(cDef.cIncs, ' '),
    cFile,
    '-o',
    cDef.target
  }
  return executeCmd(tConcat(cmd, ' '), onExit)
end

local function cLink(cDef, onExit)
  local cmd = {
    cDef.cc,
    tConcat(removeDuplicates(cDef.oFiles),   ' '),
    tConcat(removeDuplicates(cDef.libs),     ' '),
    tConcat(cDef.linkOpts, ' '),
    '-o',
    cDef.target
  }
  return executeCmd(tConcat(cmd, ' '), onExit)
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
    if not getTargetFor(anOFile) then
      target(hMerge(cDef, {
        target      = anOFile,
        command     = cCompile,
        commandName = 'C::cCompile'
      }))
      tInsert(cleanTargets, nameCleanTarget(anOFile))
    end
  end
  target(hMerge(cDef, { 
    dependencies = cDef.oFiles,
    command      = cLink,
    commandName  = 'C::cLink'
  }))
  tInsert(clobberTargets, nameClobberTarget(cDef.target))
end

function c.shared(cDef)
  cDef = hMerge(c, cDef)
  tInsert(cDef.cOpts,       '-fpic')
  tInsert(cDef.linkOpts, 1, '-shared')
  c.program(cDef)
end

function c.makeSrcTarget(cDef, cDependencies, aSrcFile)
  local aSrcPath = makePath {
    cDef.buildDir,
    aSrcFile
  }
  if aSrcFile:match('%.h$') then
    tInsert(headerTargets, aSrcPath)
  end
  target(hMerge(cDef, {
    target      = aSrcPath,
    command     = cDef.compileLitProg,
    commandName = 'cDef::compileLitProg'
  }))

  local aParentPath = getParentPath(aSrcPath)
  ensurePathExists(aParentPath)
  addDependency(makePath{cDef.docDir, cDef.mainDoc}, aParentPath)
  addDependency(aSrcPath, aParentPath)

  aInsertOnce(cDependencies, aSrcPath)
end

function c.targets(defaultDef, cDef)

  cDef = hMerge(defaultDef, cDef or { })
  cDef.creator = 'c-targets'
  
  cDef.dependencies = cDef.dependencies or { }
  local cDependencies = { }

  cDef.cSrcFiles = aAppend(cDef.cHeaderFiles, cDef.cCodeFiles)
  for i, aSrcFile in ipairs(cDef.cSrcFiles) do
    c.makeSrcTarget(cDef, cDependencies, aSrcFile)
  end
  
  for i, aProgram in ipairs(cDef.programs) do
  
    local srcTarget = makePath{ cDef.buildDir, aProgram..'.c' }
    if not getTargetFor(srcTarget) then
      c.makeSrcTarget(cDef, cDependencies, aSrcTarget)
    end
    
    local programTarget = makePath{ cDef.buildDir, aProgram }
    c.program(hMerge(cDef, {
      target       = programTarget,
      dependencies = c.collectCDependencies(cDependencies),
      needs        = { },
    }))
    tInsert(buildTargets, programTarget)
    
  end
  
  return cDef
end

