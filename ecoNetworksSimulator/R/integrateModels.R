# Defactor a data.frame
#
#' @export
.R_defactor <- function(aDF) {
  # taken from http://stackoverflow.com/questions/2851015/convert-data-frame-columns-from-factors-to-characters/2853231#2853231
  factors <- sapply(aDF, is.factor)
  aDF[factors] <- lapply(aDF[factors], as.character)
  aDF
}

# Convert an R-Model into a C-Model
# 
# @param model the model to convert
# @return the C-model
#' @export
.R_buildCModel <- function(model) {
  if (!isModel(model)) {
    stop("the model provided is not valid")
  }
  species <- .R_defactor(model$species)
  speciesIndex <- function(x) { which(species$species==x) }
  interactions <- .R_defactor(model$interactions)
  numSpecies <- nrow(species)
  cModel <- .C_newSpeciesTable(numSpecies)
  for (row in 1:numSpecies) {
    aRow <- species[row,c("growthRate", "carryingCapacity", "mortality", "halfSaturation")]
    aRow <- as.numeric(aRow)
    .C_setSpeciesValues(cModel, row-1, aRow)
    speciesName <- species[row,"species"]
    #
    # work on predators (who eats ME?)
    #
    predatorInteractions <- interactions[interactions$prey==speciesName,]
    numPredators <- nrow(predatorInteractions)
    if (0 < numPredators) {
      predators <- as.integer(lapply(predatorInteractions$predator, speciesIndex))
      .C_setPredatorCoefficients(cModel, 
                                 row - 1, 
                                 predators - 1, 
                                 predatorInteractions$attackRate,
                                 predatorInteractions$conversionRate)
    }
    #
    # work on prey (who do I eat?)
    #
    preyInteractions     <- interactions[interactions$predator==speciesName,]
    numPrey <- nrow(preyInteractions)
    if (0 < numPrey) {
      prey <- as.integer(lapply(preyInteractions$prey, speciesIndex))
      .C_setPreyCoefficients(cModel, 
                             row - 1, 
                             prey - 1,
                             preyInteractions$attackRate,
                             preyInteractions$conversionRate)
    }
  }
  cModel
}

#' Integrate a model
#' 
#' @param model the model to integrate
#' @param stepSize the size of a single time step
#' @param stepsPerSample the number of steps to compute between each sample 
#'   collected
#' @param numSamples the number of samples to collect in this integration
#' @param initialValues the initial values for this integration
#' @param numSamplesBetweenInteruptChecks the number of samples collected between
#'   each check for user interupts. A small number will slow down the
#'   integration, while a large number will inhibit the user's ability to stop a
#'   long running integration. The default is 100 samples.
#' @return the results of the integration as a matrix of numSamples row and one 
#'   colum for each species.
#' @export
integrateModel <- function(model, 
                           stepSize,
                           stepsPerSample,
                           numSamples,
                           initialValues,
                           numSamplesBetweenInteruptChecks = 100) {
  cModel <- .R_buildCModel(model)
  numSpecies <- numSpeciesInModel(model)
  workingResultsMask <- 15
  numWorkingResults <- workingResultsMask + 1
  workingResults <- matrix(NA_real_, nrow = numWorkingResults, ncol = numSpecies)
  results <- matrix(NA_real_, nrow = numSamples, ncol = numSpecies)
  #print(tail(results))
  if (numSamples < numSamplesBetweenInteruptChecks) {
    numSamplesBetweenInteruptChecks = numSamples
  }
  results <- .C_integrateEuler(cModel,
                               stepSize,
                               stepsPerSample,
                               numSamples,
                               numSamplesBetweenInteruptChecks,
                               initialValues,
                               workingResultsMask,
                               workingResults,
                               results)
  #print(tail(results))
  results
}