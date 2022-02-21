#ifndef C_MODELS_H
#define C_MODELS_H

#include <stdint.h>
#include <R.h>
#include <Rdefines.h>
#include <R_ext/Rdynload.h>

#define SMALLEST_DOUBLE           1e-15
#define MAX_ITERATIONS          1000000

#define CSpeciesTable_TAG       1
#define MAX_NUM_SPECIES         10000

#define SPECIES_GROWTH_RATE         0
#define SPECIES_CARRYING_CAPACITY   1
#define SPECIES_TIME_LAG            2
#define SPECIES_MORTALITY           3
#define SPECIES_HALF_SATURATION     4
#define SPECIES_REINTRODUCTION_PROB 5
#define SPECIES_REINTRODUCTION_SIZE 6
#define SPECIES_NUM_VALUES          7

#define PREDATOR  1
#define PREY      2

typedef struct CInteraction_STRUCT {
  size_t speciesIndex;
  double attackRate;
  double conversionRate;
  double deathRate;
  size_t timeLag;
} CInteraction;

typedef struct CSpecies_STRUCT {
  double growthRate;
  double carryingCapacity;
  size_t timeLag;
  double mortality;
  double halfSaturation;
  double reintroductionProb;
  double reintroductionSize;
  double predationFactor;

  size_t numPredators;
  CInteraction *predators;
  size_t numPrey;
  CInteraction *prey;
} CSpecies;

typedef union RNGStateItem_UNION {
  uint64_t s64;
  int      sInt[2];
} RNGStateItem;

typedef union RNGValue_UNION {
  uint64_t v64;
  double   vDbl;
} RNGValue;

typedef struct CSpeciesTable_STRUCT {
  int tag;
  //
  // The Random Number Generator (RNG) state
  // must be seeded so that it is not everywhere zero.
  //
  uint64_t rngState[16];
  int      rngP;
  //
  size_t numSpecies;
  CSpecies *species;
} CSpeciesTable;

extern SEXP L_returnMessage(const char *message);
extern int  L_isSpeciesTable(SEXP cSpeciesTable); // used by finalizer -- need to be kept simple
extern void L_assertSpeciesTable(const char* objName, SEXP cSpeciesTable);
extern void L_assertNotNull(const char* objName, void* aPointer);
extern void L_assertAnIntegerInRange(const char* objName, SEXP anInt, int min, int max);
extern void L_assertADoubleInRange(const char* objName, SEXP aDouble, double min, double max);
extern void L_assertIntegerVector(const char* objName, SEXP aVector, size_t vectorSize);
extern void L_assertDoubleVector(const char* objName, SEXP aVector, size_t vectorSize);

extern double L_getARandomNumber(CSpeciesTable *cSpeciesTable);
#endif