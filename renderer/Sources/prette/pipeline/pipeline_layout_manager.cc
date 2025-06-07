#include "prette/pipeline/pipeline_layout_manager.h"

#include <string>
#include <vector>

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/pipeline/pipeline_layout.h"
#include "prette/pipeline/pipeline_layout_finalizer.h"
#include "prette/thread_local.h"

namespace prt::vk {
static ThreadLocal<PipelineLayoutManager> manager_{};

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

PipelineLayoutManager::~PipelineLayoutManager() = default;

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
}  // namespace prt::vk