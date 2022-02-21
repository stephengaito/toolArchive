// Build C structures used to simulate a model

#include "CModels.h"

SEXP L_returnMessage(const char *message) {
  SEXP result;
  PROTECT(result = NEW_CHARACTER(1));
  SET_STRING_ELT(result, 0, mkChar(message));
  UNPROTECT(1);
  return result;
}

// We keep this version since it is used in the GC finalizer (and so we want to keep it very simple)
int L_isSpeciesTable(SEXP cSpeciesTable) {
  if (!cSpeciesTable) return FALSE;
  if (TYPEOF(cSpeciesTable) != EXTPTRSXP) return FALSE;
  CSpeciesTable *cSpeciesTablePtr = 
    (CSpeciesTable*)R_ExternalPtrAddr(cSpeciesTable);
  if (!cSpeciesTablePtr) return FALSE;
  if (cSpeciesTablePtr->tag != CSpeciesTable_TAG) return FALSE;
  return TRUE;
}

void L_assertSpeciesTable(const char* objName, SEXP cSpeciesTable) {
  if (!cSpeciesTable) Rf_error("%s is a null pointer", objName);
  if (TYPEOF(cSpeciesTable) != EXTPTRSXP) Rf_error("%s is not an EXTPTRSXP");
  CSpeciesTable *cSpeciesTablePtr = 
    (CSpeciesTable*)R_ExternalPtrAddr(cSpeciesTable);
  if (!cSpeciesTablePtr) Rf_error("%sPtr is a null pointer", objName);
  if (cSpeciesTablePtr->tag != CSpeciesTable_TAG) Rf_error("%s has the wrong tag", objName);
}

void L_assertNotNull(const char* objName, void* aPointer) {
  if (!aPointer) Rf_error("%s is null", objName);
}

void L_assertAnIntegerInRange(const char* objName, SEXP anInt, int min, int max) {
  if (!anInt) Rf_error("%s is a null pointer", objName);
  if (!IS_INTEGER(anInt)) Rf_error("%s is not an integer", objName);
  if (GET_LENGTH(anInt) < 1) Rf_error("%s has zero length", objName);
  if (INTEGER(anInt)[0] < min) Rf_error("%s is too small (%d < %d)", objName, INTEGER(anInt)[0], min);
  if (max <= INTEGER(anInt)[0]) Rf_error("%s is too big (%d <= %d)", objName, max, INTEGER(anInt)[0]);
}

void L_assertADoubleInRange(const char* objName, SEXP aDouble, double min, double max) {
  if (!aDouble) Rf_error("%s is a null pointer", objName);
  if (!IS_NUMERIC(aDouble)) Rf_error("%s is not a dobule", objName);
  if (GET_LENGTH(aDouble) < 1) Rf_error("%s has zero length", objName);
  if (REAL(aDouble)[0] < min) Rf_error("%s is too small (%e < %e)", objName, REAL(aDouble)[0], min);
  if (max <= REAL(aDouble)[0]) Rf_error("%s is too big (%e <= %e)", objName, max, REAL(aDouble)[0]);
}

void L_assertIntegerVector(const char* objName, SEXP aVector, size_t vectorSize) {
  if (!aVector) Rf_error("%s is a null pointer", objName);
  if (!IS_VECTOR(aVector)) Rf_error("%s is not a vector", objName);
  if (!IS_INTEGER(aVector)) Rf_error("%s is not an integer vector", objName);
  if ((0 < vectorSize) && (GET_LENGTH(aVector) != vectorSize)) 
    Rf_error("%s does not have the required %d elements", objName, vectorSize);
}

void L_assertDoubleVector(const char* objName, SEXP aVector, size_t vectorSize) {
  if (!aVector) Rf_error("%s is a null pointer", objName);
  if (!IS_VECTOR(aVector)) Rf_error("%s is not a vector", objName);
  if (!IS_NUMERIC(aVector)) Rf_error("%s is not a numeric vector", objName);
  if ((0 < vectorSize) && (GET_LENGTH(aVector) != vectorSize)) 
    Rf_error("%s does not have the required %d elements", objName, vectorSize);
}
