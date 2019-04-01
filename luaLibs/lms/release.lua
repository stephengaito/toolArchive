-- A lua script

-- This lua script creates the targets required to build a release

-- THIS IS CURRENTLY INCOMPLETE AS IT IS NOT NEEDED FOR CURRENT PURPOSES
-- WILL BE USED AGAIN FOR JOYLOL and later for PROTOJOYLOL

release     = release     or  { }
lms.release = lms.release or { }

local releaseDefaults = {
}

release = hMerge(releaseDefaults, lms.release, release)

-- dirsToIgnore = '|archive|tmp|installDir|tex|bin|'
dirsToIgnore = '|archive|tmp|installDir|bin|'

function findReleaseFiles(dir, filesIO)
  local dirList = { }
  for anEntry in lfs.dir(dir) do
    table.insert(dirList, anEntry)
  end
  table.sort(dirList)
  for i, anEntry in ipairs(dirList) do
    if  not anEntry:find('^%.')
    and not dirsToIgnore:find(anEntry, 1, true)
    then
      fullPath = makePath{dir, anEntry}
      if lfs.attributes(fullPath, 'mode') == 'directory' then
        findReleaseFiles(fullPath, filesIO)
      else
        if string.find(fullPath, '%.tex$')
        or string.find(fullPath, '%.mkiv$')
        or string.find(fullPath, '%.lua$')
        or string.find(fullPath, '%.c$')
        or string.find(fullPath, '%.h$')
        or string.find(fullPath, '%.joy$')
        or string.find(fullPath, testProbe)
        or string.find(fullPath, 'lmsfile$')
        then
          filesIO:write(fullPath..'\n')
        end
      end
    end
  end
end

function release.createReleaseFile()
  lfs.mkdir('releases')

  local gitHead = io.open('.git/refs/heads/master')
  local shortHash = gitHead:read(7)
  gitHead:close()
  local releaseName = makePath{
    'releases',
    'release-'..os.date('%F')..'-'..shortHash..'.tar.gz'
  }

  local filesIOName = os.tmpname()
  local filesIO = io.open(filesIOName, 'w')
  findReleaseFiles('.', filesIO)
  filesIO:close()

  os.execute('tar czvf '..releaseName..' -T '..filesIOName)

  os.remove(filesIOName)
end
