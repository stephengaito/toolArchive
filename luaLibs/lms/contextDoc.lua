-- A Lua script

-- This lms Lua script creates the targets required to build, diff or 
-- install a ConTeXt document. 

contextDoc     = contextDoc     or { }
lms.contextDoc = lms.contextDoc or { }

local contextDefaults = { 
}

contextDoc = hMerge(contextDefaults, lms.contextDoc, contextDoc)

local function publishCodeFile(ctxDef, onExit)
  -- To publish a code file we need:
  --   1. the (relative) path to the code file (ctxDef.builtCodeFilePath)
  --   2. the absolute path to the published raw code file (ctxDef.target)
    
  -- Open the original code file
  --
  local inFile, inErr = io.open(ctxDef.builtCodeFilePath, 'r')
  if inErr then
    os.execute("ls -la "..getParentPath(ctxDef.builtCodeFilePath))
    lmsError('Could not open the file ['..ctxDef.builtCodeFilePath..']')
  end
  
  -- Open the published raw code file
  --
  local codeFile, codeErr = io.open(ctxDef.target, 'w')
  if codeErr then
    lmsError('Could not open the file ['..ctxDef.target..']')
  end
  
  -- Open the published Markdown/Highlighted version
  -- of the code file
  --
  local mdFilePath = ctxDef.target..'.md'
  local mdFile, mdErr = io.open(mdFilePath, 'w')
  if mdErr then
    lmsError('Could not open the file ['..mdFilePath..']')
  end
  
  -- Determine the language type so the Markdown version
  -- of the code file can be properly highlighted
  --
  languageType = 'lua'
  local filePath = ctxDef.builtCodeFilePath
  if     filePath:match('%.c')    then languageType = 'c'
  elseif filePath:match('%.h')    then languageType = 'c'
  elseif filePath:match('%.joy')  then languageType = 'racket'
  elseif filePath:match('%.lua')  then languageType = 'lua'
  elseif filePath:match('%.mkiv') then languageType = 'tex'
  else                                 languageType = 'lua'
  end
  
  -- Write out the Jekyll Markdown header
  mdFile:write("---\n")
  mdFile:write("layout: codeFile\n")
  mdFile:write("---\n")
  mdFile:write("\n{% highlight "..languageType.." linenos %}\n")

  -- Copy the file line by line
  for aLine in inFile:lines('l') do
    codeFile:write(aLine..'\n')
    mdFile:write(aLine..'\n')
  end

  -- End the Jekyll Markdown highlighting
  mdFile:write("\n{% endhighlight %}\n\n")

  -- Close the files
  mdFile:close()
  codeFile:close()
  inFile:close()

  -- Done!
  onExit(0,0)
end

local function setupPublishCodeFile(ctxDef, aSrcFile, builtSrcFilePath)
  local codeFileTarget = makePath{ 
    ctxDef.releaseDir,
    'code',
    aSrcFile
  }
  local codeFileDir = getParentPath(codeFileTarget)

  if not getTargetFor(codeFileTarget) then
    ensurePathExists(codeFileDir)
    appendToMainTarget(codeFileTarget, 'pub')
    target(hMerge(ctxDef, {
      target            = codeFileTarget,
      builtCodeFilePath = builtSrcFilePath,
      dependencies  = {
        builtSrcFilePath,
        codeFileDir
      },
      command       = publishCodeFile,
      commandName   = 'ContextDoc::publishCodeFile'
    }))
  end
end

local function publishAbstract(ctxDef, onExit)
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

local function setupPublishDocument(ctxDef, pdfMainDoc, pdfBuildTarget)

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
    command      = publishAbstract,
    commandName  = 'ContextDoc::publishAbstract'
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

  setupPublishDocument(ctxDef, pdfMainDoc, docTarget)
  ctxDef.setupPublishCodeFile = setupPublishCodeFile

  appendToClobber(docTarget)
  appendToClobber(changeFileExtension(docTarget, 'Bib.lua'))

  appendToClean(changeFileExtension(docTarget, '.log'))
  appendToClean(changeFileExtension(docTarget, '.tuc'))

  return ctxDef
end

