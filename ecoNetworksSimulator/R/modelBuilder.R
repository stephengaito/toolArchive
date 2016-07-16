#' Create a species table
#' 
#' A species table is a data frame which has one row for each species.
#' 
#' The columns consist of species (name), growthRate (real), carryingCapacity
#' (real), and mortality (real).
#' 
#' @export
newSpeciesTable <- function() {
  data.frame(species=character(),
             growthRate=numeric(),
             carryingCapacity=numeric(),
             mortality=numeric(),
             halfSaturation=numeric())
}

#' Check that a table is a species table
#' 
#' @param speciesTable a potential speciesTable
#' @return true if the table provided is a speciesTable
#' @export
isSpeciesTable <- function(speciesTable){
  is.data.frame(speciesTable) && 
    colnames(speciesTable) == 
      c("species", "growthRate", "carryingCapacity", "mortality", "halfSaturation")
}

#' Returns the number of species in a species table
#' 
#' @param speciesTable the species table
#' @return the number of species in the species table
#' @export
numSpeciesInTable <- function(speciesTable) {
  nrow(speciesTable)  
}

#' Add a species to an existing species table
#' 
#' Adds a new row to an existing species table
#' 
#' @param speciesTable an existing species table which is to be extended
#' @param name the name of the species
#' @param growthRate the growth rate of the species (could be 0.0)
#' @param carryingCapacity the carrying capacity of the species (could be NA)
#' @param mortality the mortality of the species (could be NA)
#' @param halfSaturation the half saturation point for the predator's predation
#' @export
addSpecies <- function(speciesTable, 
                       name, 
                       growthRate=0.0, 
                       carryingCapacity=NA_real_, 
                       mortality=0.0,
                       halfSaturation=NA_real_){
  rbind(speciesTable, 
        data.frame(species=name,
                   growthRate=growthRate,
                   carryingCapacity=carryingCapacity,
                   mortality=mortality,
                   halfSaturation=halfSaturation))
}

#' Create table of interactions
#' 
#' A table of interactions is a data frame which has one row for each species 
#' interaction
#' 
#' The columns are predator (name), prey (name), attackRate (real), and 
#' conversionRate (real)
#' 
#' @export
newInteractionsTable <- function() {
  data.frame(predator=character(),
             prey=character(),
             attackRate=numeric(),
             conversionRate=numeric())
}

#' Check that a table is a interactions table
#' 
#' @param interactionsTable a potential interactionsTable
#' @return true if the table provided is a interactionsTable
#' @export
isInteractionsTable <- function(interactionsTable){
  is.data.frame(interactionsTable) && 
    colnames(interactionsTable) == 
      c("predator", "prey", "attackRate", "conversionRate")
}

#' Add a species interaction to an existing interactions table
#' 
#' Adds a new row to an existing interactions table
#' 
#' @param interactionsTable an existing interactions table which is to be
#'   extended
#' @param predator the name of the predator species
#' @param prey the name of the prey species
#' @param attackRate the rate at which the predator attacks the prey
#' @param conversionRate the rate at which the predator converts its prey into
#'   itself
#' @export
addInteraction <- function(interactionsTable, 
                       predator, 
                       prey,
                       attackRate=0.0, 
                       conversionRate=0.0) {
  rbind(interactionsTable, 
        data.frame(predator=predator,
                   prey=prey,
                   attackRate=attackRate,
                   conversionRate=conversionRate))
}

#' Check that a speciesTable and a given interactionsTable are related.
#' 
#' @param speciesTable a species table
#' @param interactionsTable a table of species interactions
#' @return true if all of the species in the interaction table are defined in
#'   the species table.
#' @export
areRelated <- function(speciesTable, interactionsTable) {
  isSpeciesTable(speciesTable) &&
    isInteractionsTable(interactionsTable) &&
    all(levels(interactionsTable$predator) %in% levels(speciesTable$species)) &&
    all(levels(interactionsTable$prey) %in% levels(speciesTable$species))
}

#' Create a model
#' 
#' A model is an list of tables
#' 
#' @param speciesTable a table of species
#' @param interactionsTable a table of species interactions
#' @export
newModel <- function(speciesTable, interactionsTable) {
  if (!isSpeciesTable(speciesTable)) {
    stop("the species table provided is incorrectly formated")
  }
  if (!isInteractionsTable(interactionsTable)) {
    stop("the interactions table provided is incorrectly formated")
  }
  if (!areRelated(speciesTable, interactionsTable)) {
    stop("some of the predator or prey species in the interations table are not defined in the species table")
  }
  list(species=speciesTable, interactions=interactionsTable)
}

#' Check that a model is valid
#' 
#' @param model the model to verify
#' @export
isModel <- function(model) {
  if (!isSpeciesTable(model$species)) {
    stop("the model does not have a valid species table")
  }
  if (!isInteractionsTable(model$interactions)) {
    stop("the model does not have a valid interactions table")
  }
  if (!areRelated(model$species, model$interactions)) {
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

#' Create a normal model
#' 
#' A normal model is a list of tables which includes both means and standard
#' deviations.
#' 
#' @param speciesMeans a table of species means
#' @param speciesStd a table of species standard deviations
#' @param interactionMeans a table of interaction means
#' @param interactionStds a table of interaction standard deviations
#' @export
newNormalModel <- function(speciesMeans, speciesStd, 
                           interactionMeans, interactionStds) {
  if (!isSpeciesTable(speciesMeans)) {
    stop("the table of species means provided is incorrectly formated")
  }
  if (!isSpeciesTable(speciesStd)) {
    stop("the table of species standard deviations provided is incorrectly formated")
  }
  if (!isInteractionsTable(interactionMeans)) {
    stop("the table of interaction means provided is incorrectly formated")
  }
  if (!isInteractionsTable(interactionStds)) {
    stop("the table of interaction standard deviations provided is incorrectly formated")
  }
  if (!areRelated(speciesMeans, interactionMeans)) {
    stop("some of the predator or prey species in the interations table are not defined in the species table")
  }  
  list(species=speciesMeans, speciesStd=speciesStd,
       interactions=interactionMeans, interactionStds=interactionStds)
}
