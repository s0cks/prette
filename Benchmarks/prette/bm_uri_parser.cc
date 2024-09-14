#include <benchmark/benchmark.h>
#include "prette/uri_parser.h"

namespace prt {
  void BM_UriParser(benchmark::State& state) {
    static constexpr const char* kBenchmarkUri = "file://example.png";
    uri::Parser parser;
    for(const auto& _ : state)
      LOG_IF(FATAL, !parser.Parse(kBenchmarkUri)) << "failed to parse: " << kBenchmarkUri;
    state.SetBytesProcessed(state.iterations() * static_cast<int64_t>(strlen(kBenchmarkUri)));
  }

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
  BENCHMARK(BM_UriParser)
    ->Name("UriParser::Parse")
    ->Unit(benchmark::TimeUnit::kNanosecond)
    ->Iterations(1000000)
    ->ThreadPerCpu();
}