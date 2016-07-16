// Build C structures used to simulate a model

#include "CModels.h"

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
  for(speciesNum = 0; speciesNum < numSpecies; speciesNum++) {
    //
    // compute predation-factor
    //
    CSpecies *curSpecies = cSpecies->species + speciesNum;
    double predationFactor = 0;
    if (!ISNAN(curSpecies->halfSaturation) && (0 < curSpecies->numPrey)) {
      predationFactor = curSpecies->halfSaturation;
      CInteraction *curPrey = curSpecies->prey;
      CInteraction *maxPrey = curPrey + curSpecies->numPrey;
      for(; curPrey < maxPrey; curPrey++) {
        predationFactor += 
          *SpeciesPtr(curPrey->speciesIndex, curStep)
          * curPrey->attackRate;
      }
      if (predationFactor < SMALLEST_DOUBLE) predationFactor = SMALLEST_DOUBLE;
      predationFactor = 
        *SpeciesPtr(speciesNum, curStep)
        / predationFactor;
    }
    //Rprintf("predationFactor[%d, %d] = %e (%e %d %d)\n", curStep, speciesNum, predationFactor,
    //        curSpecies->halfSaturation, curSpecies->numPrey, curSpecies->numPredators);
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
          carryingCapacityFactor = 1.0 - (curSpeciesValue / curSpecies->carryingCapacity);
        }
        //Rprintf("carryingCapacityFactor = %e (%e)\n", carryingCapacityFactor, curSpecies->carryingCapacity);
        rateChange += curSpecies->growthRate * curSpeciesValue * carryingCapacityFactor;
      }
      //Rprintf("Ge rateChange[%d, %d] = %e\n", curStep, speciesNum, rateChange);
      //
      // compute growth-predation
      //
      if (SMALLEST_DOUBLE < curSpecies->predationFactor) {
        double conversionFactor = 0;
        CInteraction *curPrey = curSpecies->prey;
        CInteraction *maxPrey = curPrey + curSpecies->numPrey;
        for(; curPrey < maxPrey; curPrey++) {
          conversionFactor += 
            *SpeciesPtr(curPrey->speciesIndex, curStep)
            * curPrey->attackRate * curPrey->conversionRate;
        }
        if (SMALLEST_DOUBLE < conversionFactor) {
          rateChange += conversionFactor * curSpecies->predationFactor;
        }
      }
      //Rprintf("Gp rateChange[%d, %d] = %e\n", curStep, speciesNum, rateChange);
      //
      // compute decay-predation
      //
      double decayFactor = 0;
      CInteraction *curPredator = curSpecies->predators;
      CInteraction *maxPredator = curPredator + curSpecies->numPredators;
      for(; curPredator < maxPredator; curPredator++) {
        decayFactor += curPredator->attackRate * cSpecies->species[speciesNum].predationFactor;
      }
      if (SMALLEST_DOUBLE < decayFactor) {
        rateChange += - curSpeciesValue * decayFactor;
      }
      //Rprintf("Dp rateChange[%d, %d] = %e\n", curStep, speciesNum, rateChange);
      //
      // compute decay-natural-causes
      //
      if (SMALLEST_DOUBLE < curSpecies->mortality) {
        rateChange += - curSpecies->mortality * curSpeciesValue;
      }
      //Rprintf("total rateChange[%d, %d] = %e\n", curStep, speciesNum, rateChange);
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
  if (!L_isSpeciesTable(cModelSexp)) return R_NilValue;
  CSpeciesTable* cSpecies = (CSpeciesTable*)R_ExternalPtrAddr(cModelSexp);
  size_t numSpecies = cSpecies->numSpecies;
  if (!L_isADoubleInRange(stepSizeSexp, 1e-5, 1.0)) return R_NilValue;
  double stepSize = REAL(stepSizeSexp)[0];
  if (!L_isAnIntegerInRange(stepsPerSampleSexp, 0, MAX_ITERATIONS)) return R_NilValue;
  size_t stepsPerSample = INTEGER(stepsPerSampleSexp)[0];
  if (!L_isAnIntegerInRange(numSamplesSexp, 0, MAX_ITERATIONS)) return R_NilValue;
  size_t numSamples = INTEGER(numSamplesSexp)[0];
  if (!L_isAnIntegerInRange(numSamplesBetweenInteruptChecksSexp, 0, numSamples+1)) return R_NilValue;
  size_t numSamplesBetweenInteruptChecks = INTEGER(numSamplesBetweenInteruptChecksSexp)[0];
  if (!L_isDoubleVector(initialValuesSexp, numSpecies)) return R_NilValue;
  double *initialValues = REAL(initialValuesSexp);
  if (!L_isAnIntegerInRange(workingResultsMaskSexp, 0, MAX_ITERATIONS)) return R_NilValue;
  size_t workingResultsMask = INTEGER(workingResultsMaskSexp)[0];
  size_t numWorkingResults = workingResultsMask + 1;
  if (!L_isDoubleVector(workingResultsSexp, (numSpecies * numWorkingResults))) return R_NilValue;
  double *workingResults = REAL(workingResultsSexp);
  if (!L_isDoubleVector(resultsSexp, (numSpecies * numSamples))) return R_NilValue;
  double *results = REAL(resultsSexp);
  //
  // copy the initial values into the results vector
  //
  for (size_t i = 0; i < numSpecies; i++) {
    *(workingResults + i*numSamples) = *(initialValues + i);
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
