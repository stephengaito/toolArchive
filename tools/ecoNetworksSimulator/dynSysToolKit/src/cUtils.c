// Utilities

#include "dynSysToolKit.h"

SEXP L_returnMessage(const char *message) {
  SEXP result;
  PROTECT(result = NEW_CHARACTER(1));
  SET_STRING_ELT(result, 0, mkChar(message));
  UNPROTECT(1);
  return result;
}

void L_assertNotNull(const char* objName, void* aPointer) {
  if (!aPointer) Rf_error("%s is null", objName);
}

int L_assertAnIntegerInRange(const char* objName, SEXP anInt, int min, int max) {
  if (!anInt) Rf_error("%s is a null pointer", objName);
  if (!IS_INTEGER(anInt)) Rf_error("%s is not an integer", objName);
  if (GET_LENGTH(anInt) < 1) Rf_error("%s has zero length", objName);
  int theInt = INTEGER(anInt)[0];
  if ( theInt < min) Rf_error("%s is too small (%d < %d)", objName, INTEGER(anInt)[0], min);
  if (max <= theInt) Rf_error("%s is too big (%d <= %d)", objName, max, INTEGER(anInt)[0]);
  return theInt;
}

double L_assertADoubleInRange(const char* objName, SEXP aDouble, double min, double max) {
  if (!aDouble) Rf_error("%s is a null pointer", objName);
  if (!IS_NUMERIC(aDouble)) Rf_error("%s is not a dobule", objName);
  if (GET_LENGTH(aDouble) < 1) Rf_error("%s has zero length", objName);
  double theDouble = REAL(aDouble)[0];
  if (theDouble < min) Rf_error("%s is too small (%e < %e)", objName, REAL(aDouble)[0], min);
  if (max <= theDouble) Rf_error("%s is too big (%e <= %e)", objName, max, REAL(aDouble)[0]);
  return theDouble;
}

int* L_assertIntegerVector(const char* objName, SEXP aVector, size_t vectorSize) {
  if (!aVector) Rf_error("%s is a null pointer", objName);
  if (!IS_VECTOR(aVector)) Rf_error("%s is not a vector", objName);
  if (!IS_INTEGER(aVector)) Rf_error("%s is not an integer vector", objName);
  if ((0 < vectorSize) && (GET_LENGTH(aVector) != vectorSize)) 
    Rf_error("%s does not have the required %d elements", objName, vectorSize);
  return INTEGER(aVector);
}

double* L_assertDoubleVector(const char* objName, SEXP aVector, size_t vectorSize) {
  if (!aVector) Rf_error("%s is a null pointer", objName);
  if (!IS_VECTOR(aVector)) Rf_error("%s is not a vector", objName);
  if (!IS_NUMERIC(aVector)) Rf_error("%s is not a numeric vector", objName);
  if ((0 < vectorSize) && (GET_LENGTH(aVector) != vectorSize)) 
    Rf_error("%s does not have the required %d elements", objName, vectorSize);
  return REAL(aVector);
}
