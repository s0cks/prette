#include "prette/pipeline/pipeline_layout_finalizer.h"

#include <vector>

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/pipeline/pipeline_layout.h"

namespace prt::vk {
auto PipelineLayoutFinalizer::Visit(PipelineLayout* rhs) -> bool {
  ASSERT(rhs);
  DVLOG(1) << "finalizing " << rhs->ToString();
  delete rhs;
  num_finalized_ += 1;
  return true;
}

void PipelineLayoutFinalizer::FinalizeAll(const std::vector<PipelineLayout*>& layouts) {
  DVLOG(1) << "finalizing PipelineLayouts....";
  PipelineLayoutFinalizer finalizer{};
  auto current = std::begin(layouts);
  while (current != std::end(layouts)) {
    LOG_IF(FATAL, !finalizer.Visit(*current)) << "failed to finalize: " << (*current)->ToString();
    current++;
  }
  DVLOG(1) << "finalized " << finalizer.GetNumberOfFinalized() << " PipelineLayouts";
}
}  // namespace prt::vk