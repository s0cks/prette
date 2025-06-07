#ifndef PRT_PIPELINE_LAYOUT_H
#define PRT_PIPELINE_LAYOUT_H

#include <functional>
#include <string>
#include <vulkan/vulkan_core.h>

#include "prette/common.h"
#include "prette/platform.h"
#include "prette/vk.h"

namespace prt {
class TerminatedState;
namespace vk {
class PipelineLayout;
class PipelineLayoutVisitor {
 protected:
  PipelineLayoutVisitor() = default;

 public:
  virtual ~PipelineLayoutVisitor() = default;
  virtual auto Visit(PipelineLayout* rhs) -> bool = 0;
};

using PipelineLayoutPredicate = std::function<bool(PipelineLayout*)>;

class PipelineLayout : public NamedHandleTemplate<VkPipelineLayout> {
 public:
  struct Hash : public prt::Hasher {
    auto operator()(prt::vk::PipelineLayout* lhs) const -> size_t {
      size_t hash = prt::kInvalidHash;
      Combine(hash, lhs->GetName());
      return hash;
    }
  };

  struct Eq {
    auto operator()(PipelineLayout* lhs, PipelineLayout* rhs) const -> bool {
      return lhs->GetName() == rhs->GetName();
    }
  };

 public:
  PipelineLayout(const std::string name, const VkPipelineLayoutCreateInfo& create_info);
  ~PipelineLayout() override;

  auto ToString() const -> std::string override;

  operator VkPipelineLayout() const {
    return GetHandle();
  }

 public:
  static auto New(const std::string name, const VkPipelineLayoutCreateInfo&) -> PipelineLayout*;
  static auto FromJsonFile(const fs::path& rhs) -> PipelineLayout*;
};
}  // namespace vk
}  // namespace prt

#endif  // PRT_PIPELINE_LAYOUT_H
