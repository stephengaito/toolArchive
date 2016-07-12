#' Create a model
#' 
#' A model is an RC object
#' 
#' @field species a list of species 
#' @field interactions a list of interactions between pairs of species
#' @export TrophicLogisticModel
#' @exportClass TrophicLogisticModel
TrophicLogisticModel <- setRefClass("TrophicLogisticModel",
  fields = list(
    species="list",
    interactions="list"
  ),
  methods = list(
    addSpecies = function(speciesName) {
      aSpecies <- Species$new(speciesName)
      .self$species[speciesName] <- aSpecies
      aSpecies
    }
  )
)