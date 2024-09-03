#include "prette/pipeline_executor.h"

#include <units.h>
#include "prette/uv/utils.h"

namespace prt::pipeline {
  const char* PipelineExecutor::GetPipelineName() const {
    return GetPipeline()->GetName();
  }

  bool PipelineExecutor::Visit(Pipeline* pipeline) {
    PRT_ASSERT(pipeline);
    if(!pipeline->Apply())
      return false; // abort
    if(!pipeline->HasChildren())
      return true; // continue;
    for(auto idx = 0; idx < pipeline->GetNumberOfChildren(); idx++) {
      const auto child = pipeline->GetChildAt(idx);
      PRT_ASSERT(child);
      if(!child->Accept(this))
        return false; // abort
    }
    return true; // continue
  }

  bool PipelineExecutor::Execute() {
    using namespace units::time;
    LOG(INFO) << "executing " << GetPipelineName() << "....";
    const auto start_ns = uv::Now();
    const auto result = GetPipeline()->Accept(this);
    const auto stop_ns = uv::Now();
    const auto total_ns = (stop_ns - start_ns);
    DLOG(INFO) << GetPipelineName() << " finished (" << nanosecond_t(total_ns) << ").";
    return result;
  }
}