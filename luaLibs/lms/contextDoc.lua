-- A Lua script

-- This lms Lua script creates the targets required to build, diff or 
-- install a ConTeXt document. 

contextDoc     = contextDoc     or { }
lms.contextDoc = lms.contextDoc or { }

local contextDefaults = { 
}

contextDoc = hMerge(contextDefaults, lms.contextDoc, contextDoc)

function createNewDocTarget(targetName, targetVarName, targetCommand)
  createNewTarget('doc-'..targetName, 'doc'..targetVarName, targetCommand)
  doNotRecurseTarget('doc-'..targetName)
  createNewTarget('bib-'..targetName, 'bib'..targetVarName, targetCommand)
  doNotRecurseTarget('bib-'..targetName)
  createNewTarget('pub-'..targetName, 'pub'..targetVarName, targetCommand)
  doNotRecurseTarget('pub-'..targetName)
end

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

local function gatherBibReferences(ctxDef)
  if type(ctxDef['docDir']) == 'nil' then return end

  runCmdIn('diSimp bib', ctxDef['docDir'])
  return true
end

local function copyAbstract(ctxDef)
  local inFile, inErr  = io.open('Abstract.md', 'r')
  if inErr then lmsError('Could not open the file [Abstract.md]') end
  inFile:read('L')

  local outFile, outErr = io.open(ctxDef.abstractPath, 'w')
  if outErr then lmsError('Could not open the file ['..ctxDef.abstractPath..']') end
  outFile:write("---\n")
  outFile:write("layout: paperVersion\n")
  outFile:write("year: "..os.date('%Y').."\n")
  outFile:write("month: "..os.date('%m').."\n")


  for aLine in inFile:lines('L') do
    outFile:write(aLine)
  end

  inFile:close()
  outFile:close()

  return true
end

local function publishDocument(ctxDef)
  lmsMessage('Publishing document using pdf2htmlEX')
  ctxDef['releaseOpts'] = ctxDef['releaseOpts']  or 
      '--zoom 1.3 --embed cfij --bg-format svg --split-pages 1 '
  local pageDoc = ctxDef.mainDoc:gsub('%.tex', '-%%d.page')
  local pdfDoc  = ctxDef.mainDoc:gsub('%.tex$', '.pdf')
  local pubCmd  = 
    'pdf2htmlEX ' .. ctxDef['releaseOpts'] ..
    ' --dest-dir ' .. makePath { ctxDef['releaseDir'], 'docHtml' } ..
    ' --page-filename ' .. pageDoc ..
    ' ' .. pdfDoc
  runCmdIn(pubCmd, ctxDef['docDir'])
  local cpCmd = 
    'cp ' .. pdfDoc .. ' ' .. ctxDef.releaseDir
  runCmdIn(cpCmd, ctxDef.docDir)
  return true
end

local function setupDocumentPublish(ctxDef)

  ctxDef['releaseType'] = ctxDef['releaseType'] or 'workingDraft'
  if type(ctxDef['releaseDir']) == 'nil' then
    print('WARNING: No document publishing release directory specified (using ".")')
    ctxDef['releaseDir'] = '.'
  end
  ctxDef['releasePath'] = ctxDef['releasePath'] or dirPrefix
  ctxDef['releaseDir'] = makePath{
    ctxDef['releaseDir'],
    ctxDef['releaseType'],
    os.date('%Y'),
    os.date('%m'),
    ctxDef['releasePath']
  }
  ensurePathExists(ctxDef['releaseDir'])

  local pdfMainDoc = ctxDef.mainDoc:gsub('%.tex$', '.pdf')
  local docTarget = makePath{ ctxDef.docDir, pdfMainDoc }
  local pubMainDoc = ctxDef.mainDoc:gsub('%.tex$', '.html')
  local pubTarget = makePath{ ctxDef.releaseDir, pubMainDoc }

  ctxDef['globalTargetVar'] = ctxDef['globalTargetVar'] or 'Targets'
  local pubVar = 'pub'..ctxDef['globalTargetVar']
  if type(_G[pubVar]) == 'nil' then
    createNewTarget('pub', 'pubTargets')
  end

  tInsert(_G['pub'..ctxDef['globalTargetVar']], pubTarget)
  target(hMerge(ctxDef, {
    target       = pubTarget,
    dependencies = { 
      docTarget,
      ctxDef.releaseDir,
      'Abstract.md',
      ctxDef.abstractDir
    },
    command      = publishDocument
  }))

  local absTarget = ctxDef.releaseDir..'.md'
  ctxDef['abstractPath'] = absTarget
  tInsert(_G['pub'..ctxDef['globalTargetVar']], absTarget)
  target(hMerge(ctxDef, {
    target       = absTarget,
    dependencies = { 
      'Abstract.md',
      ctxDef.releaseDir
    },
    command      = copyAbstract
  }))

end

function contextDoc.targets(ctxDef)
  ctxDef = hMerge(contextDoc, ctxDef)

  findSubDirs(ctxDef)
  findDocuments(ctxDef)

  ctxDef.dependencies = { }
  tInsert(ctxDef.docFiles, 1, makePath{ '.', ctxDef.docDir, ctxDef.mainDoc})
  for i, aDocFile in ipairs(ctxDef.docFiles) do
    local docPath = aDocFile
    if not docPath:find('^%.') then
      docPath = makePath{ ctxDef.docDir, docPath }
    end
    tInsert(ctxDef.dependencies, docPath)
  end

  local pdfMainDoc = ctxDef.mainDoc:gsub('%.tex$', '.pdf')
  local docTarget = makePath{ ctxDef.docDir, pdfMainDoc }
  ctxDef['globalTargetVar'] = ctxDef['globalTargetVar'] or 'Targets'
  tInsert(_G['doc'..ctxDef['globalTargetVar']], docTarget)
  target(hMerge(ctxDef, {
    target  = docTarget,
    command = compileDocument
  }))

  local bibMainDoc = ctxDef.mainDoc:gsub('%.tex$', 'Bib.lua')
  local bibTarget = makePath{ ctxDef.docDir, bibMainDoc }
  tInsert(_G['bib'..ctxDef['globalTargetVar']], bibTarget)
  target(hMerge(ctxDef, {
    target  = bibTarget,
    command = gatherBibReferences
  }))

  setupDocumentPublish(ctxDef)

  tInsert(clobberTargets, nameClobberTarget(docTarget))
  tInsert(clobberTargets,
    nameClobberTarget(docTarget:gsub('%.pdf', 'Bib.lua')))

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
