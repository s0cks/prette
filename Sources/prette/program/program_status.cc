#include "prette/program/program_status.h"

namespace prt::program {
  void ProgramInfoLog::GetProgramInfoLog(const ProgramId id, uint8_t** result, uint32_t* length) {
    PRT_ASSERT(id);

    GLint len = 0;
    glGetProgramiv((GLuint) id, GL_INFO_LOG_LENGTH, &len);
    CHECK_GL;

    if(len < 0) {
      DLOG(WARNING) << "not getting program info log of length: " << len;
      (*result) = nullptr;
      (*length) = len;
      return;
    }

    const auto total_size = sizeof(uint8_t) * len;
    const auto data = (uint8_t*) malloc(total_size); // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
    if(!data) {
      using namespace units::data;
      DLOG(ERROR) << "failed to allocate ProgramInfoLog data of size: " << byte_t(total_size); // NOLINT(cppcoreguidelines-narrowing-conversions)
      (*result) = nullptr;
      (*length) = 0;
      return;
    }

    glGetProgramInfoLog(id, len, &len, (GLchar*) data); // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
    CHECK_GL;
    (*result) = data;
    (*length) = len;
  }
}