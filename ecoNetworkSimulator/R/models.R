#' Create a model
#' 
#' @param modelName model name
#' @return nothing
#' @export
#' @useDynLib ecoNetworksSimulator C_createModel
createModel <- function(modelName) {
  .Call("C_createModel", modelName, PACKAGE = "ecoNetworksSimulator" )
}

#' List all known models
#' 
#' @return returns an vector of the string names of each known model
#' @export
#' @useDynLib ecoNetworksSimulator C_listModels
listModels <- function() {
  .Call("C_listModels", PACKAGE = "ecoNetworksSimulator")
}