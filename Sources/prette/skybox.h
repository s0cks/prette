#ifndef PRT_SKYBOX_H
#define PRT_SKYBOX_H

#include <vulkan/vulkan_core.h>

#include "prette/event.h"
#include "prette/gfx.h"
#include "prette/texture.h"

namespace prt {
#define FOR_EACH_SKYBOX_EVENT(V) \
  V(SkyboxInit)                  \
  V(SkyboxRendered)              \
  V(SkyboxDestroyed)

class SkyboxEvent;
#define FORWARD_DECLARE(Name) class Name##Event;
FOR_EACH_SKYBOX_EVENT(FORWARD_DECLARE)
#undef FORWARD_DECLARE

DEFINE_EVENT_PROTOTYPE(Skybox, FOR_EACH_SKYBOX_EVENT);

class SkyboxInitEvent : public SkyboxEvent {
 public:
  SkyboxInitEvent() = default;
  ~SkyboxInitEvent() override = default;
  DECLARE_EVENT_TYPE(SkyboxEvent, SkyboxInit);
};

class SkyboxRenderedEvent : public SkyboxEvent {
 public:
  SkyboxRenderedEvent() = default;
  ~SkyboxRenderedEvent() override = default;
  DECLARE_EVENT_TYPE(SkyboxEvent, SkyboxRendered);
};

class SkyboxDestroyedEvent : public SkyboxEvent {
 public:
  SkyboxDestroyedEvent() = default;
  ~SkyboxDestroyedEvent() override = default;
  DECLARE_EVENT_TYPE(SkyboxEvent, SkyboxDestroyed);
};

DEFINE_EVENT_SUBJECT(Skybox);
DEFINE_EVENT_OBSERVABLE(Skybox);
FOR_EACH_SKYBOX_EVENT(DEFINE_EVENT_OBSERVABLE);

auto OnSkyboxEvent() -> SkyboxEventObservable;
#define DEFINE_ON_EVENT(Name)                                                  \
  static inline auto On##Name##Event()->Name##EventObservable {                \
    return OnSkyboxEvent().filter(Name##Event::Filter).map(Name##Event::Cast); \
  }
FOR_EACH_SKYBOX_EVENT(DEFINE_ON_EVENT);
#undef DEFINE_ON_EVENT

struct SkyboxData {
  alignas(16) glm::mat4 view{};
};

class SkyboxMesh {
 private:
  vk::Buffer* vertices_;
  uint64_t num_vertices_;
  vk::Buffer* indices_;
  uint64_t num_indices_;

 public:
  SkyboxMesh();
  ~SkyboxMesh();

  auto GetVertexBuffer() const -> vk::Buffer* {
    return vertices_;
  }

  auto GetNumberOfVertices() const -> uint64_t {
    return num_vertices_;
  }

  auto GetIndexBuffer() const -> vk::Buffer* {
    return indices_;
  }

  auto GetNumberOfIndices() const -> uint64_t {
    return num_indices_;
  }

  void Draw(VkCommandBuffer& buffer);
  void Destroy();
};

class Driver;
class SkyboxPipeline {
 private:
  VkPipeline pipeline_{};
  VkPipelineLayout layout_{};
  VkPipelineCache cache_{};

  VkDescriptorSetLayout dset_layout_{};
  VkDescriptorSet dset_{};

  static void InitDescriptorSet(const Driver* driver, VkDescriptorSetLayout& dset_layout, VkDescriptorSet& dset);
  static void InitLayout(const Driver* driver, VkPipelineLayout& layout, const std::vector<VkDescriptorSetLayout>& dset_layouts);

 public:
  SkyboxPipeline();
  ~SkyboxPipeline();

  auto Get() const -> VkPipeline const& {
    return pipeline_;
  }

  auto GetLayout() const -> VkPipelineLayout const& {
    return layout_;
  }

  auto GetCache() const -> VkPipelineCache const& {
    return cache_;
  }

  void Draw(const VkCommandBuffer& buffer);
};

class Skybox {
 public:
  static void Init();
  static void Update();
};
}  // namespace prt

#endif  // PRT_SKYBOX_H
