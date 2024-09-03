#include <glog/logging.h>
#include <benchmark/benchmark.h>
#include "mcc/flags.h"

int main(int argc, char** argv) {
  ::google::ParseCommandLineFlags(&argc, &argv, true);
  ::google::InitGoogleLogging(argv[0]);
  ::google::LogToStderr();
  ::benchmark::Initialize(&argc, argv);
  ::benchmark::RunSpecifiedBenchmarks();
  ::benchmark::Shutdown();
  return EXIT_SUCCESS;
}