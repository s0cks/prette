#ifndef PRT_ASSERTIONS_H
#define PRT_ASSERTIONS_H

#ifdef PRT_DEBUG

#include <cassert>

#define ASSERT(x)               assert((x));
#define ASSERT_INITIALIZED(x)   ASSERT(x && x->IsInitialized())
#define ASSERT_UNINITIALIZED(x) ASSERT(x == nullptr || !x->IsInitialized())
#define ASSERT_VALID(x)         ASSERT(x.IsValid())
#define ASSERT_NOT_EMPTY(x)     ASSERT(!(x).empty())
#define ASSERT_EMPTY(x)         ASSERT((x).empty())

#else

#define ASSERT(x)
#define ASSERT_INITIALIZED(x)
#define ASSERT_UNINITIALIZED(x)
#define ASSERT_VALID(x)
#define ASSERT_NOT_EMPTY(x)
#define ASSERT_EMPTY(x)

#endif  // PRT_DEBUG

#endif  // PRT_ASSERTIONS_H
