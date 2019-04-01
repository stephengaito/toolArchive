-- A Lua script

-- This lms Lua script creates the targets required to build, diff or 
-- install a ConTeXt document. 

contextDoc     = contextDoc     or { }
lms.contextDoc = lms.contextDoc or { }

local contextDefaults = {
  findRegexp = ".*\\.tex\\|.*Bib\\.lua",
  ignoreDirs = { 'releases', 'buildDir', 'bin' },
}

contextDoc = hMerge(contextDefaults, lms.contextDoc, contextDoc)

function createNewDocTarget(targetName, targetVarName, targetCommand)
  createNewTarget('doc-'..targetName, 'doc'..targetVarName, targetCommand)
  createNewTarget('bib-'..targetName, 'bib'..targetVarName, targetCommand)
  createNewTarget('pub-'..targetName, 'pub'..targetVarName, targetCommand)
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

local function publishDocument(ctxDef)
  lmsMessage('Publishing document using pdf2htmlEX')
  if type(ctxDef['releaseOpts']) == 'nil' then
    ctxDef['releaseOpts'] =
      '--zoom 1.3 --embed cfijo --bg-format svg --split-pages 1 '
  end
  local pageDoc = ctxDef.mainDoc:gsub('%.tex', '-%%d.page')
  local pdfDoc  = ctxDef.mainDoc:gsub('%.tex$', '.pdf')
  local pubCmd  = 
    'pdf2htmlEX ' .. ctxDef['releaseOpts'] ..
    ' --dest-dir ' .. ctxDef['releaseDir'] ..
    ' --page-filename ' .. pageDoc ..
    ' ' .. pdfDoc
  runCmdIn(pubCmd, ctxDef['docDir'])
  return true
end

local function findDocuments(ctxDef)
  if type(ctxDef['docFiles']) ~= 'nil' then return end
  local docFiles = { }
  for i, aDir in ipairs(ctxDef['subDirs']) do
    local findCmd  = 'find '..aDir..' -iregex "'..ctxDef['findRegexp']..'"'
    lmsMessageVery('Dynamically finding documents using ['..findCmd..']')
    local texFileList = io.popen(findCmd)
    for docTexFilePath in texFileList:lines('*l') do
      docTexFilePath = './'..docTexFilePath
      table.insert(docFiles, docTexFilePath)
    end
  end
  ctxDef.docFiles = docFiles
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
  if type(ctxDef['globalTargetVar']) == 'nil' then 
    ctxDef['globalTargetVar'] = 'Targets'
  end
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

  if type(ctxDef['releaseType']) == 'nil' then
    ctxDef['releaseType'] = 'workingDrafts'
  end
  if type(ctxDef['releaseDir']) == 'nil' then
    print('WARNING: No document publishing release directory specified (using ".")')
  end
  if type(ctxDef['releaseName']) == 'nil' then
    ctxDef['releaseName'] = ctxDef.mainDoc:gsub('%.tex','')
  end
  ctxDef['releaseDir'] = makePath{
    ctxDef['releaseDir'],
    ctxDef['releaseType'],
    os.date('%Y'),
    os.date('%m'),
    ctxDef['releaseName']
  }
  ensurePathExists(ctxDef['releaseDir'])
  local pubMainDoc = ctxDef.mainDoc:gsub('%.tex$', '.html')
  local pubTarget = makePath{ ctxDef.releaseDir, pubMainDoc }
  tInsert(_G['pub'..ctxDef['globalTargetVar']], pubTarget)
  target(hMerge(ctxDef, {
    target       = pubTarget,
    dependencies = { docTarget, ctxDef['releaseDir'] },
    command      = publishDocument
  }))

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
