-- A Lua script

-- This lms Lua script creates the targets required to build, diff or 
-- install a ConTeXt document. 

context     = context     or { }
lms.context = lms.context or { }

-- no module defaults

context = hMerge(lms.context, context)

local function compileDocument(ctxDef)
  local curDir = lfs.currentdir()
  chDir(ctxDef.docDir)
  --
  -- build the complete context document
  --
  local result = executeCmd('context '..ctxDef.mainDoc)
  --
  chDir(curDir)
  return result
end


function context.targets(ctxDef)

  ctxDef.dependencies = { }
  tInsert(ctxDef.docFiles, 1, ctxDef.mainDoc)
  for i, aDocFile in ipairs(ctxDef.docFiles) do
    tInsert(ctxDef.dependencies, makePath{ ctxDef.docDir, aDocFile })
  end

  local pdfMainDoc = ctxDef.mainDoc:gsub('%.tex$', '.pdf')
  local docTarget = makePath{ ctxDef.docDir, pdfMainDoc }
  tInsert(docTargets, docTarget)
  target(hMerge(ctxDef, {
    target  = docTarget,
    command = compileDocument
  }))

  tInsert(clobberTargets, nameClobberTarget(docTarget))

  tInsert(cleanTargets, 
    nameCleanTarget(docTarget:gsub('%.pdf', '.log')))
  tInsert(cleanTargets, 
    nameCleanTarget(docTarget:gsub('%.pdf$', '.tuc')))

end