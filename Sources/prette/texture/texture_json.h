#ifndef PRT_TEXTURE_JSON_H
#define PRT_TEXTURE_JSON_H

#include "prette/texture/texture_wrap.h"
#include "prette/texture/texture_filter.h"

namespace prt::json {
  class TextureValue {
    DEFINE_NON_COPYABLE_TYPE(TextureValue);
  public:
    static constexpr const auto kFilePropertyName = "file";
    static constexpr const auto kWrapPropertyName = "wrap";
    static constexpr const auto kFilterPropertyName = "filter";
  protected:
    const Value* value_;

    inline const Value* value() const {
      return value_;
    }

    inline std::optional<const Value*>
    GetProperty(const char* name) const {
      if(!value()->HasMember(name))
        return std::nullopt;
      return { &(*value())[name] };
    }
  public:
    explicit TextureValue(const Value* value):
      value_(value) {
      PRT_ASSERT(value);
    }
    virtual ~TextureValue() = default;

    bool IsString() const {
      return value()->IsString();
    }

    bool IsObject() const {
      return value()->IsObject();
    }

    std::optional<const Value*> GetFileProperty() const {
      return GetProperty(kFilePropertyName);
    }

    std::optional<const Value*> GetFilterProperty() const {
      return GetProperty(kFilterPropertyName);
    }

    std::optional<texture::TextureFilter> GetFilter() const {
      const auto filter = GetFilterProperty();
      if(!filter)
        return std::nullopt;
      if((*filter)->IsString()) {
        return { texture::TextureFilter(std::string((*filter)->GetString(), (*filter)->GetStringLength())) };
      } else if((*filter)->IsObject()) {
        return { texture::TextureFilter((*filter)->GetObject()) };
      }
      DLOG(ERROR) << "invalid type for TextureFilter property.";
      return std::nullopt;
    }

    std::optional<const Value*> GetWrapProperty() const {
      return GetProperty(kWrapPropertyName);
    }

    std::optional<texture::TextureWrap> GetWrap() const {
      const auto wrap = GetWrapProperty();
      if(!wrap)
        return std::nullopt;
      if((*wrap)->IsString()) {
        return { texture::TextureWrap(std::string((*wrap)->GetString(), (*wrap)->GetStringLength())) };
      } else if((*wrap)->IsObject()) {
        return { texture::TextureWrap((*wrap)->GetObject()) };
      }
      DLOG(ERROR) << "invalid type for TextureWrap property.";
      return std::nullopt;
    }

    uri::Uri GetFile() const {
      if(IsString())
        return std::string(value()->GetString(), value()->GetStringLength());
      PRT_ASSERT(IsObject());
      const auto file = GetFileProperty();
      PRT_ASSERT(file);
      PRT_ASSERT((*file)->IsString());
      return std::string((*file)->GetString(), (*file)->GetStringLength());
    }
  };

  template<const bool Const>
  class GenericTextureObject {
  public:
    static constexpr const auto kFilePropertyName = "file"; //TODO: move to tex1d,tex2d,tex3d
    static constexpr const auto kFilterPropertyName = "filter";
    static constexpr const auto kWrapPropertyName = "wrap";
  private:
    typedef GenericObject<Const, Document::ValueType> ObjectType;
  protected:
    const ObjectType& value_;

    inline const ObjectType& value() const {
      return value_;
    }
  protected:
    inline std::optional<const Value*>
    GetProperty(const char* name) const {
      if(!value_.HasMember(name))
        return std::nullopt;
      return { &value_[name] };
    }
  public:
    explicit GenericTextureObject(const ObjectType& value):
      value_(value) {
    }
    virtual ~GenericTextureObject() = default;

    virtual std::optional<const Value*> GetFileProperty() const {
      return GetProperty(kFilePropertyName);
    }

    virtual std::optional<const Value*> GetFilterProperty() const {
      return GetProperty(kFilterPropertyName);
    }

    virtual std::optional<const Value*> GetWrapProperty() const {
      return GetProperty(kWrapPropertyName);
    }
  };
  typedef GenericTextureObject<true> ConstTextureObject;
  typedef GenericTextureObject<false> TextureObject;
}

#endif //PRT_TEXTURE_JSON_H