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
    # work on predators
    #
    predatorInteractions <- interactions[interactions$predator==speciesName,]
    numPredators <- nrow(predatorInteractions)
    if (0 < numPredators) {
      predators <- as.integer(lapply(predatorInteractions$prey, speciesIndex))
      .C_setPredatorCoefficients(cModel, 
                                 row - 1, 
                                 predators - 1, 
                                 predatorInteractions$attackRate,
                                 predatorInteractions$conversionRate)
    }
    #
    # work on prey
    #
    preyInteractions     <- interactions[interactions$prey==speciesName,]
    numPrey <- nrow(preyInteractions)
    if (0 < numPrey) {
      prey <- as.integer(lapply(preyInteractions$predator, speciesIndex))
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
#' @export
integrateModel <- function(model, stepSize, maxIterations, initialValues) {
  cModel <- .R_buildCModel(model)
  numSpecies <- numSpeciesInModel(model)
  results <- matrix(NA_real_, nrow = maxIterations, ncol = numSpecies)
  .C_integrateEuler(cModel, stepSize, maxIterations, initialValues, results)
}