// The following has been adapted from the Wikipedia article:
//   [Xorshift](https://en.wikipedia.org/wiki/Xorshift)
//
// See also the paper:
//   [Xorshift RNGs](http://www.jstatsoft.org/v08/i14/paper)
//
// To convert the resulting uint64 into a double we use the 
// IEEE 754 double point encoding described in:
//   https://en.wikipedia.org/wiki/Double-precision_floating-point_format
//
// To understand how to verify the randomness of the generated numbers
// See: https://www.random.org/
// and: http://stackoverflow.com/questions/186619/how-to-unit-test-a-pseudo-random-number-generator
// and: http://stats.stackexchange.com/questions/157345/ks-test-for-uniform-distribution-in-range-1-to-1-in-r
//
#include <time.h>
#include "CModels.h"

double L_getARandomNumber(CSpeciesTable *cSpeciesTable) {
  register int p = cSpeciesTable->rngP;
  register const uint64_t s0 = cSpeciesTable->rngState[p];
  register uint64_t s1 = cSpeciesTable->rngState[p = (p + 1) & 15];
  s1 ^= s1 << 31; // a
  register uint64_t rnd = s1 ^ s0 ^ (s1 >> 11) ^ (s0 >> 30); // b, c
  cSpeciesTable->rngP = p;
  cSpeciesTable->rngState[p] = rnd;
  // 0xFFFFFFFFFFFFF is the largest uint64 exactly representable in a double.
  return ((double)((rnd * UINT64_C(1181783497276652981)) & 0xFFFFFFFFFFFFF))/((double)0xFFFFFFFFFFFFF);
}

SEXP C_initRNGState(SEXP cSpeciesTable, SEXP initType, SEXP initialRun) {
  // initType == 0 ... init with /dev/random or its equivalent on different OSs
  //                   if no equivalent can be found use init with time instead
  // initType == 1 ... init with ones
  // initType == 2 ... init with time
  // then take and ignore initalRun samples...
  L_assertSpeciesTable("cSpeciesTable", cSpeciesTable);
  CSpeciesTable *cSpeciesTablePtr =
    (CSpeciesTable*)R_ExternalPtrAddr(cSpeciesTable);
  L_assertNotNull("cSpeciesTablePtr", cSpeciesTablePtr);
  L_assertAnIntegerInRange("initType", initType, 0, 3);
  size_t initTypeValue = INTEGER(initType)[0];
  L_assertAnIntegerInRange("initialRun", initialRun, 0, MAX_ITERATIONS);
  size_t initialRunValue = INTEGER(initialRun)[0];
  if (initTypeValue == 1) {
    cSpeciesTablePtr->rngP = 0;
    for(size_t i = 0; i < 16; i++) {
      cSpeciesTablePtr->rngState[i] = 1;
    }    
  } else {
    // we do not yet implement initType == 0
    cSpeciesTablePtr->rngP = 0;
    srandom(time(NULL));
    for(size_t i = 0; i < 16; i++) {
      RNGStateItem value;
      value.sInt[0] = random();
      value.sInt[1] = random();
      cSpeciesTablePtr->rngState[i] = value.s64;
    }
  }
  for (size_t i = 0; i < initialRunValue; i++) L_getARandomNumber(cSpeciesTablePtr);
  SEXP result = NEW_LOGICAL(1);
  LOGICAL(result)[0] = TRUE;
  return result;
}

SEXP C_setRNGState(SEXP cSpeciesTable, SEXP rngStateVec) {
  SEXP result = NEW_LOGICAL(1);
  LOGICAL(result)[0] = FALSE;
  //
  L_assertSpeciesTable("cSpeciesTable", cSpeciesTable);
  CSpeciesTable *cSpeciesTablePtr =
    (CSpeciesTable*)R_ExternalPtrAddr(cSpeciesTable);
  L_assertNotNull("cSpeciesTablePtr", cSpeciesTablePtr);
  L_assertIntegerVector("rngState", rngStateVec, 33);
  int *rngStateData = INTEGER(rngStateVec);
  cSpeciesTablePtr->rngP = rngStateData[0] & 0xF;
  for(size_t i = 0; i < 16; i++) {
    RNGStateItem anItem;
    anItem.sInt[0] = rngStateData[i*2+1];
    anItem.sInt[1] = rngStateData[i*2+2];
    cSpeciesTablePtr->rngState[i] = anItem.s64;
  }
  //
  LOGICAL(result)[0] = TRUE;
  return result;
}

SEXP C_getRNGState(SEXP cSpeciesTable) {
  L_assertSpeciesTable("cSpeciesTable", cSpeciesTable);
  CSpeciesTable *cSpeciesTablePtr =
    (CSpeciesTable*)R_ExternalPtrAddr(cSpeciesTable);
  L_assertNotNull("cSpeciesTablePtr", cSpeciesTablePtr);
  SEXP result = NEW_NUMERIC(33);
  REAL(result)[0] = cSpeciesTablePtr->rngP;
  for(size_t i = 0; i < 16; i++) {
    RNGStateItem anItem;
    anItem.s64 = cSpeciesTablePtr->rngState[i];
    REAL(result)[i*2+1] = anItem.sInt[0];
    REAL(result)[i*2+2] = anItem.sInt[1];
  }
  return result;  
}

SEXP C_getARandomNumber(SEXP cSpeciesTable) {
  L_assertSpeciesTable("cSpeciesTable", cSpeciesTable);
  CSpeciesTable *cSpeciesTablePtr =
    (CSpeciesTable*)R_ExternalPtrAddr(cSpeciesTable);
  L_assertNotNull("cSpeciesTablePtr", cSpeciesTablePtr);
  SEXP result = NEW_NUMERIC(1);
  REAL(result)[0] = L_getARandomNumber(cSpeciesTablePtr);  
  return result;
}

SEXP C_getRandomNumbers(SEXP cSpeciesTable, SEXP resultVec) {
  // fill the resultVec with length(resultVec) samples
  L_assertSpeciesTable("cSpeciesTable", cSpeciesTable);
  CSpeciesTable *cSpeciesTablePtr =
    (CSpeciesTable*)R_ExternalPtrAddr(cSpeciesTable);
  L_assertNotNull("cSpeciesTablePtr", cSpeciesTablePtr);
  L_assertDoubleVector("results", resultVec, 0);
  size_t vecSize = GET_LENGTH(resultVec);
  double *resultData = REAL(resultVec);
  for (size_t i = 0; i < vecSize; i++) {
    resultData[i] = L_getARandomNumber(cSpeciesTablePtr);
  }
  return resultVec;
}

static R_CallMethodDef CXorShiftRNG_callMethods[] = {
  { "C_initRNGState",     (DL_FUNC) &C_initRNGState,     3},
  { "C_setRNGState",      (DL_FUNC) &C_setRNGState,      2},
  { "C_getRNGState",      (DL_FUNC) &C_getRNGState,      1},
  { "C_getARandomNumber", (DL_FUNC) &C_getARandomNumber, 1},
  { "C_getRandomNumbers", (DL_FUNC) &C_getRandomNumbers, 2},
  { NULL, NULL, 0}
};

void registerCXorShiftRNG(DllInfo *info) {
  R_registerRoutines(info, NULL, CXorShiftRNG_callMethods, NULL, NULL);
}
