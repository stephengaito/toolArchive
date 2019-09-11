-- A Lua script

-- This lms Lua script creates the targets required to build, diff or 
-- install a ConTeXt document. 

contextDoc     = contextDoc     or { }
lms.contextDoc = lms.contextDoc or { }

local contextDefaults = { 
}

contextDoc = hMerge(contextDefaults, lms.contextDoc, contextDoc)

local function compileDocument(aDef, onExit)
  local curDir = lfs.currentdir()
  chDir(aDef.buildDir)
  --
  -- build the complete context document
  --
  executeCmd(aDef.target, 'context --nonstopmode --silent=all '..aDef.absMainDoc, onExit)
  
  chDir(curDir)
end

function createNewDocTarget(targetName, targetVarName, targetCommand)
  createNewTarget('doc-'..targetName, 'doc'..targetVarName, targetCommand)
  doNotRecurseTarget('doc-'..targetName)
  createNewTarget('bib-'..targetName, 'bib'..targetVarName, targetCommand)
  doNotRecurseTarget('bib-'..targetName)
  createNewTarget('pub-'..targetName, 'pub'..targetVarName, targetCommand)
  doNotRecurseTarget('pub-'..targetName)
end

local function gatherBibReferences(aDef, onExit)
  local curDir = lfs.currentdir()
  chDir(aDef.buildDir)
  --
  -- build the complete context document
  --
  executeLocalCmd(aDef.target, 'diSimp bib', onExit)
  
  chDir(curDir)
end

local function copyCodeFiles(ctxDef, onExit)
  local inFile, inErr = io.open(ctxDef.buildFilePath, 'r')
  if inErr then lmsError('Could not open the file ['..ctxDef.buildFilePath..']') end
  local codeFile, codeErr = io.open(ctxDef.target, 'w')
  if codeErr then lmsError('Could not open the file ['..ctxDef.target..']') end
  local mdFilePath = ctxDef.target..'.md'
  local mdFile, mdErr = io.open(mdFilePath, 'w')
  if mdErr then lmsError('Could not open the file ['..mdFilePath..']') end
  languageType = 'lua'
  local filePath = ctxDef.buildFilePath
  if     filePath:match('%.c')    then languageType = 'c'
  elseif filePath:match('%.h')    then languageType = 'c'
  elseif filePath:match('%.joy')  then languageType = 'racket'
  elseif filePath:match('%.lua')  then languageType = 'lua'
  elseif filePath:match('%.mkiv') then languageType = 'tex'
  else                                 languageType = 'lua'
  end
  
  mdFile:write("---\n")
  mdFile:write("layout: codeFile\n")
  mdFile:write("---\n")
  mdFile:write("\n{% highlight "..languageType.." linenos %}\n")

  for aLine in inFile:lines('l') do
    codeFile:write(aLine..'\n')
    mdFile:write(aLine..'\n')
  end

  mdFile:write("\n{% endhighlight %}\n\n")

  mdFile:close()
  codeFile:close()
  inFile:close()

  onExit(0,0)
end

local function copyAbstract(ctxDef, onExit)
  local inFile, inErr  = io.open(ctxDef.abstract, 'r')
  if inErr then lmsError('Could not open the file [Abstract.md]') end
  inFile:read('L')

  local outFile, outErr = io.open(ctxDef.abstractPath, 'w')
  if outErr then lmsError('Could not open the file ['..ctxDef.abstractPath..']') end
  outFile:write("---\n")
  outFile:write("layout: paperVersion\n")
  outFile:write("year: "..os.date('%Y').."\n")
  outFile:write("month: "..os.date('%m').."\n")


  for aLine in inFile:lines('l') do
    outFile:write(aLine..'\n')
  end

  inFile:close()
  outFile:close()

  onExit(0,0)
end

local function publishDocument(ctxDef, onExit)
  lmsMessage('Publishing document using pdf2htmlEX')
  ctxDef['releaseOpts'] = ctxDef['releaseOpts']  or 
      '--zoom 1.3 --embed cfij --bg-format svg --split-pages 1 '
  local pageDoc = ctxDef.mainDoc:gsub('%.tex', '-%%d.page')
  local htmlDir = ctxDef.mainDoc:gsub('%.tex', '')
  local pdfDoc  = ctxDef.mainDoc:gsub('%.tex$', '.pdf')
  local pubCmd  = 
    'pdf2htmlEX ' .. ctxDef['releaseOpts'] ..
    ' --dest-dir ' .. makePath { ctxDef['releaseDir'], 'doc', htmlDir } ..
    ' --page-filename ' .. pageDoc ..
    ' ' .. pdfDoc
  runLocalCmdIn(pubCmd, ctxDef['docDir'], function(code, signal)
    if code == 0 then
      local cpCmd = 
        'cp ' .. pdfDoc .. ' ' .. makePath{ ctxDef.releaseDir, 'doc' }
      runLocalCmdIn(cpCmd, ctxDef.docDir, onExit)
    else
      onExit(code, signal)
    end
  end)
end

local function setupCodeFilePublish(ctxDef)
  if lfs.attributes('buildDir', 'mode') == 'directory' then
    local findCmd  = 'find buildDir'
    lmsMessageVery('Dynamically finding code files using ['..findCmd..']')
    local texFileList = io.popen(findCmd)
    for buildFilePath in texFileList:lines('*l') do
      local fileTypePOpen = io.popen('file '..buildFilePath, 'r')
      local fileType = fileTypePOpen:read('*all')
      if fileType:match('text') or fileType:match('directory') then
        local codeFileTarget = buildFilePath:gsub('buildDir', 'code')
        codeFileTarget = makePath{ 
          ctxDef.releaseDir,
          codeFileTarget
        }
        if lfs.attributes(buildFilePath, 'mode') == 'directory' then
          ensurePathExists(codeFileTarget)
        else
          tInsert(_G['pub'..ctxDef['globalTargetVar']], codeFileTarget)
          target(hMerge(ctxDef, {
            target        = codeFileTarget,
            buildFilePath = buildFilePath,
            dependencies  = {
              buildFilePath,
              getBaseDirPath(codeFileTarget)
            },
            command       = copyCodeFiles,
            commandName   = 'ContextDoc::copyCodeFiles'
          }))
        end
      end
      fileTypePOpen:close()
    end
  end
end

local function setupDocumentPublish(ctxDef)

  -- determine the release directory
  --
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

  -- determine how to publish the document (using pdf2htmlEX)
  --
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
    command      = publishDocument,
    commandName  = 'ContextDoc::publishDocument'
  }))

  -- determine code path dependencies
  --
  setupCodeFilePublish(ctxDef)

  -- determine how to publish the abstract
  --
  ctxDef['abstract'] = ctxDef['abstract'] or 'Abstract.md'
  local absTarget = makePath {
    ctxDef.releaseDir,
    changeFileExtension(ctxDef.mainDoc, '.md')
  }
  ctxDef['abstractPath'] = absTarget
  tInsert(_G['pub'..ctxDef['globalTargetVar']], absTarget)
  target(hMerge(ctxDef, {
    target       = absTarget,
    dependencies = { 
      ctxDef.abstract,
      ctxDef.releaseDir
    },
    command      = copyAbstract,
    commandName  = 'ContextDoc::copyAbstract'
  }))

end

function contextDoc.targets(ctxDef)
  ctxDef = hMerge(contextDoc, ctxDef)
  ctxDef.creator = 'contextDoc-targets'
  
  findDocumentsIn(ctxDef, aAppend({ ctxDef.docDir }, ctxDef.lmsfileSubDirs ))

  ctxDef.compileDocument = compileDocument
  
  ctxDef.buildDir = ctxDef.buildDir or 'buildDir'
  ctxDef.buildDir = makePath { ctxDef.buildDir, dirPrefix }
  ensurePathExists(ctxDef.buildDir)
  tInsert(ctxDef.dependencies, ctxDef.buildDir)

  local pdfMainDoc = ctxDef.mainDoc:gsub('%.tex$', '.pdf')
  local docTarget =
    makePath{ ctxDef.buildDir, pdfMainDoc }
  local absMainDocPath = 
    makePath{ lfs.currentdir(), ctxDef.docDir, ctxDef.mainDoc }
  ctxDef['globalTargetVar'] = ctxDef['globalTargetVar'] or 'Targets'
  tInsert(_G['doc'..ctxDef['globalTargetVar']], docTarget)
  target(hMerge(ctxDef, {
    target      = docTarget,
    absMainDoc  = absMainDocPath,
    command     = compileDocument,
    commandName = 'ContextDoc::compileDocument'
  }))

  local bibMainDoc = ctxDef.mainDoc:gsub('%.tex$', 'Bib.lua')
  local bibTarget = makePath{ ctxDef.buildDir, bibMainDoc }
  print(bibTarget)
  tInsert(_G['bib'..ctxDef['globalTargetVar']], bibTarget)
  target(hMerge(ctxDef, {
    target       = bibTarget,
    dependencies = { docTarget },
    command      = gatherBibReferences,
    commandName  = 'ContextDef::gatherBibReferences'
  }))

  setupDocumentPublish(ctxDef)

  tInsert(clobberTargets, nameClobberTarget(docTarget))
  tInsert(clobberTargets,
    nameClobberTarget(docTarget:gsub('%.pdf', 'Bib.lua')))

  tInsert(cleanTargets, 
    nameCleanTarget(docTarget:gsub('%.pdf', '.log')))
  tInsert(cleanTargets, 
    nameCleanTarget(docTarget:gsub('%.pdf$', '.tuc')))

  return ctxDef
end

