# Create a new species table in C
# 
# @return an opaque reference to a C structure which holds a species table.
#' @export
#' @useDynLib ecoNetworksSimulator C_newSpeciesTable
.C_newSpeciesTable <-function(numSpecies) {
  .Call("C_newSpeciesTable", as.integer(numSpecies), PACKAGE = "ecoNetworksSimulator")
}

# Check that a table is a species table
# 
# @param speciesTable a potential speciesTable
# @return true if the table provided is a speciesTable
#' @export
#' @useDynLib ecoNetworksSimulator C_isSpeciesTable
.C_isSpeciesTable <-function(cSpeciesTable) {
  .Call("C_isSpeciesTable", cSpeciesTable, PACKAGE = "ecoNetworksSimulator")
}

# Return the number of species in either a species or interactions table
#
# @return the number of species
#' @export
#' @useDynLib ecoNetworksSimulator C_numSpecies
.C_numSpecies <- function(cTable) {
  .Call("C_numSpecies", cTable, PACKAGE = "ecoNetworksSimulator")
}

# Get the values for a given species
#
# @param cSpeciesTable a cSpeciesTable
# @param speciesNum a 0 relative index of species
# @return a vector of values for the given species
#' @export
#' @useDynLib ecoNetworksSimulator C_getSpeciesValues
.C_getSpeciesValues <- function(cSpeciesTable, speciesNum) {
  .Call("C_getSpeciesValues", cSpeciesTable, as.integer(speciesNum), PACKAGE = "ecoNetworksSimulator")
}

# Set the values for a given species
#
# @param cSpeciesTable a cSpeciesTable
# @param speciesNum a 0 relative index of species
# @param speciesValues a vector of species values (as doubles)
# @return true if values have been set
#' @export
#' @useDynLib ecoNetworksSimulator C_setSpeciesValues
.C_setSpeciesValues <- function(cSpeciesTable, speciesNum, speciesValues) {
  .Call("C_setSpeciesValues", cSpeciesTable, as.integer(speciesNum), speciesValues, PACKAGE = "ecoNetworksSimulator")
}

# Create a new interactions table in C
# 
# @return an opaque reference to a C structure which holds an interactions table.
#' @export
#' @useDynLib ecoNetworksSimulator C_newInteractionsTable
.C_newInteractionsTable <-function(numSpecies) {
  .Call("C_newInteractionsTable", as.integer(numSpecies), PACKAGE = "ecoNetworksSimulator")
}

# Check that a table is an interactions table
# 
# @param interactionsTable a potential interactionsTable
# @return true if the table provided is a interactionsTable
#' @export
#' @useDynLib ecoNetworksSimulator C_isInteractionsTable
.C_isInteractionsTable <-function(cInteractionsTable) {
  .Call("C_isInteractionsTable", cInteractionsTable, PACKAGE = "ecoNetworksSimulator")
}
