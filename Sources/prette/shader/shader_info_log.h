#ifndef PRT_SHADER_INFO_LOG_H
#define PRT_SHADER_INFO_LOG_H

#include "prette/common.h"
#include "prette/platform.h"
#include "prette/shader/shader_id.h"

namespace prt::shader {
  class ShaderInfoLog {
    //TODO: define proper copy & move ctor & dtor
    //TODO: cleanup string buffer utilization
  private:
    static auto GetShaderInfoLogLength(const ShaderId id) -> uword;
    static void GetShaderInfoLogData(const ShaderId id, uint8_t* bytes, const uword max_size, uword* length);
  private:
    ShaderId id_;
    uword length_;
    uint8_t* data_;

    void Resize(const uword num_bytes);

    inline void CopyFrom(const uint8_t* bytes, const uword num_bytes) {
      PRT_ASSERT(bytes);
      PRT_ASSERT(num_bytes >= 1);
      Resize(num_bytes);
      memcpy(data_, bytes, num_bytes);
    }

    inline void CopyFrom(const ShaderInfoLog& rhs) {
      return CopyFrom(rhs.data(), rhs.length());
    }

    inline void Release() {
      if(data_)
        free(data_);
      data_ = nullptr;
      length_ = 0;
    }
  public:
    ShaderInfoLog() = default;
    explicit ShaderInfoLog(const ShaderId id):
      id_(id),
      length_(0),
      data_(nullptr) {
      auto length = GetShaderInfoLogLength(id);
      if(length <= 0)
        return;
      Resize(length);
      GetShaderInfoLogData(id, data_, length, &length);
      LOG_IF(FATAL, length <= 0) << "failed to get ShaderInfoLog for shader: " << id;
    }
    ShaderInfoLog(const ShaderInfoLog& rhs):
      id_(rhs.id_),
      length_(0),
      data_(nullptr) {
      CopyFrom(rhs);
    }
    ~ShaderInfoLog() {
      Release();
    }

    auto GetShaderId() const -> ShaderId {
      return id_;
    }

    auto length() const -> uword {
      return length_;
    }

    auto data() const -> const uint8_t* {
      return data_;
    }

    auto IsEmpty() const -> bool {
      return data_ == nullptr || length_ == 0;
    }

    operator std::string() const {
      return { (const char*) data(), length() };
    }

    auto operator=(const ShaderInfoLog& rhs) -> ShaderInfoLog& {
      id_ = rhs.id_;
      CopyFrom(rhs);
      return *this;
    }

    friend auto operator<<(std::ostream& stream, const ShaderInfoLog& rhs) -> std::ostream& {
      return stream << ((const std::string&) rhs);
    }
  };
}

#endif //PRT_SHADER_INFO_LOG_H