# Integrate a CModel using Euler integration
#
#' @export
#' @useDynLib ecoNetworksSimulator C_integrateEuler
.C_integrateEuler <- function(cModel, stepSize, maxIterations, initialValues, results) {
  .Call("C_integrateEuler",
        cModel,
        stepSize,
        as.integer(maxIterations),
        initialValues,
        results,
        PACKAGE = "ecoNetworksSimulator")
}