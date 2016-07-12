#' Simulate ecological network models
#' 
#' Simulate relatively realistic spatially extended trophic networks.
#' 
#' The objective is not to explicitly model ecological systems in themselves,
#' but rather to provide a platform with which to synthetically produce large
#' reasonably realistic data sets from which to observe the influence of
#' different underlying assumptions of both network structure and data
#' collection.
#' 
"_PACKAGE"

#' Simulate the current trophic model
#' 
#' @param x anything
#' @return echos the value of x 
#' @export
#' @useDynLib ecoNetworksSimulator C_simulateModel
simulateModel <- function(x) .Call("C_simulateModel", 
                                   x, 
                                   PACKAGE = "ecoNetworksSimulator")
