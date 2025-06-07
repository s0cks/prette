#ifndef PRT_PIPELINE_LAYOUT_MANAGER_H
#define PRT_PIPELINE_LAYOUT_MANAGER_H

#include <functional>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include "prette/common.h"
#include "prette/pipeline/pipeline_layout.h"
#include "prette/rx.h"

namespace prt::vk {
template <class T>
class Loader {
  DEFINE_DEFAULT_COPYABLE_TYPE(Loader<T>);

 public:
  using Callback = std::function<T*(fs::path)>;

 private:
  fs::path root_;
  fs::path_predicate filter_;
  Callback callback_;

 public:
  explicit Loader(fs::path root, fs::path_predicate filter, Callback callback) :
    root_(std::move(root)),
    filter_(std::move(filter)),
    callback_(std::move(callback)) {}
  ~Loader() = default;

  auto GetRoot() const -> const fs::path& {
    return root_;
  }

  auto Load(const std::string filename) const -> T* {
    const auto file = GetRoot() / filename;
    if (!filter_(file))
      return nullptr;
    return callback_(file);
  }
};

using PipelineLayoutLoader = Loader<PipelineLayout>;
using PipelineLayoutSet = std::unordered_set<PipelineLayout*, PipelineLayout::Hash, PipelineLayout::Eq>;

class PipelineLayoutManager {
  friend class PipelineLayout;
  friend class prt::TerminatedState;

 private:
  PipelineLayoutSet all_{};
  std::vector<PipelineLayoutLoader> loaders_{};
  rx::subscription on_device_init_{};

  void FinalizeAll();
  void Register(PipelineLayout* rhs);
  void Deregister(PipelineLayout* rhs);
  auto LoadPipelineLayout(const std::string name) const -> PipelineLayout*;

 public:
  PipelineLayoutManager();
  ~PipelineLayoutManager();
  auto FindPipelineLayout(const std::string name) const -> PipelineLayout*;
  auto FindOrLoadPipelineLayout(const std::string name) -> PipelineLayout*;
  auto VisitAllPipelineLayouts(PipelineLayoutVisitor* vis) const -> bool;
  auto VisitAllPipelineLayouts(PipelineLayoutPredicate filter) const -> bool;
  auto GetTotalNumberOfPipelineLayouts() const -> uint64_t;
};

void InitPipelineLayoutManager();
auto IsPipelineLayoutManagerInitialized() -> bool;
auto GetPipelineLayoutManager() -> PipelineLayoutManager*;
}  // namespace prt::vk

#endif  // PRT_PIPELINE_LAYOUT_MANAGER_H
