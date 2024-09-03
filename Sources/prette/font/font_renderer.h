#ifndef PRT_FONT_RENDERER_H
#define PRT_FONT_RENDERER_H

#include "prette/font/font.h"

namespace prt::font {
  class FontRenderer {
  public:
    static constexpr const float kDefaultScale = 1.0f;
    static constexpr const glm::vec3 kDefaultColor = glm::vec3(1.0f, 1.0f, 1.0f);
  protected:
    const glm::mat4& projection_;

    Font* font_;
    float scale_;
    glm::vec3 color_;

    inline constexpr float scale(const float value) {
      return value * scale_;
    }

    inline Font* font() const {
      return font_;
    }

    inline FontMesh& mesh() {
      return font_->mesh_;
    }

    inline Program* shader() {
      return font_->shader_;
    }

    inline GlyphMap& chars() {
      return font_->chars_;
    }
  public:
    FontRenderer(const glm::mat4& projection,
                 Font* font):
      font_(font),
      scale_(kDefaultScale),
      color_(kDefaultColor),
      projection_(projection) {
    }
    virtual ~FontRenderer() = default;

    void SetColor(const glm::vec3 value) {
      color_ = value;
    }
    
    void SetScale(const float scale) {
      scale_ = scale;
    }

    void RenderText(const std::string& text, glm::vec2 pos);
  };
}

#endif //PRT_FONT_RENDERER_H