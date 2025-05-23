#include "prette/pipeline_layout.h"

#include <filesystem>
#include <fmt/format.h>
#include <functional>
#include <re2/re2.h>
#include <string>
#include <unordered_set>
#include <vector>

#include "prette/common.h"
#include "prette/descriptor_set.h"
#include "prette/device.h"
#include "prette/gfx.h"
#include "prette/gfx_driver_event.h"
#include "prette/json.h"
#include "prette/thread_local.h"
#include "prette/to_string.h"
#include "prette/vk.h"

namespace prt {
namespace vk {
static ThreadLocal<PipelineLayoutManager> manager_{};

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

PipelineLayoutManager::PipelineLayoutManager() {
  EnvironmentVariable env_path("PRT_PATH");
  std::vector<std::string> roots{};
  env_path >> roots;
  for (const fs::path& root : roots) {
    if (!fs::exists(root) || !fs::is_directory(root))
      continue;
    loaders_.emplace_back(root / "pipelines", fs::IsJsonFile, PipelineLayout::FromJsonFile);
  }
}

PipelineLayoutManager::~PipelineLayoutManager() {
  // do nothing
}

void PipelineLayoutManager::Register(PipelineLayout* rhs) {
  ASSERT(rhs);
  const auto [_, success] = all_.insert(rhs);
  LOG_IF(FATAL, !success) << "failed to register: " << rhs->ToString();
}

void PipelineLayoutManager::Deregister(PipelineLayout* rhs) {
  ASSERT(rhs);
  const auto num_erased = all_.erase(rhs);
  LOG_IF(FATAL, num_erased != 1) << "failed to deregister: " << rhs->ToString();
}

auto PipelineLayoutManager::FindPipelineLayout(const std::string name) const -> PipelineLayout* {
  ASSERT(!name.empty());
  for (const auto& playout : all_) {
    ASSERT(playout);
    if (EqualsIgnoreCase(playout->GetName(), name))
      return playout;
  }
  return nullptr;
}

auto PipelineLayoutManager::VisitAllPipelineLayouts(PipelineLayoutVisitor* vis) const -> bool {
  ASSERT(vis);
  for (const auto& playout : all_) {
    ASSERT(playout);
    if (!vis->Visit(playout))
      return false;
  }
  return true;
}

auto PipelineLayoutManager::LoadPipelineLayout(std::string name) const -> PipelineLayout* {
  ASSERT(!name.empty());
  if (loaders_.empty()) {
    DLOG(ERROR) << "no PipelineLayoutLoaders found.";
    return nullptr;
  }

  if (!EndsWith(name, ".json"))
    name = name + ".json";
  for (const auto& loader : loaders_) {
    const auto value = loader.Load(name);
    if (value) {
      DLOG(INFO) << value->ToString() << " loaded!";
      return value;
    }
  }
  LOG(ERROR) << "failed to load " << name << " PipelineLayout.";
  return nullptr;
}

auto PipelineLayoutManager::FindOrLoadPipelineLayout(const std::string name) -> PipelineLayout* {
  ASSERT(!name.empty());
  PipelineLayout* layout = nullptr;
  if ((layout = FindPipelineLayout(name)))
    return layout;
  if ((layout = LoadPipelineLayout(name + ".layout")))
    return layout;
  LOG(ERROR) << "failed to find or load PipelineLayout: " << name;
  return nullptr;
}

auto PipelineLayoutManager::VisitAllPipelineLayouts(PipelineLayoutPredicate filter) const -> bool {
  for (const auto& playout : all_) {
    ASSERT(playout);
    if (!filter(playout))
      return false;
  }
  return true;
}

void PipelineLayoutManager::FinalizeAll() {
  std::vector<PipelineLayout*> work(std::begin(all_), std::end(all_));
  return PipelineLayoutFinalizer::FinalizeAll(work);
}

auto PipelineLayoutBuilder::AddPushConstantRange(const uint32_t offset, const uint32_t size,
                                                 const VkShaderStageFlags stage) -> VkPushConstantRange* {
  const auto idx = push_ranges_.size();
  push_ranges_.resize(idx + 1);
  auto& range = push_ranges_[idx];
  range.offset = offset;
  range.size = size;
  range.stageFlags = stage;
  return &range;
}

auto PipelineLayoutBuilder::Build() -> PipelineLayout* {
  ASSERT(!name_.empty());
  info_.setLayoutCount = descriptor_set_layouts_.size();
  info_.pSetLayouts = descriptor_set_layouts_.data();
  info_.pushConstantRangeCount = 0;
  info_.pPushConstantRanges = nullptr;
  return PipelineLayout::New(name_, info_);
}

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

void InitPipelineLayoutManager() {
  ASSERT(!IsPipelineLayoutManagerInitialized());
  manager_ = new PipelineLayoutManager();
  ASSERT(IsPipelineLayoutManagerInitialized());
}

auto IsPipelineLayoutManagerInitialized() -> bool {
  return manager_.Get() != nullptr;
}

auto GetPipelineLayoutManager() -> PipelineLayoutManager* {
  return manager_.Get();
}
}  // namespace vk

namespace json {
PipelineLayoutHandler::PipelineLayoutHandler(vk::PipelineLayoutBuilder* builder) :
  BaseStatefulReaderHandler<PipelineLayoutHandlerState, PipelineLayoutHandler>(PipelineLayoutHandlerState::kEmpty),
  builder_(builder) {}

auto PipelineLayoutHandler::Key(const char* str, SizeType length, bool copy) -> bool {
  ASSERT(IsOpenDoc());
  std::string key(str, length);
  if (EqualsIgnoreCase(key, "descriptor_sets"))
    return TransitionTo(PipelineLayoutHandlerState::kParsingDescriptorSets);
  return InvalidState();
}

auto PipelineLayoutHandler::String(const char* str, SizeType length, bool copy) -> bool {
  std::string value(str, length);
  if (IsEmpty()) {
    const auto layout = vk::GetPipelineLayoutManager()->FindOrLoadPipelineLayout(value);
    LOG_IF(FATAL, !vk::IsInitialized(layout)) << "failed to find valid pipeline layout: " << value;
    SetResult(layout);
    return TransitionTo(PipelineLayoutHandlerState::kClosedDoc);
  } else if (IsParsingDescriptorSets()) {
    const auto dset = vk::FindDescriptorSet(value);
    LOG_IF(FATAL, !vk::IsInitialized(dset)) << "failed to find descriptor set: " << value;
    builder_->WithDescriptorSetLayout(&dset->GetLayout());
    return Continue();
  }
  return InvalidState();
}

auto PipelineLayoutHandler::StartObject() -> bool {
  switch (GetState()) {
    case PipelineLayoutHandlerState::kEmpty:
      return TransitionTo(PipelineLayoutHandlerState::kOpenDoc);
    default:
      break;
  }
  return InvalidState();
}

auto PipelineLayoutHandler::EndObject(SizeType memberCount) -> bool {
  switch (GetState()) {
    case PipelineLayoutHandlerState::kOpenDoc:
      return TransitionTo(PipelineLayoutHandlerState::kClosedDoc);
    default:
      break;
  }
  return InvalidState();
}

auto PipelineLayoutHandler::StartArray() -> bool {
  switch (GetState()) {
    case PipelineLayoutHandlerState::kParsingDescriptorSets:
      return Continue();
    default:
      break;
  }
  return InvalidState();
}

auto PipelineLayoutHandler::EndArray(SizeType elementCount) -> bool {
  switch (GetState()) {
    case PipelineLayoutHandlerState::kParsingDescriptorSets:
      return TransitionTo(PipelineLayoutHandlerState::kOpenDoc);
    default:
      break;
  }
  return InvalidState();
}

auto PipelineLayoutHandler::Null() -> bool {
  return InvalidState();
}

auto PipelineLayoutHandler::Bool(bool b) -> bool {
  return InvalidState();
}

auto PipelineLayoutHandler::Int(int i) -> bool {
  return InvalidState();
}

auto PipelineLayoutHandler::Uint(unsigned u) -> bool {
  return InvalidState();
}

auto PipelineLayoutHandler::Int64(int64_t i) -> bool {
  return InvalidState();
}

auto PipelineLayoutHandler::Uint64(uint64_t u) -> bool {
  return InvalidState();
}

auto PipelineLayoutHandler::Double(double d) -> bool {
  return InvalidState();
}
}  // namespace json
}  // namespace prt