#include "prette/pipeline_executor.h"

#include <units.h>
#include "prette/uv/utils.h"

namespace prt::pipeline {
  auto PipelineExecutor::GetPipelineName() const -> const char* {
    return GetPipeline()->GetName();
  }

  auto PipelineExecutor::Visit(Pipeline* pipeline) -> bool {
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

  auto PipelineExecutor::Execute() -> bool {
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