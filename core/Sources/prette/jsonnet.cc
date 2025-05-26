#include "prette/jsonnet.h"

#include <string>

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/script_engine.h"
#include "prette/thread_local.h"

namespace prt {
auto JsonnetScriptEngine::CreateJsonnetVm() -> JsonnetVm* {
  DVLOG(2) << "creating Jsonnet virtual machine....";
  return jsonnet_make();
}

JsonnetScriptEngine::JsonnetScriptEngine() = default;

JsonnetScriptEngine::~JsonnetScriptEngine() = default;

void JsonnetScriptEngine::Init() {
  ScriptEngine::Init();
  ASSERT(vm_ == nullptr);
  vm_ = CreateJsonnetVm();
  ASSERT(vm_);
}

void JsonnetScriptEngine::Finalize() {
  ScriptEngine::Finalize();
  jsonnet_destroy(vm_);
}

auto JsonnetScriptEngine::ExecuteCode(const std::string code) -> bool {
  return false;
}

auto JsonnetScriptEngine::ExecuteScript(const std::string name) -> bool {
  return false;
}

auto JsonnetScriptEngine::ExecuteGlobalFunc(const std::string name) -> bool {
  return false;
}

static ThreadLocal<JsonnetScriptEngine> jsonnet_;

auto IsJsonnetInitialized() -> bool {
  return jsonnet_.Get() != nullptr && jsonnet_->IsInitialized();
}

void InitJsonnet() {
  ASSERT(!IsJsonnetInitialized());
  jsonnet_ = new JsonnetScriptEngine();
}
}  // namespace prt