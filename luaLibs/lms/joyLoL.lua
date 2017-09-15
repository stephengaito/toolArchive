-- A Lua script

-- This lms Lua script creates the targets required to build, diff or 
-- install a suite of JoyLoL CoAlgebra extensions (shared libraries). 

require 'lms.c'

joylol = { }

docTargets     = docTargets     or { }
buildTargets   = buildTargets   or { }
installTargets = installTargets or { }
diffTargets    = diffTargets    or { }
testTargets    = testTargets    or { }

local function installCoAlg(jDef)
  print('need to install'..jDef.target)
end

function joylol.targets(jDef)

  for i, aCoAlg in ipairs(jDef.coAlgs) do

    local coAlgTarget = jDef.buildDir..'/'..aCoAlg..'.so'
    local cDependencies = { }
    for j, aSrcFile in ipairs(jDef.srcFiles) do
      tInsert(cDependencies, jDef.buildDir..'/'..aSrcFile)
    end
    c.shared(hMerge(jDef, {
      target = coAlgTarget,
      dependencies = c.collectCDependencies(cDependencies),
      needs = { 'lua5.2' },
    }))

    local installTarget = 'install-'..aCoAlg
    tInsert(installTargets, installTarget)
    target(hMerge(jDef, {
      target = installTarget,
      dependencies = { coAlgTarget },
      command = installCoAlg
    }))
  end
end