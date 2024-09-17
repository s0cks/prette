#include "prette/pipeline.h"
#include "prette/pipeline_executor.h"

namespace prt {
  auto Pipeline::Execute() -> bool {
    return PipelineExecutor::ExecutePipeline(this);
  }
}