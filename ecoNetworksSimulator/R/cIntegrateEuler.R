# Integrate a CModel using Euler integration
#
#' @export
#' @useDynLib ecoNetworksSimulator C_integrateEuler
.C_integrateEuler <- function(cModel,
                              stepSize,
                              stepsPerSample,
                              numSamples,
                              numSamplesBetweenInteruptChecks,
                              initialValues,
                              workingResults,
                              results) {
  .Call("C_integrateEuler",
        cModel,
        stepSize,
        as.integer(stepsPerSample),
        as.integer(numSamples),
        as.integer(numSamplesBetweenInteruptChecks),
        initialValues,
        workingResults,
        results,
        PACKAGE = "ecoNetworksSimulator")
}