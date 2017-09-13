-- A lua script

-- At the moment we potentially over compile... since we use *all* of the 
-- expressed dependencies... we *could* instead use gcc/clang -MD/-MF or 
-- -MMD to compute the exact dependencies. 

c = {
  cc        = 'gcc',
  cOptions  = '-c -O2 -Wall',
  cIncludes = '-I.',
}

local function collectCSrc(cDef)
  cDef.cSrc   = { }
  cDef.oFiles = { }
  for i, aDependency in ipairs(cDef.dependencies) do
    if aDependency:match('%.c$') then
      tInsert(cDef.cSrc,   aDependency)
      tInsert(cDef.oFiles, aDependency:gsub('%.c$', '.o'))
    end
  end
end

local function cCompile(cDef)
  local cmd = {
    cDef.cc,
    cDef.cOptions,
    cDef.cIncludes,
    cDef.target:gsub('%.o$', '.c'),
    '-o',
    cDef.target
  }
  print(tConcat(cmd, ' '))
end

local function cLink(cDef)
  local cmd = {
    cDef.cc,
    tConcat(cDef.oFiles, ' '),
    cDef.libs,
    cDef.ldOptions,
    '-o',
    cDef.target
  }
  print(tConcat(cmd, ' '))
end

function c.program(cDef)
  if 0 < #cDef then cDef.target       = tRemove(cDef, 1) end
  if 0 < #cDef then cDef.dependencies = tRemove(cDef, 1) end
  collectCSrc(cDef)
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