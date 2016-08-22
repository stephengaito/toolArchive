li// Integrate the Lorenz equations
//
// see: [Lorenz System](https://en.wikipedia.org/wiki/Lorenz_system)
//
// dx/dt = sigma * (y - x)
//
// dy/dt = x * (rho - z) - y,
//
// dz/dt = x * y - beta * z
//
// where interesting values for sigma, rho and beta are:
//
// sigma = 10
// rho   = 28
// beta  = 8/3
//

// To turn off debug output... comment out the next line
//#define DEBUG_OUTPUT
//
#include "dynSysToolKit.h"

#define ValuePtr( varNum, curStep) (workingResults + (varNum)*numWorkingResults + ((curStep) & workingResultsMask))
#define SamplePtr(varNum, curStep) (results + (varNum)*numSamples + (curStep)) 

#define X(curStep) *ValuePtr(0, curStep)
#define Y(curStep) *ValuePtr(1, curStep)
#define Z(curStep) *ValuePtr(2, curStep)

void L_lorenzDerivs(double  sigma, double  rho, double beta,
                    double* derivs,
                    size_t  curStep,
                    double* workingResults,
                    size_t  workingResultsMask,
                    size_t  numWorkingResults) {
  DEBUG("derivs %d %f %f %f %f %f %f \n", curStep, X(curStep), Y(curStep), Z(curStep), sigma, rho, beta);
  derivs[0] = sigma * ( Y(curStep) - X(curStep) );
  derivs[1] = ( X(curStep) * ( rho - Z(curStep) ) ) - Y(curStep);
  derivs[2] = ( X(curStep) * Y(curStep) ) - ( beta * Z(curStep) );
}

void L_lorenzJacob(double sigma, double rho, double beta,
                   double* jacob,
                   size_t curStep,
                   double* workingResults,
                   size_t  workingResultsMask,
                   size_t  numWorkingResults) {
  jacob[0] = -sigma;            # dx/dx
  jacob[1] =  sigma;            # dx/dy
  jacob[2] =  0.0;              # dx/dz
  jacob[3] =  rho - Z(curStep); # dy/dx
  jacob[4] = -1.0;              # dy/dy
  jacob[5] = -X(curStep);       # dy/dz
  jacob[6] =  Y(curStep);       # dz/dx
  jacob[7] =  X(curStep);       # dz/dy
  jacob[8] = -beta;             # dz/dz
}

SEXP C_integrateLorenzEuler(SEXP initialValuesSexp,
                            SEXP sigmaSexp,
                            SEXP rhoSexp,
                            SEXP betaSexp,
                            SEXP stepSizeSexp,
                            SEXP stepsPerSampleSexp,
                            SEXP numSamplesSexp,
                            SEXP numSamplesBetweenInteruptChecksSexp,
                            SEXP workingResultsMaskSexp,
                            SEXP workingResultsSexp,
                            SEXP resultsSexp) {
  DEBUG("\nC_integrateLorenzEuler\n");
  double sigma = L_assertADoubleInRange("sigma", sigmaSexp, -1000.0, 1000.0);
  double rho   = L_assertADoubleInRange("rho",   rhoSexp,   -1000.0, 1000.0);
  double beta  = L_assertADoubleInRange("beta",  betaSexp,  -1000.0, 1000.0);

  double* initialValues = L_assertDoubleVector("initialValues", initialValuesSexp, 3);

  double stepSize = 
    L_assertADoubleInRange("stepSize", stepSizeSexp, 1e-5, 1.0);
  int stepsPerSample = 
    L_assertAnIntegerInRange("stepsPerSample", stepsPerSampleSexp,
                             0, MAX_ITERATIONS);
  int numSamples = 
    L_assertAnIntegerInRange("numSamples", numSamplesSexp,
                             0, MAX_ITERATIONS);
  int numSamplesBetweenInteruptChecks = 
    L_assertAnIntegerInRange("numSamplesBetweenInteruptChecks", 
                             numSamplesBetweenInteruptChecksSexp,
                             0, MAX_ITERATIONS);
  
  size_t workingResultsMask =
    L_assertAnIntegerInRange("workingResultsMask", workingResultsMaskSexp,
                             0, MAX_ITERATIONS);
  size_t numWorkingResults = workingResultsMask + 1;
  double* workingResults = 
    L_assertDoubleVector("workingResults", workingResultsSexp,
                         (3 * numWorkingResults));
  double* results = 
    L_assertDoubleVector("results", resultsSexp,
                         (3 * numSamples));
  
  //
  // copy the initial values into the working results vector
  //
  for(size_t i = 0; i < 3; i++) {
    *ValuePtr(i, 0) = initialValues[i];
  }
  //
  // do the integration
  //
  double* derivs = initialValues; // initialValues are no longer needed so we can resuse them!
  size_t samplesFromLastInteruptCheck = 0;
  size_t curStep = 0;
  for (size_t curSample = 0; curSample < numSamples; curSample++) {
    for (size_t curStepPerSample = 0; curStepPerSample < stepsPerSample; curStepPerSample++) {
      curStep++;
      L_lorenzDerivs(sigma, rho, beta, derivs, curStep - 1,
                     workingResults, workingResultsMask, numWorkingResults);
      for (size_t i = 0; i < 3; i++) {
        *ValuePtr(i, curStep) = *ValuePtr(i, curStep - 1) +
          stepSize * derivs[i];
      }
      DEBUG("values %d %f %f %f \n", curStep, X(curStep), Y(curStep), Z(curStep));
    }
    //
    // collect a sample
    //
    for (size_t i = 0; i < 3; i++) {
      *SamplePtr(i, curSample) = *ValuePtr(i, curStep);
    }
    //    
    // check to see if the user wants to interupt this integration.
    //
    if (numSamplesBetweenInteruptChecks < samplesFromLastInteruptCheck) {
      R_CheckUserInterrupt();
      samplesFromLastInteruptCheck = 0;
    } else {
      samplesFromLastInteruptCheck++;
    }
  }
  return resultsSexp;
}

static R_CallMethodDef CIntegrateLorenz_callMethods[] = {
  { "C_integrateLorenzEuler", (DL_FUNC) & C_integrateLorenzEuler, 11},
  { NULL, NULL, 0}
};

void registerCIntegrateLorenz(DllInfo *info) {
  R_registerRoutines(info, NULL, CIntegrateLorenz_callMethods, NULL, NULL);
}