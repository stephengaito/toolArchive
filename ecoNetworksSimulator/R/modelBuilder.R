# Create a new species table in C
# 
# @return an opaque reference to a C structure which holds a species table.
#' @export
#' @useDynLib ecoNetworksSimulator C_newSpeciesTable
.C_newSpeciesTable <-function() {
  .Call("C_newSpeciesTable", PACKAGE = "ecoNetworksSimulator")
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