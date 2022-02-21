# Initialize the CSpeciesTable random number generator state
# 
# @param cSpeciesTable the cSpeciesTable
# @param initType the type of initialization: initType == 0, initialize using
#   /dev/random (not yet implemented); initType == 1, initialize with ones;
#   initType == 2, initialize using time function.
# @param initialRun the number of initial random numbers to generate (and throw away)
# @return true
#' @export
.C_initRNGState <- function(cSpeciesTable, initType, initialRun) {
  .Call("C_initRNGState", cSpeciesTable, as.integer(initType), as.integer(initialRun), PACKAGE = "ecoNetworksSimulator")
}

# Set the CSpeciesTable random number generator state
# 
# @param cSpeciesTable the cSpeciesTable 
# @param rngState a vector of 33 integer values which forms the initial state
# @return true is successful
#' @export
.C_setRNGState <- function(cSpeciesTable, rngState) {
  .Call("C_setRNGState", cSpeciesTable, as.integer(rngState), PACKAGE = "ecoNetworksSimulator")
}

# Get the CSpeciesTable random number generator state
# 
# @param cSpeciesTable the cSpeciesTable
# @return an integer vector of lentgh 33
#' @export
.C_getRNGState <- function(cSpeciesTable) {
  .Call("C_getRNGState", cSpeciesTable, PACKAGE = "ecoNetworksSimulator")
}

# Get a uniformly distributed random number between 0 and 1
# 
# @param cSpeciesTable the cSpeciesTable
# @return a uniformly distributed random number between 0 and 1
#' @export
.C_getARandomNumber <- function(cSpeciesTable) {
  .Call("C_getARandomNumber", cSpeciesTable, PACKAGE = "ecoNetworksSimulator")
}

# Get a collection of random numbers between 0 and 1
# 
# @param cSpeciesTable the cSpeciesTable
# @param resultsVec the vector or matrix of numerics into which the random
#   numbers should be returned (one per element)
# @return the resultsVec containing random numbers
#' @export
.C_getRandomNumbers <- function(cSpeciesTable, resultsVec) {
  .Call("C_getRandomNumbers", cSpeciesTable, resultsVec, PACKAGE = "ecoNetworksSimulator")
}