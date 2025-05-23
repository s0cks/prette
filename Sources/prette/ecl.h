#ifndef PRT_ECL_H
#define PRT_ECL_H

// IWYU pragma: begin_exports
#include <ecl/ecl.h>
// IWYU pragma: end_exports

namespace prt {
void InitLisp(int argc, char** argv);
}

#undef Null

#endif  // PRT_ECL_H
