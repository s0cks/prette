#include "prette/pipeline.h"
#include "prette/pipeline_executor.h"

namespace prt {
  bool Pipeline::Execute() {
    return PipelineExecutor::ExecutePipeline(this);
  }
}