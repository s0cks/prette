#ifndef PRT_PROGRAM_SCOPE_H
#error "Please #include <prette/program/program_scope.h> instead."
#endif //PRT_PROGRAM_SCOPE_H

#ifndef PRT_PROGRAM_SCOPE_APPLY_H
#define PRT_PROGRAM_SCOPE_APPLY_H

#include "prette/ubo/ubo_id.h"
#include "prette/camera/camera.h"
#include "prette/texture/texture_id.h"
#include "prette/program/program_scope.h"

namespace prt::program {
  using UniformLocation = GLint;
  using UniformBlockIndex = GLuint;

  class ApplyProgramScope : public ProgramScope {
    DEFINE_NON_COPYABLE_TYPE(ApplyProgramScope);
  public:
    explicit ApplyProgramScope(Program* program);
    ~ApplyProgramScope() override;
    virtual auto GetUniformLocation(const char* name) const -> UniformLocation; //TODO: cache value, reduce visibility?
    virtual auto GetUniformBlockIndex(const char* name) const -> UniformBlockIndex;
    virtual void Set(const char* name, const glm::vec2& value) const;
    virtual void Set(const char* name, const glm::vec3& value) const;
    virtual void Set(const char* name, const glm::vec4& value) const;
    virtual void Set(const char* name, const glm::mat4& value, const bool transpose = GL_FALSE) const;
    virtual void Set(const char* name, const float value) const;
    virtual void Set(const char* name, const int32_t value) const;
    virtual void Set(const char* name, const uint32_t value) const;

    inline void Set(const char* name, const bool value) const {
      return Set(name, static_cast<int32_t>(value));
    }

    inline void SetViewMatrix(const glm::mat4& value) const {
      return Set("view", value);
    }

    inline void SetProjectionMatrix(const glm::mat4& value) const {
      return Set("projection", value);
    }

    inline void SetModelMatrix(const glm::mat4& value) const {
      return Set("model", value);
    }

    inline void SetVP(const glm::mat4& view, const glm::mat4& projection) const {
      SetViewMatrix(view);
      SetProjectionMatrix(projection);
    }

    inline void SetVP(Camera* camera) const {
      return SetVP(camera->GetView(), camera->GetProjection());
    }

    virtual void Apply();
    virtual void Unapply();
  };
}

#endif //PRT_PROGRAM_SCOPE_APPLY_H