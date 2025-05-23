#ifndef PRT_PRETTY_LOGGER_H
#define PRT_PRETTY_LOGGER_H

#include <glog/logging.h>
#include <string>

#include "prette/common.h"

// IWYU pragma: no_include <_string.h>

namespace prt {
class PrettyLogger {
  using Severity = google::LogSeverity;

 protected:
  Severity severity_;
  char* file_;
  int line_;
  int indent_;

  PrettyLogger(const google::LogSeverity severity, const char* file, int line, const int indent) :
    severity_(severity),
    indent_(indent),
    line_(line),
    file_(nullptr) {
    ASSERT(file);
    file_ = strdup(file);
  }
  explicit PrettyLogger(PrettyLogger* parent) :
    severity_(google::INFO),
    indent_(0),
    line_(0),
    file_(nullptr) {
    ASSERT(parent);
    severity_ = parent->GetSeverity();
    indent_ = parent->GetIndent();
    line_ = parent->GetLine();
    ASSERT(parent->GetFile());
    file_ = strdup(parent->GetFile());
  }

  inline auto GetFile() const -> const char* {
    return file_;
  }

  inline auto GetLine() const -> int {
    return line_;
  }

  inline auto GetSeverity() const -> Severity {
    return severity_;
  }

  inline auto GetIndent() -> int {
    return indent_;
  }

  inline void Indent() {
    indent_++;
  }

  inline void Deindent() {
    indent_--;
  }

  auto GetIndentString() const -> std::string {
    return std::string(indent_ * 2, ' ');  // NOLINT(modernize-return-braced-init-list)
  }

 public:
  virtual ~PrettyLogger() {
    if (file_)
      free(file_);
  }
};
}  // namespace prt

#endif  // PRT_PRETTY_LOGGER_H
