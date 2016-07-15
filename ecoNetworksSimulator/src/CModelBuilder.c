// Build C structures used to simulate a model

#include <R.h>
#include <Rdefines.h>
#include <R_ext/Rdynload.h>

#define CSpeciesTable_TAG       1
#define MAX_NUM_SPECIES         10000

#define SPECIES_GROWTH_RATE       0
#define SPECIES_CARRYING_CAPACITY 1
#define SPECIES_MORTALITY         2
#define SPECIES_HALF_SATURATION   3
#define SPECIES_NUM_VALUES        4

#define PREDATOR  1
#define PREY      2

typedef struct CInteraction_STRUCT {
  size_t speciesIndex;
  double attackRate;
  double conversionRate;
} CInteraction;

typedef struct CSpecies_STRUCT {
  double growthRate;
  double carryingCapacity;
  double mortality;
  double halfSaturation;
  double predationFactor;
  size_t numPredators;
  CInteraction *predators;
  size_t numPrey;
  CInteraction *prey;
} CSpecies;

typedef struct CSpeciesTable_STRUCT {
  int tag;
  size_t numSpecies;
  CSpecies *species;
} CSpeciesTable;

int L_isSpeciesTable(SEXP cSpeciesTable) {
  if (!cSpeciesTable) return FALSE;
  if (TYPEOF(cSpeciesTable) != EXTPTRSXP) return FALSE;
  CSpeciesTable *cSpeciesTablePtr = 
    (CSpeciesTable*)R_ExternalPtrAddr(cSpeciesTable);
  if (!cSpeciesTablePtr) return FALSE;
  if (cSpeciesTablePtr->tag != CSpeciesTable_TAG) return FALSE;
  return TRUE;
}

int L_isAnIntegerInRange(SEXP anInt, int min, int max) {
  if (!anInt) return FALSE;
  if (!IS_INTEGER(anInt)) return FALSE;
  if (GET_LENGTH(anInt) < 1) return FALSE;
  if (INTEGER(anInt)[0] < min) return FALSE;
  if (max <= INTEGER(anInt)[0]) return FALSE;
  return TRUE;
}

int L_isIntegerVector(SEXP aVector, size_t vectorSize) {
  if (!aVector) return FALSE;
  if (!IS_VECTOR(aVector)) return FALSE;
  if (!IS_INTEGER(aVector)) return FALSE;
  if ((0 < vectorSize) && (GET_LENGTH(aVector) != vectorSize)) return FALSE;
  return TRUE;
}

int L_isDoubleVector(SEXP aVector, size_t vectorSize) {
  if (!aVector) return FALSE;
  if (!IS_VECTOR(aVector)) return FALSE;
  if (!IS_NUMERIC(aVector)) return FALSE;
  if ((0 < vectorSize) && (GET_LENGTH(aVector) != vectorSize)) return FALSE;
  return TRUE;
}

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
  if (!L_isAnIntegerInRange(numSpecies, 0, MAX_NUM_SPECIES)) return R_NilValue;
  CSpeciesTable *cSpeciesTablePtr = 
    (CSpeciesTable*)Calloc(1, CSpeciesTable);
  cSpeciesTablePtr->tag        = CSpeciesTable_TAG;
  cSpeciesTablePtr->numSpecies = INTEGER(numSpecies)[0];
  cSpeciesTablePtr->species       = 
    (CSpecies*)Calloc(cSpeciesTablePtr->numSpecies, CSpecies);
  CSpecies* species = cSpeciesTablePtr->species;
  CSpecies* maxSpecies = species + cSpeciesTablePtr->numSpecies;
  for(; species < maxSpecies; species++) {
    species->growthRate       = NA_REAL;
    species->carryingCapacity = NA_REAL;
    species->mortality        = NA_REAL;
    species->halfSaturation   = NA_REAL;
    species->predationFactor  = 0.0;
    species->numPredators     = 0;
    species->predators        = NULL;
    species->numPrey          = 0;
    species->prey             = NULL;
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
  SEXP result = NEW_NUMERIC(SPECIES_NUM_VALUES);
  REAL(result)[SPECIES_GROWTH_RATE]       = NA_REAL;
  REAL(result)[SPECIES_CARRYING_CAPACITY] = NA_REAL;
  REAL(result)[SPECIES_MORTALITY]         = NA_REAL;
  REAL(result)[SPECIES_HALF_SATURATION]   = NA_REAL;
  if (!L_isSpeciesTable(cSpeciesTable)) return result;
  CSpeciesTable *cSpeciesTablePtr = (CSpeciesTable*)R_ExternalPtrAddr(cSpeciesTable); 
  if (!L_isAnIntegerInRange(speciesNum, 0, cSpeciesTablePtr->numSpecies)) return result;
  size_t speciesIndex = INTEGER(speciesNum)[0];
  CSpecies* speciesPtr = cSpeciesTablePtr->species + speciesIndex;
  REAL(result)[SPECIES_GROWTH_RATE]       = speciesPtr->growthRate;
  REAL(result)[SPECIES_CARRYING_CAPACITY] = speciesPtr->carryingCapacity;
  REAL(result)[SPECIES_MORTALITY]         = speciesPtr->mortality;
  REAL(result)[SPECIES_HALF_SATURATION]   = speciesPtr->halfSaturation;
  return result;
}

SEXP C_setSpeciesValues(SEXP cSpeciesTable, SEXP speciesNum, SEXP speciesValues) {
  SEXP result = NEW_LOGICAL(1);
  LOGICAL(result)[0] = FALSE;
  if (!L_isSpeciesTable(cSpeciesTable)) return result;
  CSpeciesTable *cSpeciesTablePtr = (CSpeciesTable*)R_ExternalPtrAddr(cSpeciesTable); 
  if (!cSpeciesTablePtr) return result;
  if (!L_isAnIntegerInRange(speciesNum, 0, cSpeciesTablePtr->numSpecies)) return result;
  size_t speciesIndex = INTEGER(speciesNum)[0];
  CSpecies* speciesPtr = cSpeciesTablePtr->species + speciesIndex;
  if (!L_isDoubleVector(speciesValues, SPECIES_NUM_VALUES)) return result;
  speciesPtr->growthRate       = REAL(speciesValues)[SPECIES_GROWTH_RATE];
  speciesPtr->carryingCapacity = REAL(speciesValues)[SPECIES_CARRYING_CAPACITY];
  speciesPtr->mortality        = REAL(speciesValues)[SPECIES_MORTALITY];
  speciesPtr->halfSaturation   = REAL(speciesValues)[SPECIES_HALF_SATURATION];
  LOGICAL(result)[0] = TRUE;
  return result;
}

SEXP L_getPredatorPreyCoefficients(int predatorPreyType, SEXP cSpeciesTable, SEXP speciesNum) {
  if (!L_isSpeciesTable(cSpeciesTable)) return R_NilValue;
  CSpeciesTable *cSpeciesTablePtr =
    (CSpeciesTable*)R_ExternalPtrAddr(cSpeciesTable);
  if(!cSpeciesTablePtr) return R_NilValue;
  if (!L_isAnIntegerInRange(speciesNum, 0, cSpeciesTablePtr->numSpecies)) return R_NilValue;
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
  } else return R_NilValue;
  
  if (vecSize < 1) return R_NilValue;
  
  SEXP speciesNumVec            = NEW_INTEGER(vecSize);
  int *speciesNumData           = INTEGER(speciesNumVec);
  SEXP speciesAttackVec         = NEW_NUMERIC(vecSize);
  double *speciesAttackData     = REAL(speciesAttackVec);
  SEXP speciesConversionVec     = NEW_NUMERIC(vecSize);
  double *speciesConversionData = REAL(speciesConversionVec);
  for(size_t i = 0; i < vecSize; i++) {
    speciesNumData[i]        = interactions[i].speciesIndex;
    speciesAttackData[i]     = interactions[i].attackRate;
    speciesConversionData[i] = interactions[i].conversionRate;
  }
  SEXP result = NEW_LIST(3);
  SET_VECTOR_ELT(result, 0, speciesNumVec);
  SET_VECTOR_ELT(result, 1, speciesAttackVec);
  SET_VECTOR_ELT(result, 2, speciesConversionVec);
  return result;
}

SEXP L_setPredatorPreyCoefficients(int predatorPreyType, 
                                   SEXP cSpeciesTable,
                                   SEXP speciesNum,
                                   SEXP speciesNumVec,
                                   SEXP speciesAttackVec,
                                   SEXP speciesConversionVec) {
  SEXP result = NEW_LOGICAL(1);
  LOGICAL(result)[0] = FALSE;
  if (!L_isSpeciesTable(cSpeciesTable)) return result;
  CSpeciesTable *cSpeciesTablePtr =
    (CSpeciesTable*)R_ExternalPtrAddr(cSpeciesTable);
  if(!cSpeciesTablePtr) return result;
  if (!L_isAnIntegerInRange(speciesNum, 0, cSpeciesTablePtr->numSpecies)) return result;
  size_t speciesIndex = INTEGER(speciesNum)[0];
  CSpecies* speciesPtr = cSpeciesTablePtr->species + speciesIndex;
  if (!L_isIntegerVector(speciesNumVec, 0)) return result;
  int *speciesNumData = INTEGER(speciesNumVec);
  size_t vecSize = GET_LENGTH(speciesNumVec);
  if (!L_isDoubleVector(speciesAttackVec, vecSize)) return result;
  double *speciesAttackData = REAL(speciesAttackVec);
  if (!L_isDoubleVector(speciesConversionVec, vecSize)) return result;
  double *speciesConversionData = REAL(speciesConversionVec);
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
                               SEXP speciesConversionVec) {
  return L_setPredatorPreyCoefficients(PREDATOR,
                                       cSpeciesTable,
                                       speciesNum,
                                       speciesNumVec,
                                       speciesAttackVec,
                                       speciesConversionVec);
}

SEXP C_getPreyCoefficients(SEXP cSpeciesTable, SEXP speciesNum) {
  return L_getPredatorPreyCoefficients(PREY, cSpeciesTable, speciesNum);
}

SEXP C_setPreyCoefficients(SEXP cSpeciesTable, 
                           SEXP speciesNum, 
                           SEXP speciesNumVec, 
                           SEXP speciesAttackVec,
                           SEXP speciesConversionVec) {
  return L_setPredatorPreyCoefficients(PREY, 
                                       cSpeciesTable, 
                                       speciesNum, 
                                       speciesNumVec, 
                                       speciesAttackVec,
                                       speciesConversionVec);
}

static R_CallMethodDef CModelBuilder_callMethods[] = {
  { "C_newSpeciesTable",         (DL_FUNC) &C_newSpeciesTable,         1},
  { "C_isSpeciesTable",          (DL_FUNC) &C_isSpeciesTable,          1},
  { "C_numSpecies",              (DL_FUNC) &C_numSpecies,              1},
  { "C_getSpeciesValues",        (DL_FUNC) &C_getSpeciesValues,        2},
  { "C_setSpeciesValues",        (DL_FUNC) &C_setSpeciesValues,        3},
  { "C_getPredatorCoefficients", (DL_FUNC) &C_getPredatorCoefficients, 2},
  { "C_setPredatorCoefficients", (DL_FUNC) &C_setPredatorCoefficients, 5},
  { "C_getPreyCoefficients",     (DL_FUNC) &C_getPreyCoefficients,     2},
  { "C_setPreyCoefficients",     (DL_FUNC) &C_setPreyCoefficients,     5},
  { NULL, NULL, 0}
};

void registerCModelBuilder(DllInfo *info) {
  R_registerRoutines(info, NULL, CModelBuilder_callMethods, NULL, NULL);
}
