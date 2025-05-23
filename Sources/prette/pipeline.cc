#include "prette/pipeline.h"

#include <filesystem>
#include <fmt/format.h>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "prette/common.h"
#include "prette/gfx.h"
#include "prette/json.h"
#include "prette/pipeline_builder.h"
#include "prette/pipeline_cache.h"
#include "prette/pipeline_json.h"
#include "prette/to_string.h"
#include "prette/vk.h"

namespace prt::vk {
static std::vector<RenderPipeline*> all_{};

static inline void Register(RenderPipeline* rhs) {
  ASSERT(rhs);
  all_.push_back(rhs);
}

static inline void Deregister(RenderPipeline* rhs) {
  ASSERT(rhs);
}

auto VisitAllRenderPipelines(RenderPipelineVisitor* vis) -> bool {
  ASSERT(vis);
  for (const auto& value : all_) {
    if (!vis->Visit(value))
      return false;
  }
  return true;
}

auto VisitAllRenderPipelines(RenderPipelinePredicate vis) -> bool {
  ASSERT(vis);
  for (const auto& value : all_) {
    if (!vis(value))
      return false;
  }
  return true;
}

auto GetNumberOfRenderPipelines() -> uint64_t {
  return all_.size();
}

static inline auto IsValidPipelineManifest(const fs::path path) -> bool {
  return fs::exists(path) && fs::is_regular_file(path);
}

auto ResolvePipelineManifest(std::string name) -> std::optional<fs::path> {
  if (!EndsWith(name, ".json"))
    name = fmt::format("{0:s}.json", name);
  EnvironmentVariable env_path("PRT_PATH");
  std::vector<std::string> paths{};
  env_path >> paths;
  for (const auto& path : paths) {
    fs::path file = fmt::format("{}/pipelines/{}", path, name);
    if (!IsValidPipelineManifest(file))
      continue;
    return {file};
  }
  return std::nullopt;
}

RenderPipeline::RenderPipeline(std::string name, const VkExtent2D& extent,
                               const VkGraphicsPipelineCreateInfo& create_info, vk::PipelineLayout* layout,
                               vk::PipelineCache* cache) :
  vk::NamedHandleTemplate<VkPipeline>(std::move(name)),
  extent_(extent),
  layout_(std::move(layout)),
  cache_(cache) {
  ASSERT_INITIALIZED(layout_);
  const auto driver = Driver::Get();
  driver->CreateGraphicsPipeline(create_info, vk::IsInitialized(cache) ? (*cache) : VK_NULL_HANDLE, handle_ptr());
  ASSERT_INITIALIZED(this);
  Register(this);
}

RenderPipeline::~RenderPipeline() {
  const auto driver = Driver::Get();
  driver->DestroyPipeline(handle_ref());
  Deregister(this);
}

auto RenderPipeline::ToString() const -> std::string {
  ToStringHelper<RenderPipeline> helper{};
  helper.AddFieldRef("name", GetName());
  return helper;
}

void RenderPipeline::Bind(VkCommandBuffer* buffer, const VkPipelineBindPoint bind_point) {
  vkCmdBindPipeline(*buffer, bind_point, GetHandle());
}

auto RenderPipeline::FromJson(const std::string name) -> RenderPipeline* {
  DVLOG(1) << "loading pipeline " << name << "....";
  const auto manifest_file = ResolvePipelineManifest(name);
  if (!manifest_file) {
    LOG(ERROR) << "failed to find manifest file for pipeline: " << name;
    return nullptr;
  }

  using namespace prt::json;
  RenderPipelineBuilder builder{};
  DocumentHandler<PipelineHandler> handler(&builder);
  ParseJsonDocumentFrom((*manifest_file), handler);
  vk::PipelineCacheBuilder cache_builder{};
  cache_builder.WithName(handler.meta().GetName());
  builder.SetPipelineCache(cache_builder.Build());
  return builder.Build();
}

auto RenderPipelineFinalizer::Visit(RenderPipeline* rhs) -> bool {
  ASSERT(rhs);
  DVLOG(2) << "finalizing: " << rhs->ToString();
  delete rhs;
  num_finalized_++;
  return true;
}

void RenderPipelineFinalizer::FinalizeAll() {
  std::vector<RenderPipeline*> all(all_);
  return FinalizeAll(all);
}

void RenderPipelineFinalizer::FinalizeAll(const std::vector<RenderPipeline*>& rhs) {
  DVLOG(1) << "finalizing RenderPipelines....";
  RenderPipelineFinalizer finalizer{};
  for (const auto& value : rhs) {
    LOG_IF(FATAL, !finalizer(value)) << "failed to finalize: " << value->ToString();
  }
  DVLOG(1) << "finalized " << finalizer.GetNumberOfObjectsFinalized() << " RenderPipelines";
}
}  // namespace prt::vk