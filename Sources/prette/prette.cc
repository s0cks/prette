#include <sstream>
#include <glog/logging.h>
#include "prette/common.h"
#include "prette/prette.h"

namespace prt {
  auto GetVersion() -> std::string {
    std::stringstream ss;
    ss << PRT_VERSION_MAJOR << ".";
    ss << PRT_VERSION_MINOR << ".";
    ss << PRT_VERSION_PATCH;
    return ss.str();
  }

  class GlfwError {
    DEFINE_DEFAULT_COPYABLE_TYPE(GlfwError)
  private:
    const char* file_;
    int line_;
    int code_;
    std::string message_;
  public:
    GlfwError(const char* file,
              const int line,
              const int code, const char* description):
      file_(file),
      line_(line),
      code_(code),
      message_(description) {
    }
    ~GlfwError() = default;

    auto GetFile() const -> const char* {
      return file_;
    }

    auto GetLine() const -> int {
      return line_;
    }

    auto GetMessage() const -> const std::string& {
      return message_;
    }

    auto GetCode() const -> int {
      return code_;
    }

    auto GetSeverity() const -> google::LogSeverity {
      switch(GetCode()) {
        default:
          return google::FATAL;
      }
    }

    friend auto operator<<(std::ostream& stream, const GlfwError& rhs) -> std::ostream& {
      stream << "GlfwError[" << rhs.GetCode() << "]: " << rhs.GetMessage();
      return stream;
    }
  };

#define __ google::LogMessage(error.GetFile(), error.GetLine(), error.GetSeverity()).stream()
  void OnGlfwError(const int code, const char* description) {
    GlfwError error(__FILE__, __LINE__, code, description);
    __ << "unexpected " << error;
  }
#undef __
}