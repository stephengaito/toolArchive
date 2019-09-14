-- A Lua script

-- This lms Lua script creates the targets required to build, diff or 
-- install a ConTeXt document. 

contextDoc     = contextDoc     or { }
lms.contextDoc = lms.contextDoc or { }

local contextDefaults = { 
}

contextDoc = hMerge(contextDefaults, lms.contextDoc, contextDoc)

local function copyCodeFiles(ctxDef, onExit)
  print("copyCodeFiles in ["..lfs.currentdir().."]")
  print(prettyPrint(ctxDef))
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
  print("copyAbstract in ["..lfs.currentdir().."]")
  print(prettyPrint(ctxDef))
  local inFile, inErr  = io.open(ctxDef.abstractPath, 'r')
  if inErr then lmsError('Could not open the file ['..ctxDef.abstractPath..']') end
  inFile:read('L')

  local outFile, outErr = io.open(ctxDef.abstractTargetPath, 'w')
  if outErr then lmsError('Could not open the file ['..ctxDef.abstractTargetPath..']') end
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

local function setupCodeFilePublish(ctxDef)
  print("setupCodeFilePublish in ["..lfs.currentdir().."]")
  print(prettyPrint(ctxDef))
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

local function setupDocumentPublish(ctxDef, pdfMainDoc, pdfBuildTarget)

  -- determine the release directory
  --
  ctxDef.releaseType = ctxDef.releaseType or 'workingDraft'
  if type(ctxDef.releaseDir) == 'nil' then
    print('WARNING: No document publishing release directory specified (using ".")')
    ctxDef.releaseDir = '.'
  end
  ctxDef.releasePath = ctxDef.releasePath or makePath{ projectName, dirPrefix }
  ctxDef.releaseDir = makePath{
    ctxDef.releaseDir,
    ctxDef.releaseType,
    os.date('%Y'),
    os.date('%m'),
    ctxDef.releasePath
  }
  ensurePathExists(ctxDef.releaseDir)

  -- determine how to publish the pdf document (using pdf2htmlEX)
  --
  local pdfReleaseTarget = makePath{ ctxDef.releaseDir, 'doc', pdfMainDoc }
  local pdfReleaseDir    = getParentPath(pdfReleaseTarget)
  ensurePathExists(pdfReleaseDir)
  
  appendToMainTarget(pdfReleaseTarget, 'pub')
  target(hMerge(ctxDef, {
    target       = pdfReleaseTarget,
    dependencies = { 
      pdfBuildTarget,
      pdfReleaseDir,
    },
    command      = 'cp '..pdfBuildTarget..' '..pdfReleaseDir,
    commandName  = 'ContextDoc::publishPDF'
  }))

  -- determine how to publish the html document (using pdf2htmlEX)
  --
  -- We start with the main html page for the overall document
  --
  local htmlDoc           = changeFileExtension(pdfMainDoc, '.html')
  local htmlDirName       = changeFileExtension(pdfMainDoc, '')
  local htmlReleaseTarget = makePath {
    ctxDef.releaseDir,
    'doc',
    htmlDirName,
    htmlDoc
  }
  local htmlReleaseDir    = getParentPath(htmlReleaseTarget)
  ensurePathExists(htmlReleaseDir)
  
  -- Now we determine the individual html pages
  --
  ctxDef.releaseOpts = ctxDef.releaseOpts  or 
      '--zoom 1.3 --embed cfij --bg-format svg --split-pages 1 '
  local pagesDoc     = changeFileExtension(pdfMainDoc,'-%%d.page')
  
  appendToMainTarget(htmlReleaseTarget, 'pub')
  target(hMerge(ctxDef, {
    target       = htmlReleaseTarget,
    dependencies = { 
      pdfBuildTarget,
      htmlReleaseDir
    },
    command      = 'pdf2htmlEX ' .. ctxDef.releaseOpts ..
      ' --dest-dir ' .. htmlReleaseDir ..
      ' --page-filename ' .. pagesDoc ..
      ' ' .. pdfBuildTarget,
    commandName  = 'ContextDoc::publishPDF2HTML'
  }))

  -- determine how to publish the abstract
  --
  ctxDef.abstract = ctxDef.abstract or 'Abstract.md'
  local abstractPath = makePath {
    dirPrefix,
    ctxDef.abstract
  }
  ctxDef.abstractPath = abstractPath
  
  local absTarget = makePath {
    ctxDef.releaseDir,
    changeFileExtension(ctxDef.mainDoc, '.md')
  }
  ctxDef.abstractTargetPath = absTarget
  appendToMainTarget(absTarget, 'pub')
  target(hMerge(ctxDef, {
    target       = absTarget,
    dependencies = { 
      ctxDef.abstractPath,
      ctxDef.releaseDir
    },
    command      = copyAbstract,
    commandName  = 'ContextDoc::copyAbstract'
  }))
end

function contextDoc.targets(ctxDef)
  ctxDef = hMerge(contextDoc, ctxDef)
  ctxDef.creator = 'contextDoc-targets'
  
  findDocumentsIn(
    ctxDef,
    'docFiles',
    ".*\\.tex\\|.*Bib\\.lua",
    aAppend({ ctxDef.docDir }, ctxDef.lmsfileSubDirs )
  )
  
  -- Ensure we have the correct build directory for this sub-project

  ctxDef.buildDir = ctxDef.buildDir or 'buildDir'
  ctxDef.buildDir = makePath { ctxDef.buildDir, dirPrefix }
  ensurePathExists(ctxDef.buildDir)
  aInsertOnce(ctxDef.dependencies, ctxDef.buildDir)

  -- Now set up the various document targets
  
  local pdfMainDoc = changeFileExtension(ctxDef.mainDoc, '.pdf')
  local docTarget  =  makePath{ ctxDef.buildDir, pdfMainDoc }
  local absMainDocPath = 
    makePath{ lfs.currentdir(), ctxDef.docDir, ctxDef.mainDoc }
  appendToMainTarget(docTarget, 'doc')
  target(hMerge(ctxDef, {
    target      = docTarget,
    command     = 'context --nonstopmode --silent=all '..absMainDocPath,
    commandDir   = makePath { projectDir, ctxDef.buildDir },
    commandName = 'ContextDoc::compileDocument'
  }))

  local bibMainDoc = changeFileExtension(ctxDef.mainDoc, 'Bib.lua')
  local bibTarget = makePath{ ctxDef.buildDir, bibMainDoc }
  appendToMainTarget(bibTarget, 'bib')
  target(hMerge(ctxDef, {
    target       = bibTarget,
    dependencies = { docTarget },
    command      = 'diSimp bib',
    commandDir   = makePath { projectDir, ctxDef.buildDir },
    commandName  = 'ContextDef::gatherBibReferences'
  }))

  setupDocumentPublish(ctxDef, pdfMainDoc, docTarget)
  ctxDef.setupCodeFilePublish = setupCodeFilePublish

  appendToClobber(docTarget)
  appendToClobber(changeFileExtension(docTarget, 'Bib.lua'))

  appendToClean(changeFileExtension(docTarget, '.log'))
  appendToClean(changeFileExtension(docTarget, '.tuc'))

  return ctxDef
end

