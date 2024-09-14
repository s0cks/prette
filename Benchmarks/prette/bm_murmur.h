#ifndef PRT_BENCHMARK_MURMUR_H
#define PRT_BENCHMARK_MURMUR_H

#include <benchmark/benchmark.h>
#include "prette/hash/murmur3.h"

namespace prt {
  void BM_Murmur(benchmark::State& state);
}

#endif // PRT_BENCHMARK_MURMUR_H