# Create a species table
# 
# A species table is a data frame which has one row for each species.
# 
# The columns consist of species (name), growthRate (real), carryingCapacity
# (real), and mortality (real).
# 
#' @export
.R_newSpeciesTable <- function() {
  data.frame(species=character(),
             growthRate=numeric(),
             carryingCapacity=numeric(),
             timeLag=numeric(),
             mortality=numeric(),
             halfSaturation=numeric(),
             reintroductionProb=numeric(),
             reintroductionSize=numeric())
}

# Check that a table is a species table
# 
# @param speciesTable a potential speciesTable
# @return true if the table provided is a speciesTable
#' @export
.R_isSpeciesTable <- function(speciesTable){
  is.data.frame(speciesTable) && 
    colnames(speciesTable) == 
      c("species", "growthRate", "carryingCapacity", "timeLag", 
        "mortality", "halfSaturation", 
        "reintroductionProb", "reintroducitonSize")
}

# Returns the number of species in a species table
# 
# @param speciesTable the species table
# @return the number of species in the species table
#' @export
.R_numSpeciesInTable <- function(speciesTable) {
  nrow(speciesTable)  
}

#' Add a species to an existing trophic model
#' 
#' @param model an existing trophic model which is to be extended
#' @param name the name of the species
#' @param growthRate the growth rate of the species (could be 0.0)
#' @param carryingCapacity the carrying capacity of the species (could be NA)
#' @param timeLag the time lag for the carrying capacity term
#' @param mortality the mortality of the species (could be NA)
#' @param halfSaturation the half saturation point for the predator's predation
#' @param reintroductionProb the probability that a species with zero biomass
#'   will get reintroduced at a small value
#' @param reintroductionSize the amount of biomass reintroduced
#' @export
addSpecies <- function(model, 
                       name, 
                       growthRate=0.0, 
                       carryingCapacity=NA_real_, 
                       timeLag=0.0,
                       mortality=0.0,
                       halfSaturation=NA_real_,
                       reintroductionProb=0.0,
                       reintroductionSize=0.0){
  model$species <- 
    rbind(model$species, 
          data.frame(species=name,
                     growthRate=growthRate,
                     carryingCapacity=carryingCapacity,
                     timeLag=timeLag,
                     mortality=mortality,
                     halfSaturation=halfSaturation,
                     reintroductionProb=reintroductionProb,
                     reintroductionSize=reintroductionSize))
  model
}

#' Add a species standard deviations an existing trophic model
#' 
#' @param model an existing trophic model which is to be extended
#' @param name the name of the species
#' @param growthRate the growth rate of the species (could be 0.0)
#' @param carryingCapacity the carrying capacity of the species (could be NA)
#' @param timeLag the time lag for the carrying capacity term
#' @param mortality the mortality of the species (could be NA)
#' @param halfSaturation the half saturation point for the predator's predation
#' @param reintroductionProb the probability that a species with zero biomass
#'   will get reintroduced at a small value
#' @param reintroductionSize the amount of biomass reintroduced
#' @export
addSpeciesStd <- function(model, 
                          name, 
                          growthRate=0.0, 
                          carryingCapacity=NA_real_, 
                          timeLag=0.0,
                          mortality=0.0,
                          halfSaturation=NA_real_,
                          reintroductionProb=0.0,
                          reintroductionSize=0.0){
  speciesStd <- model$speciesStd
  if (is.null(speciesStd)) { speciesStd <- .R_newSpeciesTable()}
  model$speciesStd <- 
    rbind(speciesStd, 
          data.frame(species=name,
                     growthRate=growthRate,
                     carryingCapacity=carryingCapacity,
                     timeLag=timeLag,
                     mortality=mortality,
                     halfSaturation=halfSaturation,
                     reintroductionProb=reintroductionProb,
                     reintroductionSize=reintroductionSize))
  model
}

# Create table of interactions
# 
# A table of interactions is a data frame which has one row for each species 
# interaction
# 
# The columns are predator (name), prey (name), attackRate (real), and 
# conversionRate (real)
# 
#' @export
.R_newInteractionsTable <- function() {
  data.frame(predator=character(),
             prey=character(),
             attackRate=numeric(),
             conversionRate=numeric(),
             deathRate=numeric(),
             timeLag=numeric())
}

# Check that a table is a interactions table
# 
# @param interactionsTable a potential interactionsTable
# @return true if the table provided is a interactionsTable
#' @export
.R_isInteractionsTable <- function(interactionsTable){
  is.data.frame(interactionsTable) && 
    colnames(interactionsTable) == 
      c("predator", "prey", "attackRate", "conversionRate", "deathRate", "timeLag")
}

#' Add a species interaction to an existing trophic model
#' 
#' @param model an existing trophic model which is to be extended
#' @param predator the name of the predator species
#' @param prey the name of the prey species
#' @param attackRate the rate at which the predator attacks the prey
#' @param conversionRate the rate at which the predator converts its prey into
#'   itself
#' @param deathRate the rate at which the prey dies due to predator's predation
#' @param timeLag the time lag for the prey species
#' @export
addInteraction <- function(model, 
                           predator, 
                           prey,
                           attackRate=0.0, 
                           conversionRate=0.0,
                           deathRate=1.0,
                           timeLag=0.0) {
  model$interactions <- 
    rbind(model$interactions, 
          data.frame(predator=predator,
                     prey=prey,
                     attackRate=attackRate,
                     conversionRate=conversionRate,
                     deathRate=deathRate,
                     timeLag=timeLag))
  model
}
#' Add a species interaction to an existing trophic model
#' 
#' @param model an existing trophic model which is to be extended
#' @param predator the name of the predator species
#' @param prey the name of the prey species
#' @param attackRate the rate at which the predator attacks the prey
#' @param conversionRate the rate at which the predator converts its prey into
#'   itself
#' @param deathRate the rate at which the prey dies due to predator's predation
#' @param timeLag the time lag for the prey species
#' @export
addInteractionStd <- function(model, 
                              predator, 
                              prey,
                              attackRate=0.0, 
                              conversionRate=0.0,
                              deathRate=1.0,
                              timeLag=0.0) {
  interactionStds <- model$interactionStds
  if (is.null(interactionStds)) { interactionStds <- .R_newInteractionsTable()}
  model$interactionStds <- 
    rbind(interactionStds, 
          data.frame(predator=predator,
                     prey=prey,
                     attackRate=attackRate,
                     conversionRate=conversionRate,
                     deathRate=deathRate,
                     timeLag=timeLag))
  model
}

# Check that a speciesTable and a given interactionsTable are related.
# 
# @param speciesTable a species table
# @param interactionsTable a table of species interactions
# @return true if all of the species in the interaction table are defined in
#   the species table.
#' @export
.R_areRelated <- function(speciesTable, interactionsTable) {
  .R_isSpeciesTable(speciesTable) &&
    .R_isInteractionsTable(interactionsTable) &&
    all(levels(interactionsTable$predator) %in% levels(speciesTable$species)) &&
    all(levels(interactionsTable$prey) %in% levels(speciesTable$species))
}

#' Create a trophic model
#' 
#' A model is an list of tables
#' 
#' @export
newTrophicModel <- function() {
  speciesTable <- .R_newSpeciesTable()
  interactionsTable <- .R_newInteractionsTable()
  .R_newTrophicModel(speciesTable, interactionsTable)
}

# Create a new trophic model from species and interactions tables
#
# @param species the species table
# @param interactions the interactions table
# @return returns a trophic model
#' @export
.R_newTrophicModel <- function(speciesTable, interactionsTable) {
  list(species=speciesTable, interactions=interactionsTable)
}

#' Check that a trophic model is valid
#' 
#' @param model the trophic model to verify
#' @return true if the trophic model is valid
#' @export
isTrophicModel <- function(model) {
  if (!.R_isSpeciesTable(model$species)) {
    stop("the model does not have a valid species table")
  }
  if (!.R_isInteractionsTable(model$interactions)) {
    stop("the model does not have a valid interactions table")
  }
  if (!.R_areRelated(model$species, model$interactions)) {
    stop("the model's species and interactions tables are not related")
  }
  TRUE
}

#' Returns the number of species in a species table
#' 
#' @param model the model
#' @return the number of species in the model
#' @export
numSpeciesInModel <- function(model) {
  nrow(model$species)  
}

#' Returns the maximum time lag in the model
#' 
#' @param  aModel the model
#' @return the maximum time lag in the model
#' @export
maximumTimeLag <- function(aModel) {
  max( max(aModel$species$timeLag), max(aModel$interactions$timeLag) )
}

# Create a new normal trophic model from species and interactions tables
#
# @param speciesTable the species table
# @param speciesStdTable standard deviations table
# @param interactionsTable the interactions table
# @param interactionStdsTable standard deviations table
# @return returns a normal trophic model
#' @export
.R_newNormalTrophicModel <- function(speciesTable, speciesStdTable, 
                                     interactionsTable, interactionStdsTable) {
  list(species = speciesTable, speciesStd = speciesStdTable,
       interactions = interactionsTable, interactionStds = interactionStdsTable)
}

#' Check that a normal trophic model is valid
#' 
#' 
#' @param model normal trophic mode to verify
#' @return true if the normal trophic model is valid
#' @export
isNormalTrophicModel <- function(model) {
  if (!.R_isSpeciesTable(model$species)) {
    stop("the table of species means provided is incorrectly formated")
  }
  if (!.R_isSpeciesTable(model$speciesStd)) {
    stop("the table of species standard deviations provided is incorrectly formated")
  }
  if (!.R_isInteractionsTable(model$interactions)) {
    stop("the table of interaction means provided is incorrectly formated")
  }
  if (!.R_isInteractionsTable(model$interactionStds)) {
    stop("the table of interaction standard deviations provided is incorrectly formated")
  }
  if (!.R_areRelated(model$species, model$interactions)) {
    stop("some of the predator or prey species in the interations table are not defined in the species table")
  }
  TRUE
}

# Choose a normally distrubuted random number as a parameter
# 
# @param aMean
# @param aStd
# @return a normally distributed random number with mean aMean and standard
#   deviation aStd, OR aMean if either aMean or aStd is not a numeric or is NA
#' @importFrom stats rnorm
#' @export
.R_varyParameter <- function(aMean, aStd) {
  if (is.numeric(aMean) & is.numeric(aStd) &
      !is.na(aMean) & !is.na(aStd)) {
    result = rnorm(1, mean = aMean, sd = aStd)
    if (result < 0) {
      result = 0.0
    }
    result
  } else {
    aMean
  }
}

#' Create a new trophic model by randomly chosing parameters from a
#' corresponding normal trophic model
#' 
#' @param normalModel the model of means and standard deviations to be used to 
#'   randomly choose a specific model.
#' @return a specific model whose parameters have been randomly chosen from the 
#'   corresponding normal model
#' @export
varyModel <- function(normalModel) {
  if (!isNormalTrophicModel(normalModel)) {
    stop("the normal trophic model provided is not valid")
  }
  newModel <- newTrophicModel()
  
  speciesMeans  <- normalModel$species
  speciesStds   <- normalModel$speciesStd
  numSpecies    <- nrow(speciesMeans)
  speciesCols   <- names(speciesMeans)[-1]
  newSpecies    <- newModel$species
  for ( curRow in 1:numSpecies ) {
    newModel <- addSpecies(newModel, speciesMeans[curRow, "species"])
    newSpecies <- newModel$species
    for ( curCol in speciesCols ) {
      newSpecies[curRow, curCol] <- 
        .R_varyParameter(speciesMeans[curRow, curCol],
                         speciesStds[curRow, curCol])
    }
    newModel$species <- newSpecies
  }
  interactionMeans <- normalModel$interactions
  interactionStds  <- normalModel$interactionStds
  numInteractions  <- nrow(interactionMeans)
  interactionCols  <- names(interactionMeans)[-c(1:2)]
  for (curRow in 1:numInteractions) {
    newModel <- addInteraction(newModel, 
                               interactionMeans[curRow, "predator"],
                               interactionMeans[curRow, "prey"])
    newInteractions <- newModel$interactions
    for (curCol in interactionCols) {
      newInteractions[curRow, curCol] <- 
        .R_varyParameter(interactionMeans[curRow, curCol],
                         interactionStds[curRow, curCol])
    }
    newModel$interactions <- newInteractions
  }
  newModel
}