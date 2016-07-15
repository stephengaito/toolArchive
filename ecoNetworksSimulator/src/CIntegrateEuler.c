// Build C structures used to simulate a model

#include "CModels.h"

#define SpeciesPtr(results, speciesNum, numSpecies, timeOffset) ((results) + (timeOffset)*(numSpecies) + (speciesNum))

void L_rateChange(CSpeciesTable *cSpecies, double* speciesRateChanges, double* results) {
  size_t speciesNum = 0;
  size_t numSpecies = cSpecies->numSpecies;
  for(speciesNum = 0; speciesNum < numSpecies; speciesNum++) {
    //
    // compute predation-factor
    //
    CSpecies *curSpecies = cSpecies->species + speciesNum;
    double predationFactor = curSpecies->halfSaturation;
    CInteraction *curPrey = curSpecies->prey;
    CInteraction *maxPrey = curPrey + curSpecies->numPrey;
    for(; curPrey < maxPrey; curPrey++) {
      predationFactor += 
        *SpeciesPtr(results, curPrey->speciesIndex, numSpecies, 0)
        * curPrey->attackRate;
    }
    if (SMALLEST_DOUBLE < predationFactor) {
      predationFactor = 
        *SpeciesPtr(results, speciesNum, numSpecies, 0)
        / predationFactor;
    } else {
      predationFactor = 0;
    }
    curSpecies->predationFactor = predationFactor;
  }
  for(size_t speciesNum = 0; speciesNum < cSpecies->numSpecies; speciesNum++) {
    CSpecies *curSpecies = cSpecies->species + speciesNum;
    double curSpeciesValue = *SpeciesPtr(results, speciesNum, numSpecies, 0);
    double rateChange = 0;
    if (SMALLEST_DOUBLE < curSpeciesValue) {
      //
      // compute growth-external-energy
      //
      double carryingCapacityFactor = 1.0;
      if (SMALLEST_DOUBLE < curSpecies->carryingCapacity) {
        carryingCapacityFactor = 1.0 - (curSpeciesValue / curSpecies->carryingCapacity);
      }
      rateChange += curSpecies->growthRate * curSpeciesValue * carryingCapacityFactor;
      //
      // compute growth-predation
      //
      if (SMALLEST_DOUBLE < curSpecies->predationFactor) {
        double conversionFactor = 0;
        CInteraction *curPrey = curSpecies->prey;
        CInteraction *maxPrey = curPrey + curSpecies->numPrey;
        for(; curPrey < maxPrey; curPrey++) {
          conversionFactor += 
            *SpeciesPtr(results, curPrey->speciesIndex, numSpecies, 0)
            * curPrey->attackRate * curPrey->conversionRate;
        }
        if (SMALLEST_DOUBLE < conversionFactor) {
          rateChange += conversionFactor * curSpecies->predationFactor;
        }
      }
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
      //
      // compute decay-natural-causes
      //
      if (SMALLEST_DOUBLE < curSpecies->mortality) {
        rateChange += - curSpecies->mortality * curSpeciesValue;
      }
      speciesRateChanges[speciesNum] = rateChange;
    }
  }
}

SEXP C_integrateEuler(SEXP cModelSexp,
                      SEXP stepSizeSexp,
                      SEXP maxIterationsSexp,
                      SEXP initialValuesSexp,
                      SEXP resultsSexp) {
  if (!L_isSpeciesTable(cModelSexp)) return R_NilValue;
  CSpeciesTable* cSpecies = (CSpeciesTable*)R_ExternalPtrAddr(cModelSexp);
  size_t numSpecies = cSpecies->numSpecies;
  if (!L_isADoubleInRange(stepSizeSexp, 1e-5, 1.0)) return R_NilValue;
  double stepSize = REAL(stepSizeSexp)[0];
  if (!L_isAnIntegerInRange(maxIterationsSexp, 0, MAX_ITERATIONS)) return R_NilValue;
  size_t maxIterations = INTEGER(maxIterationsSexp)[0];
  if (!L_isDoubleVector(initialValuesSexp, numSpecies)) return R_NilValue;
  double *initialValues = REAL(initialValuesSexp);
  if (!L_isDoubleVector(resultsSexp, (numSpecies * maxIterations))) return R_NilValue;
  double *results = REAL(resultsSexp);
  //
  // copy the initial values into the results vector
  //
  for (size_t i = 0; i < numSpecies; i++) results[i] = initialValues[i];
  //
  // do the integration
  //
  double *speciesRateChanges = (double*)Calloc(numSpecies, double);
  double *oldResults = results;
  for (size_t i = 0; i < maxIterations; i++) {
    L_rateChange(cSpecies, speciesRateChanges, oldResults);
    double *newResults = oldResults + numSpecies;
    for (size_t speciesNum = 0; speciesNum < numSpecies; speciesNum++) {
      newResults[speciesNum] = oldResults[speciesNum] + stepSize * speciesRateChanges[speciesNum];
      if (SMALLEST_DOUBLE < newResults[speciesNum]) newResults[speciesNum] = 0;
    }
    oldResults = newResults;
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
