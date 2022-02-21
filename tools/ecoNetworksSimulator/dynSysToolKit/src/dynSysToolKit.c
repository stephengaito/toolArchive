// An R-language Dynamical Systems Tool Kit

#include <R.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>

extern void registerCIntegrateLorenz(DllInfo *info);

static R_CallMethodDef dynSysToolKit_callMethods[] = {
  { NULL, NULL, 0}
};

void R_init_dynSysToolKit(DllInfo *info) {
  registerCIntegrateLorenz(info);
  R_registerRoutines(info, NULL, dynSysToolKit_callMethods, NULL, NULL);
}