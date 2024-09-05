#include <gtest/gtest.h>
#include <glog/logging.h>

#include "prette/class.h"
#include "prette/prette.h"

int main(int argc, char** argv) {
  using namespace google;
  using namespace prt;

  InitGoogleLogging(argv[0]);
  LogToStderr();
  ::testing::InitGoogleTest(&argc, argv);
  ParseCommandLineFlags(&argc, &argv, false);
  LOG(INFO) << "Running unit tests for minecraft-clone v" << GetVersion() << "....";

  Class::Init();
  return RUN_ALL_TESTS();
}