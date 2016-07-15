#ifndef C_MODELS_H
#define C_MODELS_H

#include <R.h>
#include <Rdefines.h>
#include <R_ext/Rdynload.h>

#define SMALLEST_DOUBLE           1e-15
#define MAX_ITERATIONS          1000000

#define CSpeciesTable_TAG       1
#define MAX_NUM_SPECIES         10000

#define SPECIES_GROWTH_RATE       0
#define SPECIES_CARRYING_CAPACITY 1
#define SPECIES_MORTALITY         2
#define SPECIES_HALF_SATURATION   3
#define SPECIES_NUM_VALUES        4

#define PREDATOR  1
#define PREY      2

typedef struct CInteraction_STRUCT {
  size_t speciesIndex;
  double attackRate;
  double conversionRate;
} CInteraction;

typedef struct CSpecies_STRUCT {
  double growthRate;
  double carryingCapacity;
  double mortality;
  double halfSaturation;
  double predationFactor;
  size_t numPredators;
  CInteraction *predators;
  size_t numPrey;
  CInteraction *prey;
} CSpecies;

typedef struct CSpeciesTable_STRUCT {
  int tag;
  size_t numSpecies;
  CSpecies *species;
} CSpeciesTable;

extern int L_isSpeciesTable(SEXP cSpeciesTable);
extern int L_isAnIntegerInRange(SEXP anInt, int min, int max);
extern int L_isADoubleInRange(SEXP aDouble, double min, double max);
extern int L_isIntegerVector(SEXP aVector, size_t vectorSize);
extern int L_isDoubleVector(SEXP aVector, size_t vectorSize);

#endif