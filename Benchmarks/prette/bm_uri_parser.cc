#include <benchmark/benchmark.h>
#include "prette/uri_parser.h"

namespace prt {
  void BM_UriParser(benchmark::State& state) {
    static constexpr const char* kBenchmarkUri = "file://example.png";
    for(const auto& _ : state) {
      uri::Parser parser(kBenchmarkUri);
      LOG_IF(FATAL, !parser.Parse()) << "failed to parse: " << kBenchmarkUri;
    }
    state.SetBytesProcessed(state.iterations() * static_cast<int64_t>(strlen(kBenchmarkUri)));
  }

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
  BENCHMARK(BM_UriParser)
    ->Name("Parse")
    ->Unit(benchmark::TimeUnit::kNanosecond)
    ->ThreadPerCpu();
}