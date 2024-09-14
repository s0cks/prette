#include <benchmark/benchmark.h>
#include "prette/uri_parser.h"

namespace prt {
  void BM_UriParser(benchmark::State& state) {
    static constexpr const char* kBenchmarkUri = "file://example.png";
    for(const auto& _ : state) {
      uri::Parser parser(kBenchmarkUri);
      LOG_IF(FATAL, !parser.Parse()) << "failed to parse: " << kBenchmarkUri;
    }
  }

  BENCHMARK(BM_UriParser); //NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
}