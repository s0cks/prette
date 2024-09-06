#ifndef PRT_CRASH_REPORT_H
#define PRT_CRASH_REPORT_H

#include <ostream>
#include <exception>
#include <backward.hpp>

#include "prette/common.h"

namespace prt {
  using backward::StackTrace;
  using ColorMode=backward::ColorMode::type;

  class CrashReportCause {
    DEFINE_NON_COPYABLE_TYPE(CrashReportCause);
  public:
    static constexpr const auto kDefaultDepth = 32;
    static constexpr const auto kDefaultOffset = 7;
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
    CrashReportCause():
      cause_(nullptr),
      trace_(),
      depth_(kDefaultDepth),
      offset_(kDefaultOffset) {
    }
    CrashReportCause(const std::exception_ptr& cause,
                     const int depth = kDefaultDepth,
                     const int offset = kDefaultOffset):
      cause_(cause),
      trace_(),
      depth_(depth),
      offset_(offset) {
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
      return (bool) cause_;
    }
  };

  class CrashReport {
  public:
    static constexpr const auto kDefaultColorMode = ColorMode::always;
  private:
    const CrashReportCause& cause_;
    ColorMode color_mode_;

    void PrintStackTrace(std::ostream& stream);
  public:
    CrashReport(const CrashReportCause& cause,
                const ColorMode color_mode = kDefaultColorMode):
      color_mode_(color_mode),
      cause_(cause) {
    }

    auto GetColorMode() const -> ColorMode {
      return color_mode_;
    }

    void Print(std::ostream& stream = std::cerr);
  };
}

#endif //PRT_CRASH_REPORT_H