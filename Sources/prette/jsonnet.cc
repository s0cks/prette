#include "prette/jsonnet.h"

#include <string>
#include <utility>

#include "prette/common.h"
#include "prette/engine.h"
#include "prette/thread_local.h"

namespace prt {
class Jsonnet {
 private:
  JsonnetVm* vm_ = nullptr;

  void Init() {
    DLOG(INFO) << "initializing Jsonnet....";
    vm_ = jsonnet_make();
  }

 public:
  explicit Jsonnet(Engine* engine) {
    ASSERT(engine);
    engine->AddInitCallback([this]() {
      Init();
    });
  }
  ~Jsonnet() = default;

  auto vm() const -> JsonnetVm* {
    return vm_;
  }

  inline auto IsInitialized() const -> bool {
    return vm() != nullptr;
  }

  auto EvalSnippet(const std::string code) -> JsonnetResult {
    ASSERT(IsInitialized());
    int error = 0;
    const auto result = jsonnet_evaluate_snippet(vm(), "snippet", code.c_str(), &error);
    if (result == nullptr || error != 0)
      return JsonnetResult(true, "error evaluating jsonnet");
    JsonnetResult res(false, result);
    jsonnet_realloc(vm(), result, 0);
    return res;
  }
};

static ThreadLocal<Jsonnet> jsonnet_;

static inline auto IsInitialized() -> bool {
  return jsonnet_.Get() != nullptr;
}

void InitJsonnet() {
  jsonnet_ = new Jsonnet(GetEngine());
}

auto EvalJsonnet(const std::string code) -> JsonnetResult {
  ASSERT(IsInitialized());
  return jsonnet_->EvalSnippet(std::move(code));
}
}  // namespace prt