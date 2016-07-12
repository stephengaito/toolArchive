#' Create a Species
#' 
#' @export Species
#' @exportClass Species
Species <- setRefClass("Species",
  fields = list(
    speciesName="character",
    growthRate="numeric",
    carryingCapacity="numeric",
    mortality="numeric"
  ),
  methods = list(
    initialize = function(aSpeciesName) {
      speciesName <<- aSpeciesName
      growthRate <<- 0.0
      carryingCapacity <<- NA_real_
      mortality <<- 0.0
      .self
    },
    setGrowthRate = function(aValue) {
      growthRate <<- aValue
      .self
    }
  )
)