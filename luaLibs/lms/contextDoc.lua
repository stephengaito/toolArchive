-- A Lua script

-- This lms Lua script creates the targets required to build, diff or 
-- install a ConTeXt document. 

contextDoc     = contextDoc     or { }
lms.contextDoc = lms.contextDoc or { }

-- no module defaults

contextDoc = hMerge(lms.contextDoc, contextDoc)

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


function contextDoc.targets(ctxDef)

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

function contextDoc.multiDocument(mainDoc)
  local docFiles = { }
  local texFileList = io.popen('find -name "*.tex"')
  for docTexFilePath in texFileList:lines('*l') do
    if not docTexFilePath:find('releases') and
        not docTexFilePath:find('buildDir') and
        not docTexFilePath:find('bin')      then
      if docTexFilePath:match('^%.%/doc%/') then
        docTexFilePath = docTexFilePath:gsub('^%.%/doc%/','')
      else
        docTexFilePath = '.'..docTexFilePath
      end
      table.insert(docFiles, docTexFilePath)
    end
  end
  contextDoc.targets{
    mainDoc  = mainDoc,
    docFiles = docFiles,
    docDir   = 'doc'
  }
end