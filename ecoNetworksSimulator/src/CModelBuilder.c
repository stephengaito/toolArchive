// Build C structures used to simulate a model

#include "CModels.h"

void C_newSpeciesTable_Finalizer(SEXP cSpeciesTable) {
  if (L_isSpeciesTable(cSpeciesTable)) {
    CSpeciesTable *cSpeciesTablePtr =
      (CSpeciesTable*)R_ExternalPtrAddr(cSpeciesTable);
    CSpecies *species = cSpeciesTablePtr->species;
    CSpecies *maxSpecies = species + cSpeciesTablePtr->numSpecies;
    for( ; species < maxSpecies; species++) {
      Free(species->predators);
      Free(species->prey);
    }
    Free(cSpeciesTablePtr->species);
    Free(cSpeciesTablePtr);
  }
}

SEXP C_newSpeciesTable(SEXP numSpecies) {
  L_assertAnIntegerInRange("numSpecies", numSpecies, 0, MAX_NUM_SPECIES);
  CSpeciesTable *cSpeciesTablePtr = 
    (CSpeciesTable*)Calloc(1, CSpeciesTable);
  cSpeciesTablePtr->tag        = CSpeciesTable_TAG;
  cSpeciesTablePtr->rngP       = 0;
  for (size_t i = 0; i < 16; i++) cSpeciesTablePtr->rngState[i] = 0;
  cSpeciesTablePtr->numSpecies = INTEGER(numSpecies)[0];
  cSpeciesTablePtr->species       = 
    (CSpecies*)Calloc(cSpeciesTablePtr->numSpecies, CSpecies);
  CSpecies* species = cSpeciesTablePtr->species;
  CSpecies* maxSpecies = species + cSpeciesTablePtr->numSpecies;
  for(; species < maxSpecies; species++) {
    species->growthRate         = NA_REAL;
    species->carryingCapacity   = NA_REAL;
    species->timeLag            = 0;
    species->mortality          = NA_REAL;
    species->halfSaturation     = NA_REAL;
    species->reintroductionProb = 0.0;
    species->reintroductionSize = 0.0;
    species->predationFactor    = 0.0;
    species->numPredators       = 0;
    species->predators          = NULL;
    species->numPrey            = 0;
    species->prey               = NULL;
  }
  SEXP cSpeciesTable = R_MakeExternalPtr(cSpeciesTablePtr, R_NilValue, R_NilValue);
  R_RegisterCFinalizerEx(cSpeciesTable, (R_CFinalizer_t) &C_newSpeciesTable_Finalizer, TRUE);
  return cSpeciesTable;
}

SEXP C_isSpeciesTable(SEXP cSpeciesTable) {
  SEXP result = NEW_LOGICAL(1);
  LOGICAL(result)[0] = L_isSpeciesTable(cSpeciesTable);
  return result;
}

SEXP C_numSpecies(SEXP cTable) {
  SEXP result = NEW_INTEGER(1);
  INTEGER(result)[0] = 0;
  if (!L_isSpeciesTable(cTable)) return result;
  CSpeciesTable *cSpeciesTablePtr = (CSpeciesTable*)R_ExternalPtrAddr(cTable);
  INTEGER(result)[0] = cSpeciesTablePtr->numSpecies;
  return result;
}

SEXP C_getSpeciesValues(SEXP cSpeciesTable, SEXP speciesNum) {
  L_assertSpeciesTable("cSpeciesTable", cSpeciesTable);
  CSpeciesTable *cSpeciesTablePtr = (CSpeciesTable*)R_ExternalPtrAddr(cSpeciesTable); 
  L_assertNotNull("cSpeciesTablePtr", cSpeciesTablePtr);
  L_assertAnIntegerInRange("speciesNum", speciesNum, 0, cSpeciesTablePtr->numSpecies);
  size_t speciesIndex = INTEGER(speciesNum)[0];
  CSpecies* speciesPtr = cSpeciesTablePtr->species + speciesIndex;
  SEXP result = NEW_NUMERIC(SPECIES_NUM_VALUES);
  REAL(result)[SPECIES_GROWTH_RATE]         = speciesPtr->growthRate;
  REAL(result)[SPECIES_CARRYING_CAPACITY]   = speciesPtr->carryingCapacity;
  REAL(result)[SPECIES_TIME_LAG]            = (double)speciesPtr->timeLag;
  REAL(result)[SPECIES_MORTALITY]           = speciesPtr->mortality;
  REAL(result)[SPECIES_HALF_SATURATION]     = speciesPtr->halfSaturation;
  REAL(result)[SPECIES_REINTRODUCTION_PROB] = speciesPtr->reintroductionProb;
  REAL(result)[SPECIES_REINTRODUCTION_SIZE] = speciesPtr->reintroductionSize;
  return result;
}

SEXP C_setSpeciesValues(SEXP cSpeciesTable, SEXP speciesNum, SEXP speciesValues) {
  SEXP result = NEW_LOGICAL(1);
  LOGICAL(result)[0] = FALSE;
  L_assertSpeciesTable("cSpeciedTable", cSpeciesTable);
  CSpeciesTable *cSpeciesTablePtr = (CSpeciesTable*)R_ExternalPtrAddr(cSpeciesTable); 
  L_assertNotNull("cSpeciesTablePtr", cSpeciesTablePtr);
  L_assertAnIntegerInRange("speciesNum", speciesNum, 0, cSpeciesTablePtr->numSpecies);
  size_t speciesIndex = INTEGER(speciesNum)[0];
  CSpecies* speciesPtr = cSpeciesTablePtr->species + speciesIndex;
  L_assertDoubleVector("speciesValues", speciesValues, SPECIES_NUM_VALUES);
  speciesPtr->growthRate         = REAL(speciesValues)[SPECIES_GROWTH_RATE];
  speciesPtr->carryingCapacity   = REAL(speciesValues)[SPECIES_CARRYING_CAPACITY];
  speciesPtr->timeLag            = (int) (REAL(speciesValues)[SPECIES_TIME_LAG] + SMALLEST_DOUBLE);
  speciesPtr->mortality          = REAL(speciesValues)[SPECIES_MORTALITY];
  speciesPtr->halfSaturation     = REAL(speciesValues)[SPECIES_HALF_SATURATION];
  speciesPtr->reintroductionProb = REAL(speciesValues)[SPECIES_REINTRODUCTION_PROB];
  speciesPtr->reintroductionSize = REAL(speciesValues)[SPECIES_REINTRODUCTION_SIZE];
  LOGICAL(result)[0] = TRUE;
  return result;
}

SEXP L_getPredatorPreyCoefficients(int predatorPreyType, SEXP cSpeciesTable, SEXP speciesNum) {
  L_assertSpeciesTable("cSpeciesTable", cSpeciesTable);
  CSpeciesTable *cSpeciesTablePtr =
    (CSpeciesTable*)R_ExternalPtrAddr(cSpeciesTable);
  L_assertNotNull("cSpeciesTablePtr", cSpeciesTablePtr);
  L_assertAnIntegerInRange("speciesNum", speciesNum, 0, cSpeciesTablePtr->numSpecies);
  size_t speciesIndex = INTEGER(speciesNum)[0];
  CSpecies* speciesPtr = cSpeciesTablePtr->species + speciesIndex;

  size_t vecSize = 0;
  CInteraction *interactions = NULL;

  if (predatorPreyType == PREDATOR) {
    vecSize      = speciesPtr->numPredators;
    interactions = speciesPtr->predators;
  } else if (predatorPreyType == PREY) {
    vecSize      = speciesPtr->numPrey;
    interactions = speciesPtr->prey;
  } else return L_returnMessage("incorrect predatorPreyType");
  
  if (vecSize < 1) return R_NilValue;
  
  SEXP speciesNumVec            = NEW_INTEGER(vecSize);
  int *speciesNumData           = INTEGER(speciesNumVec);
  SEXP speciesAttackVec         = NEW_NUMERIC(vecSize);
  double *speciesAttackData     = REAL(speciesAttackVec);
  SEXP speciesConversionVec     = NEW_NUMERIC(vecSize);
  double *speciesConversionData = REAL(speciesConversionVec);
  SEXP speciesTimeLagVec        = NEW_INTEGER(vecSize);
  int *speciesTimeLagData       = INTEGER(speciesTimeLagVec);
  for(size_t i = 0; i < vecSize; i++) {
    speciesNumData[i]        = interactions[i].speciesIndex;
    speciesAttackData[i]     = interactions[i].attackRate;
    speciesConversionData[i] = interactions[i].conversionRate;
    speciesTimeLagData[i]    = interactions[i].timeLag;
  }
  SEXP result = NEW_LIST(4);
  SET_VECTOR_ELT(result, 0, speciesNumVec);
  SET_VECTOR_ELT(result, 1, speciesAttackVec);
  SET_VECTOR_ELT(result, 2, speciesConversionVec);
  SET_VECTOR_ELT(result, 3, speciesTimeLagVec);
  return result;
}

SEXP L_setPredatorPreyCoefficients(int predatorPreyType, 
                                   SEXP cSpeciesTable,
                                   SEXP speciesNum,
                                   SEXP speciesNumVec,
                                   SEXP speciesAttackVec,
                                   SEXP speciesConversionVec,
                                   SEXP speciesTimeLagVec) {
  SEXP result = NEW_LOGICAL(1);
  LOGICAL(result)[0] = FALSE;
  L_assertSpeciesTable("cSpeciesTable", cSpeciesTable);
  CSpeciesTable *cSpeciesTablePtr =
    (CSpeciesTable*)R_ExternalPtrAddr(cSpeciesTable);
  L_assertNotNull("cSpeciesTablePtr", cSpeciesTablePtr);
  L_assertAnIntegerInRange("speciesNum", speciesNum, 0, cSpeciesTablePtr->numSpecies);
  size_t speciesIndex = INTEGER(speciesNum)[0];
  CSpecies* speciesPtr = cSpeciesTablePtr->species + speciesIndex;
  L_assertIntegerVector("speciesNum", speciesNumVec, 0);
  int *speciesNumData = INTEGER(speciesNumVec);
  size_t vecSize = GET_LENGTH(speciesNumVec);
  L_assertDoubleVector("speciesAttack", speciesAttackVec, vecSize);
  double *speciesAttackData = REAL(speciesAttackVec);
  L_assertDoubleVector("speciesConversion", speciesConversionVec, vecSize);
  double *speciesConversionData = REAL(speciesConversionVec);
  L_assertIntegerVector("speciesTimeLag", speciesTimeLagVec, vecSize);
  int *speciesTimeLagData = INTEGER(speciesTimeLagVec);
  CInteraction* interactions = (CInteraction*)Calloc(vecSize, CInteraction);
  for(size_t i = 0; i < vecSize; i++) {
    // check to ensure the species index is valid for this species table
    if ((speciesNumData[i] < 0) || (cSpeciesTablePtr->numSpecies <= speciesNumData[i])) {
      Free(interactions);
      return result;
    }
    interactions[i].speciesIndex   = speciesNumData[i];
    interactions[i].attackRate     = speciesAttackData[i];
    interactions[i].conversionRate = speciesConversionData[i];
    interactions[i].timeLag        = speciesTimeLagData[i];
  }
  if (predatorPreyType == PREDATOR) {
    if (speciesPtr->numPredators) {
      Free(speciesPtr->predators);
    }
    speciesPtr->numPredators = vecSize;
    speciesPtr->predators    = interactions;
  } else if (predatorPreyType == PREY) {
    if (speciesPtr->numPrey) {
      Free(speciesPtr->prey);
    }
    speciesPtr->numPrey = vecSize;
    speciesPtr->prey    = interactions;
  } else {
    Free(interactions);
    return result;
  }
  LOGICAL(result)[0] = TRUE;
  return result;
}

SEXP C_getPredatorCoefficients(SEXP cSpeciesTable, SEXP speciesNum) {
  return L_getPredatorPreyCoefficients(PREDATOR, cSpeciesTable, speciesNum);
}

SEXP C_setPredatorCoefficients(SEXP cSpeciesTable, 
                               SEXP speciesNum,
                               SEXP speciesNumVec,
                               SEXP speciesAttackVec,
                               SEXP speciesConversionVec,
                               SEXP speciesTimeLagVec) {
  return L_setPredatorPreyCoefficients(PREDATOR,
                                       cSpeciesTable,
                                       speciesNum,
                                       speciesNumVec,
                                       speciesAttackVec,
                                       speciesConversionVec,
                                       speciesTimeLagVec);
}

SEXP C_getPreyCoefficients(SEXP cSpeciesTable, SEXP speciesNum) {
  return L_getPredatorPreyCoefficients(PREY, cSpeciesTable, speciesNum);
}

SEXP C_setPreyCoefficients(SEXP cSpeciesTable, 
                           SEXP speciesNum, 
                           SEXP speciesNumVec, 
                           SEXP speciesAttackVec,
                           SEXP speciesConversionVec,
                           SEXP speciesTimeLagVec) {
  return L_setPredatorPreyCoefficients(PREY, 
                                       cSpeciesTable, 
                                       speciesNum, 
                                       speciesNumVec, 
                                       speciesAttackVec,
                                       speciesConversionVec,
                                       speciesTimeLagVec);
}

static R_CallMethodDef CModelBuilder_callMethods[] = {
  { "C_newSpeciesTable",         (DL_FUNC) &C_newSpeciesTable,         1},
  { "C_isSpeciesTable",          (DL_FUNC) &C_isSpeciesTable,          1},
  { "C_numSpecies",              (DL_FUNC) &C_numSpecies,              1},
  { "C_getSpeciesValues",        (DL_FUNC) &C_getSpeciesValues,        2},
  { "C_setSpeciesValues",        (DL_FUNC) &C_setSpeciesValues,        3},
  { "C_getPredatorCoefficients", (DL_FUNC) &C_getPredatorCoefficients, 2},
  { "C_setPredatorCoefficients", (DL_FUNC) &C_setPredatorCoefficients, 6},
  { "C_getPreyCoefficients",     (DL_FUNC) &C_getPreyCoefficients,     2},
  { "C_setPreyCoefficients",     (DL_FUNC) &C_setPreyCoefficients,     6},
  { NULL, NULL, 0}
};

void registerCModelBuilder(DllInfo *info) {
  R_registerRoutines(info, NULL, CModelBuilder_callMethods, NULL, NULL);
}
