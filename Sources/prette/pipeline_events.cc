#include "prette/pipeline_events.h"
#include <sstream>
#include "prette/pipeline.h"

namespace prt::pipeline {
  std::string PipelineStartedEvent::ToString() const {
    std::stringstream ss;
    ss << "PipelineStartedEvent(";
    ss << "pipeline=" << GetPipeline()->GetName();
    ss << ")";
    return ss.str();
  }

  std::string PipelineFinishedEvent::ToString() const {
    std::stringstream ss;
    ss << "PipelineFinishedEvent(";
    ss << "pipeline=" << GetPipeline()->GetName();
    ss << ")";
    return ss.str();
  }
}