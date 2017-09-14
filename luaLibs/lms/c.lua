-- A lua script

-- At the moment we potentially over compile... since we use *all* of the 
-- expressed dependencies... we *could* instead use gcc/clang -MD/-MF or 
-- -MMD to compute the exact dependencies. 

c = c or { }

local cDefaults = {
  cc     = 'gcc',
  cOpts  = '-c -O2 -Wall',
  cIncs  = { '-I.' },
  cExt   = { '.h', '.c'},
  needs  = { },
  libs   = { },
  ldOpts = { }
}

c = hMerge(cDefaults, c)

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
  local oFile = cDef.target:gsub('%.o$', '.c')
  local cmd = {
    cDef.cc,
    cDef.cOpts,
    tConcat(cDef.cIncs, ' '),
    oFile,
    '-o',
    cDef.target
  }
  return executeCmd(tConcat(cmd, ' '))
end

local function cLink(cDef)
  local cmd = {
    cDef.cc,
    tConcat(cDef.oFiles, ' '),
    tConcat(cDef.libs, ' '),
    tConcat(cDef.ldOpts, ' '),
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
  tInsert(cDef.cIncs, '-I'..cDef.buildDir)
  for i, anOFile in ipairs(cDef.oFiles) do
    target(hMerge(cDef, {
      target = anOFile,
      command = cCompile
    }))
  end
  target(hMerge(cDef, { 
    dependencies = cDef.oFiles,
    command = cLink    
  }))
end