// Build C structures used to simulate a model

#include <R.h>
#include <Rdefines.h>
#include <R_ext/Rdynload.h>

#define CSpeciesTable_TAG 1
#define MAX_NUM_SPECIES 10000

typedef struct CSpeciesTable_STRUCT {
  int tag;
  size_t size;
  double *data;
} CSpeciesTable;

int L_isSpeciesTable(SEXP cSpeciesTable) {
  int result = FALSE;
  if (!cSpeciesTable) return FALSE;
  if (TYPEOF(cSpeciesTable) != EXTPTRSXP) return FALSE;
  CSpeciesTable *cSpeciesTablePtr = (CSpeciesTable*)R_ExternalPtrAddr(cSpeciesTable);
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

int L_isDoubleVector(SEXP aVector, size_t vectorSize) {
  if (!aVector) return FALSE;
  if (!IS_VECTOR(aVector)) return FALSE;
  if (!IS_NUMERIC(aVector)) return FALSE;
  if (GET_LENGTH(aVector) != vectorSize) return FALSE;
  return TRUE;
}
void C_newSpeciesTable_Finalizer(SEXP cSpeciesTable) {
  if (L_isSpeciesTable(cSpeciesTable)) {
    CSpeciesTable *cSpeciesTablePtr = (CSpeciesTable*)R_ExternalPtrAddr(cSpeciesTable);
    Free(cSpeciesTablePtr->data);
    Free(cSpeciesTablePtr);
  }
}

SEXP C_newSpeciesTable(SEXP numSpecies) {
  if (!L_isAnIntegerInRange(numSpecies, 0, MAX_NUM_SPECIES)) return R_NilValue;
  CSpeciesTable *cSpeciesTablePtr = Calloc(1, CSpeciesTable);
  cSpeciesTablePtr->tag  = CSpeciesTable_TAG;
  cSpeciesTablePtr->size = INTEGER(numSpecies)[0];
  cSpeciesTablePtr->data = (double*)Calloc((3*(cSpeciesTablePtr->size)), double);
  double* dataPtr = cSpeciesTablePtr->data;
  double* dataMax = dataPtr + 3*(cSpeciesTablePtr->size);
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

SEXP C_numSpecies(SEXP cSpeciesTable) {
  SEXP result = NEW_INTEGER(1);
  INTEGER(result)[0] = 0;
  if (!L_isSpeciesTable(cSpeciesTable)) return result;
  CSpeciesTable *cSpeciesTablePtr = (CSpeciesTable*)R_ExternalPtrAddr(cSpeciesTable);
  INTEGER(result)[0] = cSpeciesTablePtr->size;
  return result;
}

#define SPECIES_PTR(cSpeciesTablePtr, speciesIndex) ((cSpeciesTablePtr)->data + (speciesIndex)*3)

SEXP C_getSpeciesValues(SEXP cSpeciesTable, SEXP speciesNum) {
  SEXP result = NEW_NUMERIC(3);
  REAL(result)[0] = NA_REAL;
  REAL(result)[1] = NA_REAL;
  REAL(result)[2] = NA_REAL;
  if (!L_isSpeciesTable(cSpeciesTable)) return result;
  CSpeciesTable *cSpeciesTablePtr = (CSpeciesTable*)R_ExternalPtrAddr(cSpeciesTable); 
  if (!L_isAnIntegerInRange(speciesNum, 0, cSpeciesTablePtr->size)) return result;
  size_t speciesIndex = INTEGER(speciesNum)[0];
  double* speciesPtr = SPECIES_PTR(cSpeciesTablePtr, speciesIndex);
  REAL(result)[0] = speciesPtr[0];
  REAL(result)[1] = speciesPtr[1];
  REAL(result)[2] = speciesPtr[2];
  return result;
}

SEXP C_setSpeciesValues(SEXP cSpeciesTable, SEXP speciesNum, SEXP speciesValues) {
  SEXP result = NEW_LOGICAL(1);
  LOGICAL(result)[0] = FALSE;
  if (!L_isSpeciesTable(cSpeciesTable)) return result;
  CSpeciesTable *cSpeciesTablePtr = (CSpeciesTable*)R_ExternalPtrAddr(cSpeciesTable); 
  if (!L_isAnIntegerInRange(speciesNum, 0, cSpeciesTablePtr->size)) return result;
  size_t speciesIndex = INTEGER(speciesNum)[0];
  double* speciesPtr = SPECIES_PTR(cSpeciesTablePtr, speciesIndex);
  if (!L_isDoubleVector(speciesValues, 3)) return result;
  speciesPtr[0] = REAL(speciesValues)[0];
  speciesPtr[1] = REAL(speciesValues)[1];
  speciesPtr[2] = REAL(speciesValues)[2];
  LOGICAL(result)[0] = TRUE;
  return result;
}

static R_CallMethodDef callMethods[] = {
  { "C_newSpeciesTable",   (DL_FUNC) &C_newSpeciesTable,    1},
  { "C_isSpeciesTable",    (DL_FUNC) &C_isSpeciesTable,     1},
  { "C_numSpecies",        (DL_FUNC) &C_numSpecies,         1},
  { "C_getSpeciesValues",  (DL_FUNC) &C_getSpeciesValues,   2},
  { "C_setSpeciesValues",  (DL_FUNC) &C_setSpeciesValues,   3},
  { NULL, NULL, 0}
};

void registerCModelBuilder(DllInfo *info) {
  R_registerRoutines(info, NULL, callMethods, NULL, NULL);
}
