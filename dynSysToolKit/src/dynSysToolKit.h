#ifndef DYN_SYS_TOOL_KIT_H
#define DYN_SYS_TOOL_KIT_H

#include <R.h>
#include <Rinternals.h>
#include <Rdefines.h>
#include <R_ext/Rdynload.h>

#ifdef  DEBUG_OUTPUT
#define DEBUG(...)  Rprintf(__VA_ARGS__)
#else
#define DEBUG(...)
#endif

#define SMALLEST_DOUBLE 1e-15
#define MAX_ITERATIONS  1000000

extern SEXP    L_returnMessage(const char *message);
extern void    L_assertNotNull(const char* objName, void* aPointer);
extern int     L_assertAnIntegerInRange(const char* objName, SEXP anInt, int min, int max);
extern double  L_assertADoubleInRange(const char* objName, SEXP aDouble, double min, double max);
extern int*    L_assertIntegerVector(const char* objName, SEXP aVector, size_t vectorSize);
extern double* L_assertDoubleVector(const char* objName, SEXP aVector, size_t vectorSize);


#endif