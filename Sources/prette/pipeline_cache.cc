#include "prette/pipeline_cache.h"

#include <algorithm>
#include <functional>
#include <string>
#include <utility>
#include <vector>

#include "prette/common.h"
#include "prette/gfx.h"
#include "prette/gfx_vk.h"
#include "prette/to_string.h"
#include "prette/vk.h"

namespace prt::vk {
static std::vector<PipelineCache*> all_{};

PipelineCacheBuilder::PipelineCacheBuilder() {
  info_ptr()->sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
}

auto PipelineCacheBuilder::Build() -> PipelineCache* {
  return PipelineCache::New(GetName(), info_ptr());
}

static inline void Register(PipelineCache* rhs) {
  ASSERT(rhs);
  all_.push_back(rhs);
}

static inline void Deregister(PipelineCache* rhs) {
  ASSERT(rhs);
}

PipelineCache::PipelineCache(const std::string name, const VkPipelineCacheCreateInfo* create_info) :
  NamedHandleTemplate<VkPipelineCache>(std::move(name)) {
  const auto driver = Driver::Get();
  driver->CreatePipelineCache(create_info, handle_ptr());
  Register(this);
}

PipelineCache::~PipelineCache() {
  const auto driver = Driver::Get();
  driver->DestroyPipelineCache(handle_ref());
  Deregister(this);
}

auto PipelineCache::ToString() const -> std::string {
  return ToStringHelper<PipelineCache>{};
}

auto PipelineCacheFinalizer::Visit(PipelineCache* rhs) -> bool {
  ASSERT(rhs);
  DVLOG(1) << "finalizing " << rhs->ToString() << "....";
  delete rhs;
  num_finalized_++;
  return true;
}

void PipelineCacheFinalizer::FinalizeAll() {
  std::vector<PipelineCache*> all(std::begin(all_), std::end(all_));
  return FinalizeAll(all);
}

void PipelineCacheFinalizer::FinalizeAll(const std::vector<PipelineCache*>& all) {
  DVLOG(1) << "finalizing PipelineCaches....";
  PipelineCacheFinalizer finalizer{};
  for (const auto& cache : all_) {
    LOG_IF(FATAL, !finalizer.Visit(cache)) << "failed to finalize: " << cache->ToString();
  }
  DVLOG(1) << "finalized " << finalizer.GetNumberOfObjectsFinalized() << " PipelineCaches";
}

auto FindPipelineCache(const char* name) -> PipelineCache* {
  const auto value = std::ranges::find_if(all_, PipelineCache::FilterByName(name));
  return value != std::end(all_) ? (*value) : nullptr;
}

auto VisitAllPipelineCaches(PipelineCacheVisitor* vis) -> bool {
  ASSERT(vis);
  for (const auto& cache : all_) {
    if (!vis->Visit(cache))
      return false;
  }
  return true;
}

auto VisitAllPipelineCaches(std::function<bool(PipelineCache*)> vis) -> bool {
  ASSERT(vis);
  for (const auto& cache : all_) {
    if (!vis(cache))
      return false;
  }
  return true;
}

auto GetTotalNumberOfPipelineCaches() -> uint64_t {
  return all_.size();
}
}  // namespace prt::vk