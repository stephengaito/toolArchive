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
  rndModel <- addSpecies(   rndModel, speciesName,
                            growthRate = 0.2, carryingCapacity = 25, timeLag = 5,
                            reintroductionProb = 0.9, reintroductionSize = 1
                            )
  rndModel <- addSpeciesStd(rndModel, speciesName,
                            growthRate = 0.1,  carryingCapacity = 5, timeLag = 1,
                            reintroductionProb = 0.1, reintroductionSize = 0.1)
}
#
# Build middle trophic levels (consumers/resources)
#
for (trophicLevel in 2:4) {
  for (species in 1:5) {
    speciesName <- buildSpeciesName(trophicLevel, species)
    rndModel <- addSpecies(   rndModel, speciesName,
                              halfSaturation = 40,
                              reintroductionProb = (6 - trophicLevel) * 0.1,
                              reintroductionSize = (6 - trophicLevel) * 0.1)
    rndModel <- addSpeciesStd(rndModel, speciesName,
                              halfSaturation = 5,
                              reintroductionProb = 0.1, reintroductionSize = 0.1)
  }
}
#
# Build trophic level 5 (top-predators)
#
for (species in 1:5) {
  speciesName <- buildSpeciesName(5, species)
  rndModel <- addSpecies(   rndModel, speciesName,
                            mortality = 10, halfSaturation = 40,
                            reintroductionProb = 0.9, reintroductionSize = 0.1
  )
  rndModel <- addSpeciesStd(rndModel, speciesName,
                            mortality = 0.1, halfSaturation = 5,
                            reintroductionProb = 0.1, reintroductionSize = 0.1)
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
                                      attackRate = 0.01, conversionRate = 0.5, timeLag = 5)
        rndModel <- addInteractionStd(rndModel, predSpeciesName, preySpeciesName,
                                      attackRate = 0.005, conversionRate = 0.05, timeLag = 1)
      }
    }
  }
}

#print(rndModel)

bIV <- c(         10,     10,     10,     10,     10) # tl1
bIV <- c(bIV,    0.5,    0.5,    0.5,    0.5,    0.5) # tl2
bIV <- c(bIV,   0.25,   0.25,   0.25,   0.25,   0.25) # tl3
bIV <- c(bIV,  0.125,  0.125,  0.125,  0.125,  0.125) # tl4
bIV <- c(bIV, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625) # tl5

#numRuns <- 99999
numRuns <- 9
fileNameFormat <- "multiTrophic/r%05dtl%d"
stepSize <- 0.1
stepsPerSample <- 10
numSamples <- 1000

system("rm -rf multiTrophic")
system("mkdir -p multiTrophic")


for (curRun in 0:numRuns) {
  print(curRun)
  model <- varyModel(rndModel)
  print(model)
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
