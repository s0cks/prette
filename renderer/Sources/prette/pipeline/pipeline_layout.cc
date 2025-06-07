#include "prette/pipeline/pipeline_layout.h"

#include <filesystem>
#include <fmt/format.h>
#include <re2/re2.h>
#include <string>
#include <vulkan/vulkan_core.h>

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/device.h"  // IWYU pragma: keep
#include "prette/gfx.h"
#include "prette/gfx_driver_event.h"
#include "prette/json.h"
#include "prette/pipeline/pipeline_layout_builder.h"
#include "prette/pipeline/pipeline_layout_json.h"
#include "prette/pipeline/pipeline_layout_manager.h"
#include "prette/render_pass/scene_renderer.h"
#include "prette/to_string.h"
#include "prette/vk.h"

namespace prt::vk {
PipelineLayout::PipelineLayout(const std::string name, const VkPipelineLayoutCreateInfo& create_info) :
  vk::NamedHandleTemplate<VkPipelineLayout>(name) {
  Driver::Get()->CreatePipelineLayout(&create_info, handle_ptr());
  GetPipelineLayoutManager()->Register(this);
}

PipelineLayout::~PipelineLayout() {
  const auto driver = Driver::Get();
  ASSERT(driver);
  driver->DestroyPipelineLayout(handle_ref());
  GetPipelineLayoutManager()->Deregister(this);
}

auto PipelineLayout::ToString() const -> std::string {
  ToStringHelper<PipelineLayout> helper{};
  helper.AddFieldRef("name", GetName());
  return helper;
}

auto PipelineLayout::New(const std::string name, const VkPipelineLayoutCreateInfo& create_info) -> PipelineLayout* {
  ASSERT(!name.empty());
  return new PipelineLayout(name, create_info);
}

static inline auto IsValidPipelineLayoutManifest(const fs::path path) -> bool {
  return fs::exists(path) && fs::is_regular_file(path);
}

auto PipelineLayout::FromJsonFile(const fs::path& rhs) -> PipelineLayout* {
  DVLOG(2) << "loading PipelineLayout from: " << rhs << "....";
  ASSERT(fs::exists(rhs) && fs::is_regular_file(rhs));
  using namespace prt::json;
  PipelineLayoutBuilder builder{};
  DocumentHandler<PipelineLayoutHandler> handler(&builder);
  ParseJsonDocumentFrom(rhs, handler);
  ASSERT(handler.meta().IsClosedDoc());
  builder.WithName(handler.meta().GetName());
  return builder.Build();
}
}  // namespace prt::vk