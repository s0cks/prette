#ifndef PRT_SHADER_COMPILE_STATUS_H
#define PRT_SHADER_COMPILE_STATUS_H

#include "prette/shader/shader_id.h"
#include "prette/shader/shader_info_log.h"

namespace prt::shader {
  class ShaderCompileStatus {
    DEFINE_DEFAULT_COPYABLE_TYPE(ShaderCompileStatus);
  private:
    ShaderId id_{};
    bool compiled_{};
    ShaderInfoLog info_{};
  public:
    ShaderCompileStatus(const ShaderId id);
    ~ShaderCompileStatus() = default;

    auto GetShaderId() const -> ShaderId {
      return id_;
    }

    auto IsCompiled() const -> bool {
      return compiled_;
    }

    auto GetInfo() const -> const ShaderInfoLog& {
      return info_;
    }

    auto HasMessage() const -> bool {
      return !GetInfo().IsEmpty();
    }

    operator bool() const {
      return IsCompiled();
    }

    auto ToString() const -> std::string;

    friend auto operator<<(std::ostream& stream, const ShaderCompileStatus& rhs) -> std::ostream& {
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