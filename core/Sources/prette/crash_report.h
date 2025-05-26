#ifndef PRT_CRASH_REPORT_H
#define PRT_CRASH_REPORT_H

#include <backward.hpp>
#include <exception>
#include <iostream>
#include <memory>
#include <utility>

#include "prette/assertions.h"
#include "prette/common.h"

namespace prt {
using backward::StackTrace;
using ColorMode = backward::ColorMode::type;

class CrashReportCause {
  DEFINE_NON_COPYABLE_TYPE(CrashReportCause);

 public:
  static constexpr const auto kDefaultDepth = 32;
  static constexpr const auto kDefaultOffset = 13;

 private:
  std::exception_ptr cause_;
  StackTrace trace_;
  int depth_;
  int offset_;

  inline void LoadTrace() {
    trace_.load_here(GetDepth());
    trace_.skip_n_firsts(GetOffset());
  }

 public:
  CrashReportCause() :
    cause_(nullptr),
    trace_(),
    depth_(kDefaultDepth),
    offset_(kDefaultOffset) {}
  explicit CrashReportCause(const std::exception_ptr& cause, const int depth = kDefaultDepth,
                            const int offset = kDefaultOffset) :
    cause_(cause),
    trace_(),
    depth_(depth),
    offset_(offset) {
    if (depth_ < offset_)
      depth_ = (offset_ + depth_);
    LoadTrace();
  }
  ~CrashReportCause() = default;

  auto GetException() const -> const std::exception_ptr& {
    return cause_;
  }

  auto GetTrace() const -> const StackTrace& {
    return trace_;
  }

  auto GetDepth() const -> int {
    return depth_;
  }

  auto GetOffset() const -> int {
    return offset_;
  }

  auto operator=(const std::exception_ptr& cause) -> CrashReportCause& {
    cause_ = cause;
    LoadTrace();
    return *this;
  }

  explicit operator bool() const {
    return (bool)cause_;
  }

 public:
  static inline auto New(const std::exception_ptr& cause, const int depth = kDefaultDepth,
                         const int offset = kDefaultOffset) -> std::shared_ptr<CrashReportCause> {
    ASSERT(cause);
    ASSERT(depth >= 1);
    ASSERT(offset >= 0);
    return std::make_shared<CrashReportCause>(cause, depth, offset);
  }
};

class CrashReport {
 public:
  static constexpr const auto kDefaultColorMode = ColorMode::always;

 private:
  std::shared_ptr<CrashReportCause> cause_;
  ColorMode color_mode_;

  void PrintStackTrace(std::ostream& stream);

 public:
  explicit CrashReport(std::shared_ptr<CrashReportCause> cause, const ColorMode color_mode = kDefaultColorMode) :
    color_mode_(color_mode),
    cause_(std::move(cause)) {}

  auto GetColorMode() const -> ColorMode {
    return color_mode_;
  }

  void Print(std::ostream& stream = std::cerr);
};
}  // namespace prt

#endif  // PRT_CRASH_REPORT_H