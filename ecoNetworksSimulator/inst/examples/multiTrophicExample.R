# This is a complex multi trophic model 
#
buildSpeciesName <- function(trophicLevel, speciesNum) {
  paste("tl", trophicLevel, "s", speciesNum, sep = "")
}
rndModel <- newTrophicModel()
#
# Build trophic level 1 (resouces)
#
for (species in 1:5) {
  speciesName <- buildSpeciesName(1, species)
  rndModel <- addSpecies(   rndModel, speciesName, growthRate = 0.2, carryingCapacity = 25, timeLag = 5)
  rndModel <- addSpeciesStd(rndModel, speciesName, growthRate = 0.1,  carryingCapacity = 5, timeLag = 1)
}
#
# Build all other trophic levels (consumers)
#
for (trophicLevel in 2:5) {
  for (species in 1:5) {
    speciesName <- buildSpeciesName(trophicLevel, species)
    rndModel <- addSpecies(   rndModel, speciesName, mortality = 0.2, halfSaturation = 40)
    rndModel <- addSpeciesStd(rndModel, speciesName, mortality = 0.1, halfSaturation = 5)
  }
}
#
# Add interactions
#
for (predTrophicLevel in 2:5) {
  preyTrophicLevel <- predTrophicLevel - 1
  for (predSpecies in 1:5) {
    predSpeciesName <- buildSpeciesName(predTrophicLevel, predSpecies)
    for (preySpecies in 1:5) {
      if (predSpecies != preySpecies) {
        preySpeciesName <- buildSpeciesName(preyTrophicLevel, preySpecies)
        rndModel <- addInteraction(   rndModel, predSpeciesName, preySpeciesName,
                                      attackRate = 0.2, conversionRate = 0.35, timeLag = 5)
        rndModel <- addInteractionStd(rndModel, predSpeciesName, preySpeciesName,
                                      attackRate = 0.1, conversionRate = 0.05, timeLag = 1)
      }
    }
  }
}

print(rndModel)