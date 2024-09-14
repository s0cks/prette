#include "prette/bm_murmur.h"

namespace prt {
  void BM_Murmur(benchmark::State& state) {
    static constexpr const char* kExampleString = "This is an example string.";
    for(const auto& _ : state)
      murmur3_32::of(kExampleString);
  }

  BENCHMARK(BM_Murmur); // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
}