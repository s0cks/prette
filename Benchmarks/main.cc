#include <glog/logging.h>
#include <gflags/gflags.h>
#include <benchmark/benchmark.h>

auto main(int argc, char** argv) -> int {
  ::google::ParseCommandLineFlags(&argc, &argv, true);
  ::google::InitGoogleLogging(argv[0]);
  ::google::LogToStderr();
  ::benchmark::Initialize(&argc, argv);
  ::benchmark::RunSpecifiedBenchmarks();
  ::benchmark::Shutdown();
  return EXIT_SUCCESS;
}