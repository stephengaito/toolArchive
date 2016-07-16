# Integrate a CModel using Euler integration
#
#' @export
#' @useDynLib ecoNetworksSimulator C_integrateEuler
.C_integrateEuler <- function(cModel,
                              stepSize,
                              maxIterations,
                              numStepsBetweenInteruptChecks,
                              initialValues,
                              results) {
  .Call("C_integrateEuler",
        cModel,
        stepSize,
        as.integer(maxIterations),
        as.integer(numStepsBetweenInteruptChecks),
        initialValues,
        results,
        PACKAGE = "ecoNetworksSimulator")
}