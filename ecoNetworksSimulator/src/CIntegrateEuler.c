// Build C structures used to simulate a model

#include "CModels.h"

#define SpeciesPtr(speciesNum, i) (results + (speciesNum)*maxIterations + (i))

void L_rateChange(CSpeciesTable *cSpecies, 
                  double* speciesRateChanges, 
                  size_t curStep, 
                  double* results, 
                  size_t maxIterations) {
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
                      SEXP maxIterationsSexp,
                      SEXP numStepsBetweenInteruptChecksSexp,
                      SEXP initialValuesSexp,
                      SEXP resultsSexp) {
  if (!L_isSpeciesTable(cModelSexp)) return R_NilValue;
  CSpeciesTable* cSpecies = (CSpeciesTable*)R_ExternalPtrAddr(cModelSexp);
  size_t numSpecies = cSpecies->numSpecies;
  if (!L_isADoubleInRange(stepSizeSexp, 1e-5, 1.0)) return R_NilValue;
  double stepSize = REAL(stepSizeSexp)[0];
  if (!L_isAnIntegerInRange(maxIterationsSexp, 0, MAX_ITERATIONS)) return R_NilValue;
  size_t maxIterations = INTEGER(maxIterationsSexp)[0];
  if (!L_isAnIntegerInRange(numStepsBetweenInteruptChecksSexp, 0, maxIterations+1)) return R_NilValue;
  size_t numStepsBetweenInteruptChecks = INTEGER(numStepsBetweenInteruptChecksSexp)[0];
  if (!L_isDoubleVector(initialValuesSexp, numSpecies)) return R_NilValue;
  double *initialValues = REAL(initialValuesSexp);
  if (!L_isDoubleVector(resultsSexp, (numSpecies * maxIterations))) return R_NilValue;
  double *results = REAL(resultsSexp);
  //
  // copy the initial values into the results vector
  //
  for (size_t i = 0; i < numSpecies; i++) {
    *(results + i*maxIterations) = *(initialValues + i);
  }
  //
  // do the integration
  //
  double *speciesRateChanges = initialValues; // initialValues are no longer needed so we can reuse them!
  size_t stepsFromLastInteruptCheck = 0;
  for (size_t i = 1; i < maxIterations; i++) {
    L_rateChange(cSpecies, speciesRateChanges, i - 1, results, maxIterations);
    for (size_t speciesNum = 0; speciesNum < numSpecies; speciesNum++) {
      *SpeciesPtr(speciesNum, i) = *SpeciesPtr(speciesNum, i - 1) +
        stepSize * speciesRateChanges[speciesNum];
      if (*SpeciesPtr(speciesNum, i) < SMALLEST_DOUBLE) *SpeciesPtr(speciesNum, i) = 0;
    }
    //    
    // check to see if the user wants to interupt this integration.
    //
    if (numStepsBetweenInteruptChecks < stepsFromLastInteruptCheck) {
      R_CheckUserInterrupt();
      stepsFromLastInteruptCheck = 0;
    } else {
      stepsFromLastInteruptCheck++;
    }
  }
  return resultsSexp;
}

static R_CallMethodDef CIntegrateEuler_callMethods[] = {
  { "C_integrateEuler", (DL_FUNC) &C_integrateEuler, 5},
  { NULL, NULL, 0}
};

void registerCIntegrateEuler(DllInfo *info) {
  R_registerRoutines(info, NULL, CIntegrateEuler_callMethods, NULL, NULL);
}
