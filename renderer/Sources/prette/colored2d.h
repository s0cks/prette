#ifndef PRT_COLORED2D_H
#define PRT_COLORED2D_H

#include <string>
#include <utility>

#include "prette/color.h"
#include "prette/glm.h"
#include "prette/pipeline/pipeline.h"
#include "prette/platform.h"
#include "prette/vertex/vertex2d.h"
#include "prette/vk.h"

namespace prt {
class Colored2DPipeline : public vk::RenderPipelineTemplate<color2d::Vertex, 128, uint16_t, 128> {
 private:
  Color color_ = kWhite;
  uint64_t num_instances_ = 0;
  vk::DescriptorSet* descriptors_ = nullptr;

 public:
  Colored2DPipeline(std::string name, const VkExtent2D& extent, const VkGraphicsPipelineCreateInfo& create_info,
                    vk::PipelineLayout* layout, vk::PipelineCache* cache) :
    vk::RenderPipelineTemplate<color2d::Vertex, 128, uint16_t, 128>(name, extent, create_info, layout, cache) {
    // do nothing
  }
  ~Colored2DPipeline() override = default;

  auto GetDescriptors() const -> vk::DescriptorSet* {
    return descriptors_;
  }

  void SetColor(const Color& rhs) {
    color_ = rhs;
  }

  void PushRect(const glm::vec2 top_left, const glm::vec2 bottom_left, const glm::vec2 bottom_right,
                const glm::vec2 top_right);

  inline void PushRect(const glm::vec2 top_left, const glm::vec2 size) {
    const auto bottom_left = glm::vec2(top_left.x, top_left.y + size.y);
    const auto bottom_right = glm::vec2(top_left.x + size.x, top_left.y + size.y);
    const auto top_right = glm::vec2(top_left.x + size.x, top_left.y);
    return PushRect(std::move(top_left), std::move(bottom_left), std::move(bottom_right), std::move(top_right));
  }

  void Draw(VkCommandBuffer buffer);
};
}  // namespace prt

#endif  // PRT_COLORED2D_H
