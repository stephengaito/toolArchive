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
      predatorCoeffs <- predatorInteractions$attackRate * predatorInteractions$conversionRate
      predators <- as.integer(lapply(predatorInteractions$prey, speciesIndex))
      predatorNums <- predators - 1
      .C_setPredatorCoefficients(cModel, row-1, predatorNums, predatorCoeffs)
    }
    #
    # work on prey
    #
    preyInteractions     <- interactions[interactions$prey==speciesName,]
    numPrey <- nrow(preyInteractions)
    if (0 < numPrey) {
      preyCoeffs <- preyInteractions$attackRate
      prey <- as.integer(lapply(preyInteractions$predator, speciesIndex))
      preyNums <- prey - 1
      .C_setPreyCoefficients(cModel, row-1, preyNums, preyCoeffs)
    }
  }
  cModel
}

#' Integrate a model
#' 
#' @param model the model to integrate
#' @export
integrateModel <- function(model) {
  cModel <- .R_buildCModel(model)
}