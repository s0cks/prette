#ifndef PRT_LINE_H
#define PRT_LINE_H

#include <vector>

#include "prette/descriptor_set.h"
#include "prette/glm.h"
#include "prette/pipeline.h"
#include "prette/platform.h"
#include "prette/vk.h"
#include "prette/vk_buffer.h"

namespace prt {
struct LineVertex {
  glm::vec2 pos;
  glm::vec3 color;
};

struct Line {
  glm::vec2 start;
  glm::vec2 end;
};

class LineRenderer {
 public:
  using IndexType = uint16_t;

 private:
  static auto CreateDescriptorSet() -> vk::DescriptorSet*;
  static auto CreateIndexBuffer(const uint64_t num_lines) -> vk::Buffer*;
  static auto CreateVertexBuffer(const uint64_t num_lines) -> vk::Buffer*;

 private:
  uint64_t max_lines_;
  vk::RenderPipeline* pipeline_;
  vk::Buffer* vertices_;
  vk::Buffer* indices_;
  vk::DescriptorSet* descriptors_;
  std::vector<Line> data_{};

  void UpdateDescriptorSet();

 public:
  LineRenderer(const VkRenderPass& pass, const uint64_t max_lines);
  ~LineRenderer();

  auto GetIndexBuffer() const -> vk::Buffer* {
    return indices_;
  }

  auto GetVertexBuffer() const -> vk::Buffer* {
    return vertices_;
  }

  auto GetNumberOfLines() const -> uint64_t {
    return data_.size();
  }

  void Render(VkCommandBuffer& buffer);
};
}  // namespace prt

#endif  // PRT_LINE_H
