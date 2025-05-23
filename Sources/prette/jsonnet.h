#ifndef PRT_JSONNET_H
#define PRT_JSONNET_H

#include <fmt/format.h>
#include <ostream>

// IWYU pragma: begin_exports
extern "C" {
#include <libjsonnet.h>
}
// IWYU pragma: end_exports

#include <string>

#include "prette/common.h"

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

void InitJsonnet();
auto EvalJsonnet(const std::string code) -> JsonnetResult;
}  // namespace prt

#endif  // PRT_JSONNET_H
