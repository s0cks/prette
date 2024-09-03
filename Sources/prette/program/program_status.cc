#include "prette/program/program_status.h"

namespace prt::program {
  void ProgramInfoLog::GetProgramInfoLog(const ProgramId id, uint8_t** result, uint32_t* length) {
    PRT_ASSERT(IsValidProgramId(id));

    GLint len = 0;
    glGetProgramiv(id, GL_INFO_LOG_LENGTH, &len);
    CHECK_GL(FATAL);

    if(len < 0) {
      DLOG(WARNING) << "not getting program info log of length: " << len;
      (*result) = nullptr;
      (*length) = len;
      return;
    }

    const auto total_size = sizeof(uint8_t) * len;
    const auto data = (uint8_t*) malloc(total_size);
    if(!data) {
      using namespace units::data;
      DLOG(ERROR) << "failed to allocate ProgramInfoLog data of size: " << byte_t(total_size);
      (*result) = nullptr;
      (*length) = 0;
      return;
    }

    glGetProgramInfoLog(id, len, &len, (GLchar*) data);
    CHECK_GL(FATAL);
    (*result) = data;
    (*length) = len;
  }
}