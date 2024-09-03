#include <benchmark/benchmark.h>
#include "mcc/uri_parser.h"

namespace mcc {
  void BM_UriParser(benchmark::State& state) {
    static constexpr const char* kBenchmarkUri = "file://example.png";
    for(const auto& _ : state) {
      uri::Parser parser(kBenchmarkUri);
      LOG_IF(FATAL, !parser.Parse()) << "failed to parse: " << kBenchmarkUri;
    }
  }

  BENCHMARK(BM_UriParser);
}