// Build C structures used to simulate a model

#include "CModels.h"

// To turn off debug output... comment out the next line
//#define DEBUG_OUTPUT
#ifdef  DEBUG_OUTPUT
#define DEBUG(...)  Rprintf(__VA_ARGS__)
#else
#define DEBUG(...)
#endif

#define TimeLag(timeLag) (((((int)curStep) - ((int)(timeLag))) < 1) ? 0 : (curStep - (timeLag)))
#define SpeciesPtr(speciesNum, i) (workingResults + (speciesNum)*numWorkingResults + ((i) & workingResultsMask))
#define SpeciesSamplesPtr(speciesNum, i) (results + (speciesNum)*numSamples + (i))

void L_rateChange(CSpeciesTable *cSpecies, 
                  double* speciesRateChanges, 
                  size_t curStep, 
                  double* workingResults, 
                  size_t workingResultsMask,
                  size_t numWorkingResults) {
  size_t speciesNum = 0;
  size_t numSpecies = cSpecies->numSpecies;
  DEBUG("\ncurStep = %d workingResults = %p numWorkingResults = %d workingResultsMask = 0x%X\n",
                curStep, workingResults, numWorkingResults, workingResultsMask);
  for(speciesNum = 0; speciesNum < numSpecies; speciesNum++) {
    DEBUG("species[%d, %d] = %e\n", speciesNum, curStep, *SpeciesPtr(speciesNum, curStep));
    //
    // compute predation-factor
    //
    CSpecies *curSpecies = cSpecies->species + speciesNum;
    double predationFactor = 0;
    if (!ISNAN(curSpecies->halfSaturation) && (0 < curSpecies->numPrey)) {
      predationFactor = curSpecies->halfSaturation;
      DEBUG("halfSaturation = %e\n", curSpecies->halfSaturation);
      CInteraction *curPrey = curSpecies->prey;
      CInteraction *maxPrey = curPrey + curSpecies->numPrey;
      for(; curPrey < maxPrey; curPrey++) {
        predationFactor += 
          *SpeciesPtr(curPrey->speciesIndex, TimeLag(curPrey->timeLag))
          * curPrey->attackRate;
      }
      DEBUG("predationSum = %e\n", predationFactor);
      if (predationFactor < SMALLEST_DOUBLE) predationFactor = SMALLEST_DOUBLE;
      predationFactor = 
        *SpeciesPtr(speciesNum, curStep)
        / predationFactor;
    }
    DEBUG("predationFactor[%d, %d] = %e (%e %d %d)\n", curStep, speciesNum, predationFactor,
        curSpecies->halfSaturation, curSpecies->numPrey, curSpecies->numPredators);
    curSpecies->predationFactor = predationFactor;
  }
  for(size_t speciesNum = 0; speciesNum < cSpecies->numSpecies; speciesNum++) {
    CSpecies *curSpecies = cSpecies->species + speciesNum;
    double curSpeciesValue = *SpeciesPtr(speciesNum, curStep);
    double rateChange = 0;
    if (SMALLEST_DOUBLE < curSpeciesValue) {
      //
      // compute growth-external-energy
      //
      if (SMALLEST_DOUBLE < curSpecies->growthRate) {
        double carryingCapacityFactor = 1.0;
        if (!ISNAN(curSpecies->carryingCapacity)) {
          if (curSpecies->carryingCapacity < SMALLEST_DOUBLE) {
            curSpecies->carryingCapacity = SMALLEST_DOUBLE;
          }
          carryingCapacityFactor = 1.0 -
            ((*SpeciesPtr(speciesNum, TimeLag(curSpecies->timeLag))) / curSpecies->carryingCapacity);
        }
        DEBUG("carryingCapacityFactor = %e (%e)\n", carryingCapacityFactor, curSpecies->carryingCapacity);
        rateChange += curSpecies->growthRate * curSpeciesValue * carryingCapacityFactor;
      }
      DEBUG("Ge rateChange[%d, %d] = %e\n", curStep, speciesNum, rateChange);
      //
      // compute growth-predation
      //
      if (SMALLEST_DOUBLE < curSpecies->predationFactor) {
        double conversionFactor = 0;
        CInteraction *curPrey = curSpecies->prey;
        CInteraction *maxPrey = curPrey + curSpecies->numPrey;
        for(; curPrey < maxPrey; curPrey++) {
          conversionFactor += 
            *SpeciesPtr(curPrey->speciesIndex, TimeLag(curPrey->timeLag))
            * curPrey->attackRate * curPrey->conversionRate;
        }
        if (SMALLEST_DOUBLE < conversionFactor) {
          rateChange += conversionFactor * curSpecies->predationFactor;
        }
      }
      DEBUG("Gp rateChange[%d, %d] = %e\n", curStep, speciesNum, rateChange);
      //
      // compute decay-predation
      //
      double decayFactor = 0;
      CInteraction *curPredator = curSpecies->predators;
      CInteraction *maxPredator = curPredator + curSpecies->numPredators;
      for(; curPredator < maxPredator; curPredator++) {
        decayFactor += *SpeciesPtr(speciesNum, TimeLag(curPredator->timeLag))
          * curPredator->attackRate * cSpecies->species[curPredator->speciesIndex].predationFactor;
      }
      if (SMALLEST_DOUBLE < decayFactor) {
        rateChange += - decayFactor;
      }
      DEBUG("Dp rateChange[%d, %d] = %e\n", curStep, speciesNum, rateChange);
      //
      // compute decay-natural-causes
      //
      if (SMALLEST_DOUBLE < curSpecies->mortality) {
        rateChange += - curSpecies->mortality * curSpeciesValue;
      }
      DEBUG("total rateChange[%d, %d] = %e\n", curStep, speciesNum, rateChange);
      speciesRateChanges[speciesNum] = rateChange;
    }
  }
}

SEXP C_integrateEuler(SEXP cModelSexp,
                      SEXP stepSizeSexp,
                      SEXP stepsPerSampleSexp,
                      SEXP numSamplesSexp,
                      SEXP numSamplesBetweenInteruptChecksSexp,
                      SEXP initialValuesSexp,
                      SEXP workingResultsMaskSexp,
                      SEXP workingResultsSexp,
                      SEXP resultsSexp) {
  DEBUG("\nC_intgrateEuler\n");
  L_assertSpeciesTable("model", cModelSexp);
  CSpeciesTable* cSpecies = (CSpeciesTable*)R_ExternalPtrAddr(cModelSexp);
  L_assertNotNull("cSpecies", cSpecies);
  size_t numSpecies = cSpecies->numSpecies;
  L_assertADoubleInRange("stepSize", stepSizeSexp, 1e-5, 1.0);
  double stepSize = REAL(stepSizeSexp)[0];
  L_assertAnIntegerInRange("stepsPerSample", stepsPerSampleSexp, 0, MAX_ITERATIONS);
  size_t stepsPerSample = INTEGER(stepsPerSampleSexp)[0];
  L_assertAnIntegerInRange("numSamples", numSamplesSexp, 0, MAX_ITERATIONS);
  size_t numSamples = INTEGER(numSamplesSexp)[0];
  L_assertAnIntegerInRange("numSamplesBetweenInteruptChecks", numSamplesBetweenInteruptChecksSexp, 0, numSamples+1);
  size_t numSamplesBetweenInteruptChecks = INTEGER(numSamplesBetweenInteruptChecksSexp)[0];
  L_assertDoubleVector("initialValues", initialValuesSexp, numSpecies);
  double *initialValues = REAL(initialValuesSexp);
  L_assertAnIntegerInRange("workingResultsMask", workingResultsMaskSexp, 0, MAX_ITERATIONS);
  size_t workingResultsMask = INTEGER(workingResultsMaskSexp)[0];
  DEBUG("  workingResultsMask = 0x%X\n", workingResultsMask);
  size_t numWorkingResults = workingResultsMask + 1;
  DEBUG("  numWorkingResults = %d\n", numWorkingResults);
  L_assertDoubleVector("workingResults", workingResultsSexp, (numSpecies * numWorkingResults));
  double *workingResults = REAL(workingResultsSexp);
  DEBUG("  workingResults = %p length = %d\n", workingResults, GET_LENGTH(workingResultsSexp));
  L_assertDoubleVector("results", resultsSexp, (numSpecies * numSamples));
  double *results = REAL(resultsSexp);
  //
  // copy the initial values into the results vector
  //
  for (size_t i = 0; i < numSpecies; i++) {
    *SpeciesPtr(i, 0) = *(initialValues + i);
  }
  //
  // do the integration
  //
  double *speciesRateChanges = initialValues; // initialValues are no longer needed so we can reuse them!
  size_t samplesFromLastInteruptCheck = 0;
  size_t curStep = 0;
  for (size_t curSample = 0; curSample < numSamples; curSample++) {
    for (size_t curStepPerSample = 0; curStepPerSample < stepsPerSample; curStepPerSample++ ) {
      curStep++;
      L_rateChange(cSpecies, speciesRateChanges, curStep - 1, workingResults, workingResultsMask, numWorkingResults);
      for (size_t speciesNum = 0; speciesNum < numSpecies; speciesNum++) {
        *SpeciesPtr(speciesNum, curStep) = *SpeciesPtr(speciesNum, curStep - 1) +
          stepSize * speciesRateChanges[speciesNum];
        if (*SpeciesPtr(speciesNum, curStep) < SMALLEST_DOUBLE) *SpeciesPtr(speciesNum, curStep) = 0;
      }
    }
    //
    // collect a sample
    //
    for (size_t i = 0; i < numSpecies; i++) {
      *SpeciesSamplesPtr(i, curSample) = *SpeciesPtr(i, curStep);
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

static R_CallMethodDef CIntegrateEuler_callMethods[] = {
  { "C_integrateEuler", (DL_FUNC) &C_integrateEuler, 9},
  { NULL, NULL, 0}
};

void registerCIntegrateEuler(DllInfo *info) {
  R_registerRoutines(info, NULL, CIntegrateEuler_callMethods, NULL, NULL);
}
