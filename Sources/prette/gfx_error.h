#ifndef PRT_GFX_H
#error "Please #include <prette/gfx.h> instead."
#endif //PRT_GFX_H

#ifndef PRT_GFX_ERROR_H
#define PRT_GFX_ERROR_H

#include <exception>

namespace prt::gfx {
#define FOR_EACH_GL_ERROR(V)                                          \
  V(NoError,                      GL_NO_ERROR)                        \
  V(InvalidEnum,                  GL_INVALID_ENUM)                    \
  V(InvalidValue,                 GL_INVALID_VALUE)                   \
  V(InvalidOperation,             GL_INVALID_OPERATION)               \
  V(InvalidFramebufferOperation,  GL_INVALID_FRAMEBUFFER_OPERATION)   \
  V(OutOfMemory,                  GL_OUT_OF_MEMORY)

  enum ErrorCode : GLenum {
#define DEFINE_ERROR_CODE(Name, GlValue)    \
    k##Name = (GlValue),
    FOR_EACH_GL_ERROR(DEFINE_ERROR_CODE)
#undef DEFINE_ERROR_CODE
  };

  static inline const char*
  GetName(const ErrorCode& rhs) {
    switch(rhs) {
#define DEFINE_GET_NAME(Name, GlValue)        \
      case GlValue: return #Name;
      FOR_EACH_GL_ERROR(DEFINE_GET_NAME)
#undef DEFINE_GET_NAME
      default: return "Unknown";
    }
  }

  static inline std::string
  GetMessage(const ErrorCode& rhs) {
    switch(rhs) {
#define DEFINE_GET_NAME(Name, GlValue)        \
      case GlValue: return #Name;
      FOR_EACH_GL_ERROR(DEFINE_GET_NAME)
#undef DEFINE_GET_NAME
      default: {
        std::stringstream ss;
        ss << "Unknown GlError: " << rhs;
        return ss.str();
      }
    }
  }

  class GlException : public std::exception {
  private:
    ErrorCode value_;
    std::string message_;
  public:
    explicit GlException(const ErrorCode value):
      std::exception(),
      value_(value),
      message_(GetMessage(value)) {
    }
    ~GlException() override = default;

    ErrorCode value() const {
      return value_;
    }

    const char* what() const throw() override {
      return message_.data();
    }
  };

#ifdef PRT_DEBUG

#define _CHECK_GL_(Severity, File, Line) ({                                                  \
  gfx::ErrorCode error;                                                                      \
  while((error = static_cast<gfx::ErrorCode>(glGetError())) != gfx::ErrorCode::kNoError)     \
    throw gfx::GlException(error);                                                           \
})

#define CHECK_GL(Severity)  \
  _CHECK_GL_(Severity, __FILE__, __LINE__)

#else

#define CHECK_GL(Severity)

#endif//PRT_DEBUG
}

#endif //PRT_GFX_ERROR_H