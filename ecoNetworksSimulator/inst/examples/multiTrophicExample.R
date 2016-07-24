# This is a complex multi trophic model 
#

library("ecoNetworksSimulator")

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

#print(rndModel)

bIV <- c(        10,    10,    10,    10,    10) # tl1
bIV <- c(bIV,     5,     5,     5,     5,     5) # tl2
bIV <- c(bIV,   2.5,   2.5,   2.5,   2.5,   2.5) # tl3
bIV <- c(bIV,  1.25,  1.25,  1.25,  1.25,  1.25) # tl4
bIV <- c(bIV, 0.625, 0.625, 0.625, 0.625, 0.625) # tl5

numRuns <- 10000
fileNameFormat <- "multiTrophic/r%05dtl%d"
stepSize <- 0.1
stepsPerSample <- 10
numSamples <- 1000

for (curRun in 1:numRuns) {
  print(curRun)
  model <- varyModel(rndModel)
  modelSpecies <- model$species
  initialValues <- newInitialValues(model, bIV)
  #print(initialValues)
  results <- integrateModel(
    model, stepSize, stepsPerSample, numSamples, initialValues)
  for (trophicLevel in 2:5) {
    baseSpeciesNum <- (trophicLevel - 1) * 5
    sum <- 0.0
    for (species in 1:5) {
      speciesNum <- baseSpeciesNum + species
      sum <- sum +
        modelSpecies[[speciesNum, "mortality"]] * results[numSamples, speciesNum]
    }
    if (sum < 1e-5) {
      break
    }
    fileNameBase <- sprintf(
      fileNameFormat, as.integer(curRun), as.integer(trophicLevel)) 
    print(fileNameBase)
    write.csv(model$species, paste(fileNameBase, "-species.csv", sep = ""))
    write.csv(model$interactions, paste(fileNameBase, "-interactions.csv", sep = ""))
    write.csv(results, paste(fileNameBase, "-results.csv", sep = ""))
    #print(tail(results))
  }
}
