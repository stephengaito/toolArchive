// An R-language Ecological Networks Simulator

#include <R.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>

extern void registerCModelBuilder(DllInfo *info);
extern void registerCIntegrateEuler(DllInfo *info);

static R_CallMethodDef ecoNetworksSimulator_callMethods[] = {
  { NULL, NULL, 0}
};

void R_init_ecoNetworksSimulator(DllInfo *info) {
  registerCModelBuilder(info);
  registerCIntegrateEuler(info);
  R_registerRoutines(info, NULL, ecoNetworksSimulator_callMethods, NULL, NULL);
};
