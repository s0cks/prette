#ifndef PRT_TEXTURE_BUILDER_H
#define PRT_TEXTURE_BUILDER_H

#include "prette/image/image.h"
#include "prette/texture/texture_id.h"
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
    img::Image* data_;

    void InitTexture(const TextureId id) const override;
  public:
    explicit TextureBuilder(const TextureTarget target):
      TextureBuilderBase(target),
      level_(0),
      border_(0),
      data_(nullptr) {
    }
    virtual ~TextureBuilder() = default;

    void SetLevel(const int level) {
      PRT_ASSERT(level >= 0);
      level_ = level;
    }

    int GetBorder() const {
      return border_;
    }

    void SetBorder(const int border) {
      PRT_ASSERT(border >= 0);
      border_ = border;
    }

    int GetLevel() const {
      return level_;
    }

    img::Image* GetData() const {
      return data_;
    }

    void SetData(img::Image* rhs) {
      PRT_ASSERT(rhs);
      data_ = rhs;
    }

    inline bool HasData() const {
      return data_ != nullptr;
    }

    TextureBuilder& operator<<(img::Image* rhs) {
      SetData(rhs);
      return *this;
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