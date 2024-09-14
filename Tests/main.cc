#include <gtest/gtest.h>
#include <glog/logging.h>

#include "prette/class.h"
#include "prette/prette.h"

auto main(int argc, char** argv) -> int {
  using namespace google;
  using namespace prt;
  using namespace ::testing;

  InitGoogleLogging(argv[0]);
  InitGoogleTest(&argc, argv);
  ParseCommandLineFlags(&argc, &argv, false);
  LOG(INFO) << "Running unit tests for prette v" << GetVersion() << "....";

  Class::Init();
  return RUN_ALL_TESTS();
}