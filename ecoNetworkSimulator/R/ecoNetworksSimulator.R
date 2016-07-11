
#' Simulate the current trophic model
#' 
#' @param x anything
#' @return echos the value of x 
#' @export
#' @useDynLib ecoNetworksSimulator C_simulateModel
simulateModel <- function(x) .Call("C_simulateModel", 
                                   x, 
                                   PACKAGE = "ecoNetworksSimulator")
