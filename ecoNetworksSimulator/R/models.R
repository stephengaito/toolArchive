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
             stringsAsFactors=FALSE) 
}

#' Check that a table is a species table
#' 
#' @param speciesTable a potential speciesTable
#' @return true if the table provided is a speciesTable
#' @export
isSpeciesTable <- function(speciesTable){
  is.data.frame(speciesTable) && 
    colnames(speciesTable) == 
      c("species", "growthRate", "carryingCapacity", "mortality")
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
#' @export
addSpecies <- function(speciesTable, 
                       name, 
                       growthRate=0.0, 
                       carryingCapacity=NA_real_, 
                       mortality=NA_real_){
  rbind(speciesTable, 
        data.frame(species=name,
                   growthRate=growthRate,
                   carryingCapacity=carryingCapacity,
                   mortality=mortality),
        stringsAsFactors = FALSE)
}

#' Create table of interactions
#' 
#' A table of interactions is a data frame which has one row for each species 
#' interaction
#' 
#' The columns are preditor (name), prey (name), attackRate (real), and 
#' conversionRate (real)
#' 
#' @export
newInteractionsTable <- function() {
  data.frame(preditor=character(),
             prey=character(),
             attackRate=numeric(),
             conversionRate=numeric(),
             stringsAsFactors=FALSE)
}

#' Check that a table is a interactions table
#' 
#' @param interactionsTable a potential interactionsTable
#' @return true if the table provided is a interactionsTable
#' @export
isInteractionsTable <- function(interactionsTable){
  is.data.frame(interactionsTable) && 
    colnames(interactionsTable) == 
      c("preditor", "prey", "attackRate", "conversionRate")
}

#' Add a species interaction to an existing interactions table
#' 
#' Adds a new row to an existing interactions table
#' 
#' @param interactionsTable an existing interactions table which is to be
#'   extended
#' @param preditor the name of the preditor species
#' @param prey the name of the prey species
#' @param attackRate the rate at which the preditor attacks the prey
#' @param conversionRate the rate at which the preditor converts its prey into
#'   itself
#' @export
addInteraction <- function(interactionsTable, 
                       preditor, 
                       prey,
                       attackRate=0.0, 
                       conversionRate=0.0) {
  rbind(interactionsTable, 
        data.frame(preditor=preditor,
                   prey=prey,
                   attackRate=attackRate,
                   conversionRate=conversionRate),
        stringsAsFactors = FALSE)
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
    all(levels(interactionsTable$preditor) %in% levels(speciesTable$species)) &&
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
    stop("some of the preditor or prey species in the interations table are not defined in the species table")
  }
  list(species=speciesTable, interactions=interactionsTable)
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
    stop("some of the preditor or prey species in the interations table are not defined in the species table")
  }  
  list(species=speciesMeans, speciesStd=speciesStd,
       interactions=interactionMeans, interactionStds=interactionStds)
}
