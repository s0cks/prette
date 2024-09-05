#ifndef PRT_GFX_H
#error "Please #include <prette/gfx.h> instead."
#endif //PRT_GFX_H

#ifndef PRT_GFX_ERROR_H
#define PRT_GFX_ERROR_H

#include "prette/gfx.h"
#include <exception>

namespace prt::gfx {
#define FOR_EACH_GL_ERROR(V)                                          \
  V(NoError,                      GL_NO_ERROR)                        \
  V(InvalidEnum,                  GL_INVALID_ENUM)                    \
  V(InvalidValue,                 GL_INVALID_VALUE)                   \
  V(InvalidOperation,             GL_INVALID_OPERATION)               \
  V(InvalidFramebufferOperation,  GL_INVALID_FRAMEBUFFER_OPERATION)   \
  V(OutOfMemory,                  GL_OUT_OF_MEMORY)

  static inline const char*
  glErrorString(const GLenum err) {
    switch(err) {
#define DEFINE_TO_STRING(Name, GlValue)         \
      case GlValue: return #Name;               
      FOR_EACH_GL_ERROR(DEFINE_TO_STRING)
#undef DEFINE_TO_STRING
      default:
        return "Unknown GlError";
    }
  }

  class GlError {
  public:
    enum Code : GLenum {
#define DEFINE_CODE(Name, GlValue) k##Name = (GlValue),
      FOR_EACH_GL_ERROR(DEFINE_CODE)
#undef DEFINE_CODE
    };
  private:
    Code code_;
  public:
    constexpr GlError(const Code code = static_cast<Code>(glGetError())):
      code_(code) {
    }
    constexpr GlError(const GlError& rhs):
      code_(rhs.GetCode()) {
    }
    ~GlError() = default;

    constexpr Code GetCode() const {
      return code_;
    }

    constexpr bool IsOk() const {
      return GetCode() == kNoError;
    }

#define DEFINE_CODE_CHECK(Name, GlValue)    \
    constexpr bool Is##Name() const {       \
      return GetCode() == Code::k##Name;    \
    }
    FOR_EACH_GL_ERROR(DEFINE_CODE_CHECK)
#undef DEFINE_CODE_CHECK

    std::string ToString() const;

    constexpr operator bool () const {
      return IsOk(); 
    }

    GlError& operator=(const GlError& rhs) = default;

    friend std::ostream& operator<<(std::ostream& stream, const GlError& rhs) {
      return stream << rhs.ToString();
    }
  };
  
  class GlException : public std::exception {
  private:
    GlError error_;
    std::string message_;
  public:
    explicit GlException(const GlError error);
    ~GlException() override = default;

    GlError GetError() const {
      return error_;
    }

    const std::string& GetMessage() const {
      return message_;
    }

    const char* what() const throw() override {
      return message_.c_str();
    }
  };

#ifdef PRT_DEBUG

#define CHECK_GL                             \
  do {                                       \
    const auto error = gfx::GlError();       \
    if(!error)                               \
      throw gfx::GlException(error);         \
  } while(0);

#else

#define CHECK_GL

#endif//PRT_DEBUG
}

#endif //PRT_GFX_ERROR_H