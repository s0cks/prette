#ifndef PRT_RENDER_PIPELINE_H
#define PRT_RENDER_PIPELINE_H

#include "prette/pipeline.h"

namespace prt::render {
  class RenderPipeline {
    static constexpr const auto kX_AXIS = glm::vec3(1.0f, 0.0f, 0.0f);
    static constexpr const auto kY_AXIS = glm::vec3(0.0f, 1.0f, 0.0f);
    static constexpr const auto kZ_AXIS = glm::vec3(0.0f, 0.0f, 1.0f);
  protected:
    glm::mat4 projection_;
    glm::mat4 view_;
    glm::mat4 model_;

    RenderPipeline(const glm::mat4& projection,
                   const glm::mat4& view,
                   const glm::mat4& model = glm::mat4(1.0f)):
      projection_(projection),
      view_(view),
      model_(model) {
    }
  public:
    virtual ~RenderPipeline() = default;

    const glm::mat4& GetProjection() const {
      return projection_;
    }

    void SetProjection(const glm::mat4& projection) {
      projection_ = projection;
    }

    const glm::mat4& GetView() const {
      return view_;
    }

    void SetView(const glm::mat4& view) {
      view_ = view;
    }

    const glm::mat4& GetModel() const {
      return model_;
    }

    void SetModel(const glm::mat4& model) {
      model_ = model;
    }

    void Translate(const glm::vec3& translation) {
      model_ = glm::translate(model_, translation);
    }

    inline void TranslateX(const float x) {
      return Translate(glm::vec3(0.0f, x, 0.0f));
    }

    inline void TranslateY(const float y) {
      return Translate(glm::vec3(0.0f, y, 0.0f));
    }

    inline void TranslateZ(const float z) {
      return Translate(glm::vec3(0.0f, 0.0f, z));
    }

    void Scale(const glm::vec3& scale) {
      model_ = glm::scale(model_, scale);
    }

    inline void ScaleX(const float x) {
      return Scale(glm::vec3(x, 1.f, 1.0f));
    }

    inline void ScaleY(const float y) {
      return Scale(glm::vec3(1.0f, y, 1.0f));
    }

    inline void ScaleZ(const float z) {
      return Scale(glm::vec3(1.0f, 1.0f, z));
    }

    void Rotate(const float deg, const glm::vec3& axis) {
      model_ = glm::rotate(model_, glm::radians(deg), axis);
    }

    inline void RotateX(const float deg) {
      return Rotate(deg, kX_AXIS);
    }

    inline void RotateY(const float deg) {
      return Rotate(deg, kY_AXIS);
    }

    inline void RotateZ(const float deg) {
      return Rotate(deg, kZ_AXIS);
    }
  };
}

#endif //PRT_RENDER_PIPELINE_H