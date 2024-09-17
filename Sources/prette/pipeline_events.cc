#include "prette/pipeline_events.h"
#include <sstream>
#include "prette/pipeline.h"

namespace prt::pipeline {
  auto PipelineStartedEvent::ToString() const -> std::string {
    std::stringstream ss;
    ss << "PipelineStartedEvent(";
    ss << "pipeline=" << GetPipeline()->GetName();
    ss << ")";
    return ss.str();
  }

  auto PipelineFinishedEvent::ToString() const -> std::string {
    std::stringstream ss;
    ss << "PipelineFinishedEvent(";
    ss << "pipeline=" << GetPipeline()->GetName();
    ss << ")";
    return ss.str();
  }
}