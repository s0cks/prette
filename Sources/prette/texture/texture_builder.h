#ifndef PRT_TEXTURE_BUILDER_H
#define PRT_TEXTURE_BUILDER_H

#include "prette/image/image.h"
#include "prette/texture/texture_id.h"
#include "prette/texture/texture_data.h"
#include "prette/texture/texture_wrap.h"
#include "prette/texture/texture_target.h"
#include "prette/texture/texture_format.h"
#include "prette/texture/texture_filter.h"

namespace prt::texture {
  class TextureBuilderBase {
  protected:
    TextureTarget target_;
    TextureFilter filter_;
    TextureWrap wrap_;

    explicit TextureBuilderBase(const TextureTarget target):
      target_(target),
      filter_(kLinearFilter),
      wrap_(kClampToEdgeWrap) {
    }

    virtual void InitTexture(const TextureId id) const = 0;
  public:
    virtual ~TextureBuilderBase() = default;
    virtual TextureId Build() const;
    virtual rx::observable<TextureId> BuildAsync() const;

    TextureTarget GetTarget() const {
      return target_;
    }

    const TextureFilter& GetFilter() const {
      return filter_;
    }

    void SetFilter(const TextureFilter& filter) {
      filter_ = filter;
    }

    const TextureWrap& GetWrap() const {
      return wrap_;
    }

    void SetWrap(const TextureWrap& wrap) {
      wrap_ = wrap;
    }
  };

  class TextureBuilder : public TextureBuilderBase {
  protected:
    int level_;
    int border_;
    TextureData data_;

    void InitTexture(const TextureId id) const override;
  public:
    explicit TextureBuilder(const TextureTarget target):
      TextureBuilderBase(target),
      level_(0),
      border_(0),
      data_() {
    }
    virtual ~TextureBuilder() = default;

    const TextureFormat GetFormat() const {
      return data_.format();
    }

    virtual void SetFormat(const TextureFormat format) {
      data_.format_ = format;
    }

    const TextureFormat GetInternalFormat() const {
      return data_.internal_format();
    }

    virtual void SetInternalFormat(const TextureFormat format) {
      data_.internal_format_ = format;
    }

    const glm::vec2& GetSize() const {
      return data_.size();
    }

    virtual void SetSize(const TextureSize& size) {
      data_.size_ = size;
    }

    float GetWidth() const {
      return data_.width();
    }

    float GetHeight() const {
      return data_.height();
    }

    const int& GetLevel() const {
      return level_;
    }

    virtual void SetLevel(const int level) {
      level_ = level;
    }

    const int& GetBorder() const {
      return border_;
    }

    virtual void SetBorder(const int border) {
      border_ = border;
    }
    
    GLenum GetType() const {
      return data_.type();
    }

    virtual void SetType(const GLenum type) {
      data_.type_ = type;
    }

    virtual const GLvoid* GetData() const {
      return (const GLvoid*) data_.bytes();
    }

    void operator<<(const img::Image* img) {
      PRT_ASSERT(img);
      data_ = img;
    }

    friend std::ostream& operator<<(std::ostream& stream, const TextureBuilder& rhs) {
      stream << "TextureBuilder(";
      stream << "target=" << rhs.GetTarget() << ", ";
      stream << "filter=" << rhs.GetFilter() << ", ";
      stream << "wrap=" << rhs.GetWrap() << ", ";
      stream << "border=" << rhs.GetBorder() << ", ";
      stream << "level=" << rhs.GetLevel() << ", ";
      stream << "data=" << rhs.data_;
      stream << ")";
      return stream;
    }
  };
}

#endif //PRT_TEXTURE_BUILDER_H