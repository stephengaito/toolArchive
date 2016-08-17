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

# Get the species and coefficients for the predators
#
# @param cSpeciesTable a cSpeciesTable
# @param speciesNum a 0 relative index denoting the required species
# @return a vector of vectors one being the (0 relative) species numbers and the
#  other begin the coefficients. NULL on any error or if there are no coefficients.
#' @export
#' @useDynLib ecoNetworksSimulator C_getPredatorCoefficients
.C_getPredatorCoefficients <- function(cSpeciesTable, speciesNum) {
  .Call("C_getPredatorCoefficients", cSpeciesTable, as.integer(speciesNum), PACKAGE = "ecoNetworksSimulator")
}

# Set the species and coefficients for the predators
#
# @param cSpeciesTable a cSpeciesTable
# @param speciesNum a 0 relative index denoting the required species
# @param speciesNumVec a vector of integer indexes into the collection of species (0 relative)
# @param speciesAttackVec a vector of real attack rates corresponding to each species in the species vector
# @param speciesConversionVec a vector of real conversion rates corresponding to each species in the species vector
# @return true if predator values have been set
#' @export
#' @useDynLib ecoNetworksSimulator C_setPredatorCoefficients
.C_setPredatorCoefficients <- function(cSpeciesTable, 
                                       speciesNum, 
                                       speciesNumVec, 
                                       speciesAttackVec,
                                       speciesConversionVec,
                                       speciesDeathRateVec,
                                       speciesTimeLagVec) {
  .Call("C_setPredatorCoefficients", 
        cSpeciesTable, 
        as.integer(speciesNum), 
        as.integer(speciesNumVec), 
        speciesAttackVec,
        speciesConversionVec,
        speciesDeathRateVec,
        as.integer(speciesTimeLagVec),
        PACKAGE = "ecoNetworksSimulator")
}

# Get the species and coefficients for the prey
#
# @param cSpeciesTable a cSpeciesTable
# @param speciesNum a 0 relative index denoting the required species
# @return a vector of vectors one being the (0 relative) species numbers and the
#  other begin the coefficients. NULL on any error or if there are no coeffiecients.
#' @export 
#' @useDynLib ecoNetworksSimulator C_getPreyCoefficients
.C_getPreyCoefficients <- function(cSpeciesTable, speciesNum) {
  .Call("C_getPreyCoefficients", cSpeciesTable, as.integer(speciesNum), PACKAGE = "ecoNetworksSimulator")
}

# Set the species and coefficients for the prey
#
# @param cSpeciesTable a cSpeciesTable
# @param speciesNum a 0 relative index denoting the required species
# @param speciesNumVec a vector of integer indexes into the collection of species (0 relative)
# @param speciesCoeffVec a vector of real coeffiecients corresponding to each species in the species vector
# @return true if prey values have been set
#' @export
#' @useDynLib ecoNetworksSimulator C_setPreyCoefficients
.C_setPreyCoefficients <- function(cSpeciesTable, 
                                   speciesNum, 
                                   speciesNumVec, 
                                   speciesAttackVec,
                                   speciesConversionVec,
                                   speciesDeathRateVec,
                                   speciesTimeLagVec) {
  .Call("C_setPreyCoefficients", 
        cSpeciesTable, 
        as.integer(speciesNum), 
        as.integer(speciesNumVec), 
        speciesAttackVec,
        speciesConversionVec,
        speciesDeathRateVec,
        as.integer(speciesTimeLagVec),
        PACKAGE = "ecoNetworksSimulator")
}

