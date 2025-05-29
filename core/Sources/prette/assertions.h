#ifndef PRT_ASSERTIONS_H
#define PRT_ASSERTIONS_H

#ifdef PRT_DEBUG

#include <cassert>

#define ASSERT(x) assert((x));

#else

#define ASSERT(x)

#endif  // PRT_DEBUG

#define ASSERT_INITIALIZED(x)   ASSERT(x && x->IsInitialized())
#define ASSERT_UNINITIALIZED(x) ASSERT(x == nullptr || !x->IsInitialized())
#define ASSERT_VALID(x)         ASSERT(x.IsValid())
#define ASSERT_NOT_EMPTY(x)     ASSERT(!(x).empty())
#define ASSERT_NULL(x)          ASSERT(x == nullptr)
#define ASSERT_NOT_NULL(x)      ASSERT(x != nullptr)
#define ASSERT_EMPTY(x)         ASSERT((x).empty())
#define ASSERT_GT(a, b)         ASSERT(a > b)
#define ASSERT_GTE(a, b)        ASSERT(a >= b)
#define ASSERT_LT(a, b)         ASSERT(a < b)
#define ASSERT_LTE(a, b)        ASSERT(a <= b)

#endif  // PRT_ASSERTIONS_H
