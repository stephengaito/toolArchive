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
    tConcat(cDef.oFiles,   ' '),
    tConcat(cDef.libs,     ' '),
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

