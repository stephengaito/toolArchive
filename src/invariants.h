#ifndef INVARIANTS_H
#define INVARIANTS_H

#ifdef PRODUCTION
#define ASSERT(cond)
#else
#include <assert.h>
#define ASSERT assert
#endif

#define ASSERT_INVARIANT	\
  ASSERT(invariant())

#define ASSERT_INVARIANT2	\
  ASSERT(invariant1());		\
  ASSERT(invariant2())

#define ASSERT_INVARIANT3	\
  ASSERT(invariant1());		\
  ASSERT(invariant2());		\
  ASSERT(invariant3())

#define ASSERT_INVARIANT4	\
  ASSERT(invariant1());		\
  ASSERT(invariant2());		\
  ASSERT(invariant3());		\
  ASSERT(invariant4())

#define ASSERT_INVARIANT5	\
  ASSERT(invariant1());		\
  ASSERT(invariant2());		\
  ASSERT(invariant3());		\
  ASSERT(invariant4());		\
  ASSERT(invariant5())

#endif
