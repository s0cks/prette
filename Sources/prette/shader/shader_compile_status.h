#ifndef PRT_SHADER_COMPILE_STATUS_H
#define PRT_SHADER_COMPILE_STATUS_H

#include "prette/gfx.h"
#include "prette/shader/shader_id.h"
#include "prette/shader/shader_info_log.h"

namespace prt::shader {
  class ShaderCompileStatus {
  protected:
    ShaderId id_;
    bool compiled_;
    ShaderInfoLog info_;
  public:
    ShaderCompileStatus(const ShaderId id);
    ShaderCompileStatus(const ShaderCompileStatus& rhs) = default;
    ~ShaderCompileStatus() = default;

    ShaderId GetShaderId() const {
      return id_;
    }

    bool IsCompiled() const {
      return compiled_;
    }

    const ShaderInfoLog& GetInfo() const {
      return info_;
    }

    bool HasMessage() const {
      return !GetInfo().IsEmpty();
    }

    operator bool() const {
      return IsCompiled();
    }

    std::string ToString() const;

    ShaderCompileStatus& operator=(const ShaderCompileStatus& rhs) = default;

    friend std::ostream& operator<<(std::ostream& stream, const ShaderCompileStatus& rhs) {
      if(rhs) {
        return stream << "Compiled";
      } else if(!rhs && rhs.HasMessage()) {
        return stream << "Compilation Error: " << rhs.GetInfo();
      }
      return stream << "Unknown ShaderCompilerStatus: " << rhs.ToString();
    }
  };
}

#endif //PRT_SHADER_COMPILE_STATUS_H