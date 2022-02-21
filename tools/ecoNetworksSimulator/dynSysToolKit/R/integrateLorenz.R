#' Integrate the Lorenz Equations
#' 
#' Taken from wikipedia: [Lorenz
#' System](https://en.wikipedia.org/wiki/Lorenz_system)
#' 
#' @param x initial value of x
#' @param y initial value of y
#' @param z initial value of z
#' @param sigma value of sigma (10 default)
#' @param rho value of rho (28 default)
#' @param beta value of beta (8/3 default)
#' @param stepSize size of an individual intergration step (0.1 default)
#' @param stepsPerSample number of integration steps per sample (100 default)
#' @param numSamples number of samples to collect (1000 default)
#' @param numSamplesBetweenInteruptChecks the number of samples collected
#'   between each check for user interupts. A small number will slow down the 
#'   integration, while a large number will inhibit the user's ability to stop a
#'   long running integration. The default is 100 samples.
#' @return results of the integration as a matrix of numSamples rows and x, y, z
#'   columns
#' @export
integrateLorenz <- function(x=0.01, y=0.01, z=0.01, sigma=10, rho=28, beta=2.6666666666667,
                            stepSize=0.01, stepsPerSample=100, numSamples=1000,
                            numSamplesBetweenInteruptChecks = 100) {
  initialValues <- c(x,y,z)
  numWorkingResults <- 2*2
  workingResultsMask <- numWorkingResults - 1
  workingResults <- matrix(NA_real_, nrow = numWorkingResults, ncol = 3)
  results <- matrix(NA_real_, nrow = numSamples, ncol = 3)
  .C_integrateLorenzEuler(initialValues,
                          sigma, rho, beta,
                          stepSize, stepsPerSample, numSamples,
                          numSamplesBetweenInteruptChecks,
                          workingResultsMask,
                          workingResults,
                          results)
}