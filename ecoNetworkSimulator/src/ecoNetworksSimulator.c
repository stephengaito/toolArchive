// An R-language Ecological Networks Simulator

#include <R.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>

SEXP simulate(SEXP x) {
  Rprintf("Hello, world!\n");
  return x;
}

static R_CallMethodDef callMethods[] = {
  { "C_simulate", (DL_FUNC) &simulate,  1},
  { NULL, NULL, 0}
};

void R_init_ecoNetworksSimulator(DllInfo *info) {
  printf("hello from R_init_ecoNetworksSimulator\n");
  R_registerRoutines(info, NULL, callMethods, NULL, NULL);
};
