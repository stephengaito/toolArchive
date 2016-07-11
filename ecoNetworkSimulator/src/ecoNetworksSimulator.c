// An R-language Ecological Networks Simulator

#include <R.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>

SEXP C_simulateModel(SEXP x) {
  Rprintf("Hello, world!\n");
  return x;
}

static R_CallMethodDef callMethods[] = {
  { "C_simulateModel", (DL_FUNC) &C_simulateModel,  1},
  { NULL, NULL, 0}
};

void R_init_ecoNetworksSimulator(DllInfo *info) {
  R_registerRoutines(info, NULL, callMethods, NULL, NULL);
};
