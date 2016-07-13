// Build C structures used to simulate a model

#include <R.h>
#include <Rdefines.h>
#include <R_ext/Rdynload.h>

#define CSpeciesTable_TAG 1

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

void C_newSpeciesTable_Finalizer(SEXP cSpeciesTable) {
  if (L_isSpeciesTable(cSpeciesTable)) {
    CSpeciesTable *cSpeciesTablePtr = (CSpeciesTable*)R_ExternalPtrAddr(cSpeciesTable);
    Free(cSpeciesTablePtr->data);
    Free(cSpeciesTablePtr);
  }
}

SEXP C_newSpeciesTable(SEXP numSpecies) {
  if (!numSpecies) return R_NilValue;
  if (!IS_INTEGER(numSpecies)) return R_NilValue;
  if (GET_LENGTH(numSpecies) < 1) return R_NilValue;
  CSpeciesTable *cSpeciesTablePtr = Calloc(1, CSpeciesTable);
  cSpeciesTablePtr->tag  = CSpeciesTable_TAG;
  cSpeciesTablePtr->size = INTEGER(numSpecies)[0];
  cSpeciesTablePtr->data = (double*)Calloc((3*(cSpeciesTablePtr->size)), double);
  SEXP cSpeciesTable = R_MakeExternalPtr(cSpeciesTablePtr, R_NilValue, R_NilValue);
  R_RegisterCFinalizerEx(cSpeciesTable, (R_CFinalizer_t) &C_newSpeciesTable_Finalizer, TRUE);
  return cSpeciesTable;
}

SEXP C_isSpeciesTable(SEXP cSpeciesTable) {
  SEXP sexpResult = NEW_LOGICAL(1);
  LOGICAL(sexpResult)[0] = L_isSpeciesTable(cSpeciesTable);
  return sexpResult;
}

static R_CallMethodDef callMethods[] = {
  { "C_newSpeciesTable",   (DL_FUNC) &C_newSpeciesTable,    0},
  { "C_isSpeciesTable",    (DL_FUNC) &C_isSpeciesTable,     1},
  { NULL, NULL, 0}
};

void registerCModelBuilder(DllInfo *info) {
  R_registerRoutines(info, NULL, callMethods, NULL, NULL);
}
