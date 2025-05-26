#ifndef PRT_PIPELINE_CACHE_H
#define PRT_PIPELINE_CACHE_H

#include <cstdint>
#include <functional>
#include <string>
#include <utility>
#include <vector>

#include "prette/common.h"
#include "prette/vk.h"

namespace prt::vk {
class PipelineCache;
class PipelineCacheBuilder :
  public NamedHandleBuilderTemplate<VkPipelineCacheCreateInfo, PipelineCache, PipelineCacheBuilder> {
 public:
  PipelineCacheBuilder();
  ~PipelineCacheBuilder() override = default;

  auto WithNext(const void* rhs) -> PipelineCacheBuilder& {
    ASSERT(rhs);
    info_ptr()->pNext = rhs;
    return *this;
  }

  auto WithFlags(const VkPipelineCacheCreateFlags rhs) -> PipelineCacheBuilder& {
    info_ptr()->flags = rhs;
    return *this;
  }

  auto WithInitialData(const void* data, const uint64_t num_bytes) -> PipelineCacheBuilder& {
    ASSERT(data && num_bytes > 0);
    info_ptr()->pInitialData = data;
    info_ptr()->initialDataSize = num_bytes;
    return *this;
  }

  auto Build() -> PipelineCache* override;
};

class PipelineCacheVisitor {
 public:
  PipelineCacheVisitor() = default;
  virtual ~PipelineCacheVisitor() = default;
  virtual auto Visit(PipelineCache* rhs) -> bool = 0;
};

class PipelineCache : public NamedHandleTemplate<VkPipelineCache> {
  friend class PipelineCacheBuilder;

 public:
  using Filter = std::function<bool(PipelineCache*)>;

 public:
  PipelineCache(const std::string name, const VkPipelineCacheCreateInfo* create_info);
  ~PipelineCache() override;

  auto ToString() const -> std::string override;
  operator VkPipelineCache() const {
    return GetHandle();
  }

 private:
  static inline auto New(const std::string name, const VkPipelineCacheCreateInfo* create_info) -> PipelineCache* {
    ASSERT(!name.empty() && create_info != nullptr);
    return new PipelineCache(std::move(name), create_info);
  }

 public:
  static inline auto FilterByName(const char* name) -> Filter {
    return [name](PipelineCache* cache) {
      return cache && cache->IsNamed(name);
    };
  }
};

auto FindPipelineCache(const char* name) -> PipelineCache*;
auto VisitAllPipelineCaches(PipelineCacheVisitor* vis) -> bool;
auto VisitAllPipelineCaches(std::function<bool(PipelineCache*)> vis) -> bool;
auto GetTotalNumberOfPipelineCaches() -> uint64_t;

class PipelineCacheFinalizer : public PipelineCacheVisitor {
 private:
  uint64_t num_finalized_ = 0;

 public:
  PipelineCacheFinalizer() = default;
  ~PipelineCacheFinalizer() override = default;
  auto Visit(PipelineCache* rhs) -> bool override;

  auto GetNumberOfObjectsFinalized() const -> uint64_t {
    return num_finalized_;
  }

 public:
  static void FinalizeAll();
  static void FinalizeAll(const std::vector<PipelineCache*>& all);
};
}  // namespace prt::vk

#endif  // PRT_PIPELINE_CACHE_H
