// Build C structures used to simulate a model

#include <R.h>
#include <Rdefines.h>
#include <R_ext/Rdynload.h>

#define CSpeciesTable_TAG       1
#define CInteractionsTable_TAG  2
#define MAX_NUM_SPECIES         10000

#define SPECIES_GROWTH_RATE     0
#define SPECIES_CARRYING_CAP    1
#define SPECIES_MORTALITY       2
#define SPECIES_HALF_SAT        3
#define SPECIES_PRED_TMP        4
#define NUM_SPECIES_DOUBLES     5

typedef struct CSpeciesTable_STRUCT {
  int tag;
  size_t numSpecies;
  double *data;
} CSpeciesTable;

typedef struct CInteraction_STRUCT {
  size_t speciesIndex;
  double coefficient;
} CInteraction;

typedef struct CInteractionVec_STRUCT {
  size_t numInteractions;
  CInteraction *interactions;
} CInteractionVec;

typedef struct CInteractionsTable_STRUCT {
  int tag;
  size_t numSpecies;
  CInteractionVec *interactionVecs;
} CInteractionsTable;

int L_isSpeciesTable(SEXP cSpeciesTable) {
  if (!cSpeciesTable) return FALSE;
  if (TYPEOF(cSpeciesTable) != EXTPTRSXP) return FALSE;
  CSpeciesTable *cSpeciesTablePtr = 
    (CSpeciesTable*)R_ExternalPtrAddr(cSpeciesTable);
  if (!cSpeciesTablePtr) return FALSE;
  if (cSpeciesTablePtr->tag != CSpeciesTable_TAG) return FALSE;
  return TRUE;
}

int L_isInteractionsTable(SEXP cInteractionsTable) {
  if (!cInteractionsTable) return FALSE;
  if (TYPEOF(cInteractionsTable) != EXTPTRSXP) return FALSE;
  CInteractionsTable *cInteractionsTablePtr =
    (CInteractionsTable*)R_ExternalPtrAddr(cInteractionsTable);
  if (!cInteractionsTablePtr) return FALSE;
  if (cInteractionsTablePtr->tag != CInteractionsTable_TAG) return FALSE;
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

int L_isDoubleVector(SEXP aVector, size_t vectorSize) {
  if (!aVector) return FALSE;
  if (!IS_VECTOR(aVector)) return FALSE;
  if (!IS_NUMERIC(aVector)) return FALSE;
  if (GET_LENGTH(aVector) != vectorSize) return FALSE;
  return TRUE;
}

void C_newSpeciesTable_Finalizer(SEXP cSpeciesTable) {
  if (L_isSpeciesTable(cSpeciesTable)) {
    CSpeciesTable *cSpeciesTablePtr =
      (CSpeciesTable*)R_ExternalPtrAddr(cSpeciesTable);
    Free(cSpeciesTablePtr->data);
    Free(cSpeciesTablePtr);
  }
}

SEXP C_newSpeciesTable(SEXP numSpecies) {
  if (!L_isAnIntegerInRange(numSpecies, 0, MAX_NUM_SPECIES)) return R_NilValue;
  CSpeciesTable *cSpeciesTablePtr = 
    (CSpeciesTable*)Calloc(1, CSpeciesTable);
  cSpeciesTablePtr->tag        = CSpeciesTable_TAG;
  cSpeciesTablePtr->numSpecies = INTEGER(numSpecies)[0];
  cSpeciesTablePtr->data       = 
    (double*)Calloc((NUM_SPECIES_DOUBLES*(cSpeciesTablePtr->numSpecies)), double);
  double* dataPtr = cSpeciesTablePtr->data;
  double* dataMax = dataPtr + NUM_SPECIES_DOUBLES*(cSpeciesTablePtr->numSpecies);
  for(; dataPtr < dataMax; dataPtr++) *dataPtr = NA_REAL;
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
  if (L_isSpeciesTable(cTable)) {
    CSpeciesTable *cSpeciesTablePtr = (CSpeciesTable*)R_ExternalPtrAddr(cTable);
    INTEGER(result)[0] = cSpeciesTablePtr->numSpecies;
  } else if (L_isInteractionsTable(cTable)) {
    CInteractionsTable *cInteractionsTablePtr = (CInteractionsTable*)R_ExternalPtrAddr(cTable);
    INTEGER(result)[0] = cInteractionsTablePtr->numSpecies;
  } else return result;
  return result;
}

#define SPECIES_PTR(cSpeciesTablePtr, speciesIndex) ((cSpeciesTablePtr)->data + (speciesIndex)*NUM_SPECIES_DOUBLES)

SEXP C_getSpeciesValues(SEXP cSpeciesTable, SEXP speciesNum) {
  SEXP result = NEW_NUMERIC(4);
  REAL(result)[SPECIES_GROWTH_RATE]  = NA_REAL;
  REAL(result)[SPECIES_CARRYING_CAP] = NA_REAL;
  REAL(result)[SPECIES_MORTALITY]    = NA_REAL;
  REAL(result)[SPECIES_HALF_SAT]     = NA_REAL;
  if (!L_isSpeciesTable(cSpeciesTable)) return result;
  CSpeciesTable *cSpeciesTablePtr = (CSpeciesTable*)R_ExternalPtrAddr(cSpeciesTable); 
  if (!L_isAnIntegerInRange(speciesNum, 0, cSpeciesTablePtr->numSpecies)) return result;
  size_t speciesIndex = INTEGER(speciesNum)[0];
  double* speciesPtr = SPECIES_PTR(cSpeciesTablePtr, speciesIndex);
  REAL(result)[SPECIES_GROWTH_RATE]  = speciesPtr[SPECIES_GROWTH_RATE];
  REAL(result)[SPECIES_CARRYING_CAP] = speciesPtr[SPECIES_CARRYING_CAP];
  REAL(result)[SPECIES_MORTALITY]    = speciesPtr[SPECIES_MORTALITY];
  REAL(result)[SPECIES_HALF_SAT]     = speciesPtr[SPECIES_HALF_SAT];
  return result;
}

SEXP C_setSpeciesValues(SEXP cSpeciesTable, SEXP speciesNum, SEXP speciesValues) {
  SEXP result = NEW_LOGICAL(1);
  LOGICAL(result)[0] = FALSE;
  if (!L_isSpeciesTable(cSpeciesTable)) return result;
  CSpeciesTable *cSpeciesTablePtr = (CSpeciesTable*)R_ExternalPtrAddr(cSpeciesTable); 
  if (!L_isAnIntegerInRange(speciesNum, 0, cSpeciesTablePtr->numSpecies)) return result;
  size_t speciesIndex = INTEGER(speciesNum)[0];
  double* speciesPtr = SPECIES_PTR(cSpeciesTablePtr, speciesIndex);
  if (!L_isDoubleVector(speciesValues, 4)) return result;
  speciesPtr[SPECIES_GROWTH_RATE]  = REAL(speciesValues)[SPECIES_GROWTH_RATE];
  speciesPtr[SPECIES_CARRYING_CAP] = REAL(speciesValues)[SPECIES_CARRYING_CAP];
  speciesPtr[SPECIES_MORTALITY]    = REAL(speciesValues)[SPECIES_MORTALITY];
  speciesPtr[SPECIES_HALF_SAT]     = REAL(speciesValues)[SPECIES_HALF_SAT];
  LOGICAL(result)[0] = TRUE;
  return result;
}

void C_newInteractionsTable_Finalizer(SEXP cInteractionsTable) {
    if (L_isInteractionsTable(cInteractionsTable)) {
    CInteractionsTable *cInteractionsTablePtr = 
      (CInteractionsTable*)R_ExternalPtrAddr(cInteractionsTable);
    CInteractionVec *interactionVec = 
      cInteractionsTablePtr->interactionVecs;
    CInteractionVec *maxInteractionVec = 
      interactionVec + 2*(cInteractionsTablePtr->numSpecies);
    for(; interactionVec < maxInteractionVec; interactionVec++) {
      if (interactionVec) {
        Free(interactionVec->interactions);
      }
    }
    Free(cInteractionsTablePtr->interactionVecs);
    Free(cInteractionsTablePtr);
  }
}

SEXP C_newInteractionsTable(SEXP numSpecies) {
  if (!L_isAnIntegerInRange(numSpecies, 0, MAX_NUM_SPECIES)) return R_NilValue;
  CInteractionsTable *cInteractionsTablePtr = Calloc(1, CInteractionsTable);
  cInteractionsTablePtr->tag  = CInteractionsTable_TAG;
  cInteractionsTablePtr->numSpecies = INTEGER(numSpecies)[0];
  cInteractionsTablePtr->interactionVecs = 
    (CInteractionVec*)Calloc((2*(cInteractionsTablePtr->numSpecies)), CInteractionVec);
  CInteractionVec *interactionVec = 
    cInteractionsTablePtr->interactionVecs;
  CInteractionVec *maxInteractionVec = 
    interactionVec + 2*(cInteractionsTablePtr->numSpecies);
  for(; interactionVec < maxInteractionVec; interactionVec++) {
    interactionVec->numInteractions = 0;
    interactionVec->interactions = NULL;
  }
  SEXP cInteractionsTable = R_MakeExternalPtr(cInteractionsTablePtr, R_NilValue, R_NilValue);
  R_RegisterCFinalizerEx(cInteractionsTable, (R_CFinalizer_t) &C_newInteractionsTable_Finalizer, TRUE);
  return cInteractionsTable;
}

SEXP C_isInteractionsTable(SEXP cInteractionsTable) {
  SEXP result = NEW_LOGICAL(1);
  LOGICAL(result)[0] = L_isInteractionsTable(cInteractionsTable);
  return result;
}

static R_CallMethodDef CModelBuilder_callMethods[] = {
  { "C_newSpeciesTable",      (DL_FUNC) &C_newSpeciesTable,      1},
  { "C_isSpeciesTable",       (DL_FUNC) &C_isSpeciesTable,       1},
  { "C_numSpecies",           (DL_FUNC) &C_numSpecies,           1},
  { "C_getSpeciesValues",     (DL_FUNC) &C_getSpeciesValues,     2},
  { "C_setSpeciesValues",     (DL_FUNC) &C_setSpeciesValues,     3},
  { "C_newInteractionsTable", (DL_FUNC) &C_newInteractionsTable, 1},
  { "C_isInteractionsTable",  (DL_FUNC) &C_isInteractionsTable,  1},
  { NULL, NULL, 0}
};

void registerCModelBuilder(DllInfo *info) {
  R_registerRoutines(info, NULL, CModelBuilder_callMethods, NULL, NULL);
}
