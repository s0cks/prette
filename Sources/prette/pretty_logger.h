#ifndef PRT_PRETTY_LOGGER_H
#define PRT_PRETTY_LOGGER_H

#include <glog/logging.h>
#include "prette/common.h"

namespace prt {
  class PrettyLogger {
    using Severity=google::LogSeverity;
  protected:
    Severity severity_;
    char* file_;
    int line_;
    int indent_;

    PrettyLogger(const google::LogSeverity severity,
                 const char* file,
                 int line,
                 const int indent):
      severity_(severity),
      indent_(indent),
      line_(line),
      file_(nullptr) {
      PRT_ASSERT(file);
      file_ = strdup(file);
    }
    explicit PrettyLogger(PrettyLogger* parent):
      severity_(google::INFO),
      indent_(0),
      line_(0),
      file_(nullptr) {
      PRT_ASSERT(parent);
      severity_ = parent->GetSeverity();
      indent_ = parent->GetIndent();
      line_ = parent->GetLine();
      PRT_ASSERT(parent->GetFile());
      file_ = strdup(parent->GetFile());
    }

    inline const char* GetFile() const {
      return file_;
    }

    inline int GetLine() const {
      return line_;
    }

    inline Severity GetSeverity() const {
      return severity_;
    }

    inline int GetIndent() {
      return indent_;
    }

    inline void Indent() {
      indent_++;
    }

    inline void Deindent() {
      indent_--;
    }

    std::string GetIndentString() const {
      return std::string(indent_ * 2, ' ');
    }
  public:
    virtual ~PrettyLogger() {
      if(file_)
        free(file_);
    }
  };
}

#endif //PRT_PRETTY_LOGGER_H