#include <sstream>
#include <glog/logging.h>

#include "prette/prette.h"
#include "prette/flags.h"

#include "prette/engine/engine.h"

#include "prette/vao/vao.h"
#include "prette/vbo/vbo.h"
#include "prette/ibo/ibo.h"
#include "prette/fbo/fbo.h"

#include "prette/shader/shader.h"
#include "prette/program/program.h"

namespace prt {
  std::string GetVersion(){
    std::stringstream ss;
    ss << PRT_VERSION_MAJOR << ".";
    ss << PRT_VERSION_MINOR << ".";
    ss << PRT_VERSION_PATCH;
    return ss.str();
  }

  class GlfwError {
  public:
    int code_;
    std::string message_;
  public:
    GlfwError(const int code, const char* description):
      code_(code),
      message_(description) {
    }
    ~GlfwError() = default;

    std::string name() const {
      return "GlfwError";
    }

    std::string message() const {
      return message_;
    }

    int code() const {
      return code_;
    }

    google::LogSeverity severity() const {
      switch(code_) {
        default:
          return google::FATAL;
      }
    }

    friend std::ostream& operator<<(std::ostream& stream, const GlfwError& rhs) {
      stream << rhs.name() << "[" << rhs.code() << "]: " << rhs.message();
      return stream;
    }
  };

  void OnGlfwError(const int code, const char* description) {
    GlfwError error(code, description);
    LOG_AT_LEVEL(error.severity()) << "unexpected " << error;
  }
}