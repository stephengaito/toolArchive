# Integrate the Lorenz equations (C-R interface)
#

# Use the simplest Euler method
#
#' @export
#' @useDynLib dynSysToolKit C_integrateLorenzEuler
.C_integrateLorenzEuler <- function(initialValues,
                                    sigma, rho, beta,
                                    stepSize, stepsPerSample, numSamples,
                                    numSamplesBetweenInteruptChecks,
                                    workingResultsMask,
                                    workingResults,
                                    results) {
  .Call("C_integrateLorenzEuler", 
        initialValues,
        sigma, rho, beta,
        stepSize, as.integer(stepsPerSample), as.integer(numSamples),
        as.integer(numSamplesBetweenInteruptChecks),
        as.integer(workingResultsMask), workingResults, results,
        PACKAGE = "dynSysToolKit")
}