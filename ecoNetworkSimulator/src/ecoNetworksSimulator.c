// An R-language Ecological Networks Simulator

#include <R.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>

SEXP C_createModel(SEXP x) {
  Rprintf("Created model\n");
  return x;
}

SEXP C_listModels() {
  Rprintf("listing models\n");
  return allocVector(STRSXP, 1);
}

SEXP C_simulateModel(SEXP x) {
  Rprintf("Simulating model\n");
  return x;
}

static R_CallMethodDef callMethods[] = {
  { "C_createModel",   (DL_FUNC) &C_createModel,    1},
  { "C_listModels",    (DL_FUNC) &C_listModels,     0},
  { "C_simulateModel", (DL_FUNC) &C_simulateModel,  1},
  { NULL, NULL, 0}
};

void R_init_ecoNetworksSimulator(DllInfo *info) {
  R_registerRoutines(info, NULL, callMethods, NULL, NULL);
};
