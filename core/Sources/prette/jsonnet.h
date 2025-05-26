#ifndef PRT_JSONNET_H
#define PRT_JSONNET_H

#include <fmt/format.h>
#include <ostream>
#include <string>

#include "prette/assertions.h"
#include "prette/rx.h"  // IWYU pragma: keep

extern "C" {
// IWYU pragma: begin_exports
#include <libjsonnet.h>
// IWYU pragma: end_exports
}

#include "prette/common.h"
#include "prette/script_engine.h"

namespace prt {
class JsonnetResult {
  DEFINE_DEFAULT_COPYABLE_TYPE(JsonnetResult);

 private:
  bool error_ = false;
  std::string value_;

 public:
  JsonnetResult(const bool error, const std::string value) :
    error_(error),
    value_(value) {}
  ~JsonnetResult() = default;

  auto Get() const -> const std::string& {
    return value_;
  }

  auto IsError() const -> bool {
    return error_;
  }

  auto IsEmpty() const -> bool {
    return Get().empty();
  }

  inline auto IsOk() const -> bool {
    return !IsEmpty() && !IsError();
  }

  operator std::string() const {
    return Get();
  }

  auto ToString() const -> std::string {
    if (IsError())
      return fmt::format("Jsonnet error: {}", Get());
    return Get();
  }

  friend auto operator<<(std::ostream& stream, const JsonnetResult& rhs) -> std::ostream& {
    return stream << rhs.ToString();
  }
};

class JsonnetScriptEngine : public ScriptEngine {
  static auto CreateJsonnetVm() -> JsonnetVm*;

 private:
  JsonnetVm* vm_ = nullptr;

  void Init() override;
  void Finalize() override;

 public:
  JsonnetScriptEngine();
  ~JsonnetScriptEngine();

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

  auto GetLanguage() const -> const char* override {
    return "jsonnet";
  }

  auto GetLanguageVersion() const -> const char* override {
    return "??";
  }

  auto ExecuteCode(const std::string code) -> bool override;
  auto ExecuteScript(const std::string name) -> bool override;
  auto ExecuteGlobalFunc(const std::string name) -> bool override;
};

auto IsJsonnetInitialized() -> bool;
void InitJsonnet();
}  // namespace prt

#endif  // PRT_JSONNET_H
