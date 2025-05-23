#ifndef PRT_PIPELINE_LAYOUT_H
#define PRT_PIPELINE_LAYOUT_H

#include <functional>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include "prette/common.h"
#include "prette/descriptor_set.h"
#include "prette/json.h"
#include "prette/platform.h"
#include "prette/rx.h"
#include "prette/vk.h"

namespace prt {
class TerminatedState;
namespace vk {
class PipelineLayout;
class PipelineLayoutVisitor {
 protected:
  PipelineLayoutVisitor() = default;

 public:
  virtual ~PipelineLayoutVisitor() = default;
  virtual auto Visit(PipelineLayout* rhs) -> bool = 0;
};

using PipelineLayoutPredicate = std::function<bool(PipelineLayout*)>;

class PipelineLayout : public NamedHandleTemplate<VkPipelineLayout> {
 public:
  struct Hash : public prt::Hasher {
    auto operator()(prt::vk::PipelineLayout* lhs) const -> size_t {
      size_t hash = prt::kInvalidHash;
      Combine(hash, lhs->GetName());
      return hash;
    }
  };

  struct Eq {
    auto operator()(PipelineLayout* lhs, PipelineLayout* rhs) const -> bool {
      return lhs->GetName() == rhs->GetName();
    }
  };

 public:
  PipelineLayout(const std::string name, const VkPipelineLayoutCreateInfo& create_info);
  ~PipelineLayout() override;

  auto ToString() const -> std::string override;

  operator VkPipelineLayout() const {
    return GetHandle();
  }

 public:
  static auto New(const std::string name, const VkPipelineLayoutCreateInfo&) -> PipelineLayout*;
  static auto FromJsonFile(const fs::path& rhs) -> PipelineLayout*;
};

class PipelineLayoutFinalizer : public PipelineLayoutVisitor {
 private:
  uint64_t num_finalized_ = 0;

 public:
  PipelineLayoutFinalizer() = default;
  ~PipelineLayoutFinalizer() override = default;
  auto Visit(PipelineLayout* rhs) -> bool override;

  auto GetNumberOfFinalized() const -> uint64_t {
    return num_finalized_;
  }

 public:
  static void FinalizeAll(const std::vector<PipelineLayout*>& layouts);
};

class PipelineLayoutBuilder {
  using VkDescriptorSetLayoutList = std::vector<VkDescriptorSetLayout>;

 private:
  std::string name_{};
  VkPipelineLayoutCreateInfo info_{};
  VkDescriptorSetLayoutList descriptor_set_layouts_{};
  std::vector<VkPushConstantRange> push_ranges_{};

 public:
  PipelineLayoutBuilder() {
    info_.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  }
  ~PipelineLayoutBuilder() = default;

  auto WithName(const std::string name) -> PipelineLayoutBuilder& {
    name_ = std::move(name);
    return *this;
  }

  auto WithDescriptorSetLayouts(const VkDescriptorSetLayout* data, const uint64_t num_layouts)
      -> PipelineLayoutBuilder& {
    descriptor_set_layouts_.insert(std::end(descriptor_set_layouts_), data, data + num_layouts);
    return *this;
  }

  inline auto WithDescriptorSetLayout(const VkDescriptorSetLayout* rhs) -> PipelineLayoutBuilder& {
    ASSERT(rhs);
    return WithDescriptorSetLayouts(rhs, 1);
  }

  inline auto WithDescriptorSetLayouts(DescriptorSet* rhs) -> PipelineLayoutBuilder& {
    ASSERT(rhs);
    return WithDescriptorSetLayout(&rhs->GetLayout());
  }

  template <typename DSetLayoutContainer>
  inline auto WithDescriptorSetLayouts(const DSetLayoutContainer& data) -> PipelineLayoutBuilder& {
    ASSERT(!data.empty());
    return WithDescriptorSetLayouts(data.data(), data.size());
  }

  auto AddPushConstantRange(const uint32_t offset, const uint32_t size, const VkShaderStageFlags stage)
      -> VkPushConstantRange*;

  auto Build() -> PipelineLayout*;
};

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
}  // namespace vk

namespace json {
#define FOR_EACH_PIPELINE_LAYOUT_HANDLER_STATE(V) \
  V(Empty)                                        \
  V(OpenDoc)                                      \
  V(ParsingDescriptorSets)                        \
  V(ClosedDoc)                                    \
  V(Error)

enum class PipelineLayoutHandlerState {
#define DEFINE_STATE(Name) k##Name,
  FOR_EACH_PIPELINE_LAYOUT_HANDLER_STATE(DEFINE_STATE)
#undef DEFINE_STATE
};

class PipelineLayoutHandler : public BaseStatefulReaderHandler<PipelineLayoutHandlerState, PipelineLayoutHandler> {
 private:
  vk::PipelineLayoutBuilder* builder_;
  vk::PipelineLayout* result_ = nullptr;

  inline void SetResult(vk::PipelineLayout* rhs) {
    ASSERT(rhs);
    result_ = rhs;
  }

 public:
  explicit PipelineLayoutHandler(vk::PipelineLayoutBuilder* builder = nullptr);
  ~PipelineLayoutHandler() override = default;

  auto GetResult() const -> vk::PipelineLayout* {
    return result_;
  }

  inline auto HasResult() const -> bool {
    return GetResult() != nullptr;
  }

#define DEFINE_STATE_CHECK(Name)                              \
  inline auto Is##Name() const->bool {                        \
    return GetState() == PipelineLayoutHandlerState::k##Name; \
  }
  FOR_EACH_PIPELINE_LAYOUT_HANDLER_STATE(DEFINE_STATE_CHECK)
#undef DEFINE_STATE_CHECK

  inline auto IsClosed() const -> bool {
    return IsClosedDoc();
  }

  auto Key(const char* str, SizeType length, bool copy) -> bool override;
  auto String(const char* str, SizeType length, bool copy) -> bool override;
  auto StartObject() -> bool override;
  auto EndObject(SizeType memberCount) -> bool override;
  auto StartArray() -> bool override;
  auto EndArray(SizeType elementCount) -> bool override;

  auto Null() -> bool override;
  auto Bool(bool b) -> bool override;
  auto Int(int i) -> bool override;
  auto Uint(unsigned u) -> bool override;
  auto Int64(int64_t i) -> bool override;
  auto Uint64(uint64_t u) -> bool override;
  auto Double(double d) -> bool override;

  operator vk::PipelineLayout*() const {
    return GetResult();
  }
};
}  // namespace json
}  // namespace prt

#endif  // PRT_PIPELINE_LAYOUT_H
