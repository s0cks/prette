#ifndef PRT_PIPELINE_LAYOUT_FINALIZER_H
#define PRT_PIPELINE_LAYOUT_FINALIZER_H

#include <cstdint>
#include <vector>

#include "prette/pipeline/pipeline_layout.h"

namespace prt::vk {
class PipelineLayoutFinalizer : public PipelineLayoutVisitor {
 private:
  uint64_t num_finalized_ = 0;

 public:
  PipelineLayoutFinalizer() = default;
  ~PipelineLayoutFinalizer() override = default;
  auto Visit(PipelineLayout* rhs) -> bool override;

  auto GetNumberOfFinalized() const -> uint64_t {
    return num_finalized_;
  }

 public:
  static void FinalizeAll(const std::vector<PipelineLayout*>& layouts);
};
}  // namespace prt::vk

#endif  // PRT_PIPELINE_LAYOUT_FINALIZER_H
