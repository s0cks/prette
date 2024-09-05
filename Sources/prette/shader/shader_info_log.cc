#include "prette/shader/shader_info_log.h"
#include <units.h>
#include <glog/logging.h>

namespace prt::shader {
  using namespace units::data;

  void ShaderInfoLog::Resize(const uword num_bytes) {
    if(num_bytes < length_) {
      DLOG(WARNING) << "cannot resize ShaderInfoLog from " << byte_t(length_) << " to " << byte_t(num_bytes);
      return;
    }
    const auto data = realloc(data_, num_bytes);
    LOG_IF(FATAL, !data) << "failed to Resize ShaderInfoLog to " << byte_t(num_bytes) << ".";
    data_ = (uint8_t*) data;
    length_ = num_bytes;
  }

  uword ShaderInfoLog::GetShaderInfoLogLength(const ShaderId id) {
    PRT_ASSERT(IsValidShaderId(id));
    GLint result;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &result);
    CHECK_GL;
    return static_cast<uword>(result);
  }

  void ShaderInfoLog::GetShaderInfoLogData(const ShaderId id, uint8_t* bytes, const uword max_size, uword* length) {
    glGetShaderInfoLog(id, max_size, (GLsizei*)length, (GLchar*) bytes);
    CHECK_GL;
  }
}