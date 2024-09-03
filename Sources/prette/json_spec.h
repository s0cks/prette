#ifndef PRT_JSON_SPEC_H
#define PRT_JSON_SPEC_H

#include "prette/json.h"

namespace prt::json {
  template<const bool IsConst>
  class GenericSpecObject {
  private:
    typedef json::GenericObject<IsConst, Document::ValueType> ObjectType;
  protected:
    ObjectType value_;

    explicit GenericSpecObject(const ObjectType& value):
      value_(value) {
    }

    inline const ObjectType&
    value() const {
      return value_;
    }
  public:
    GenericSpecObject() = default;
    GenericSpecObject(const GenericSpecObject<IsConst>& rhs) = default;
    virtual ~GenericSpecObject() = default;

    GenericSpecObject<IsConst>& operator=(const GenericSpecObject<IsConst>& rhs) = default;
  };
  typedef GenericSpecObject<true> ConstSpecObject;
  typedef GenericSpecObject<false> SpecObject;

  template<typename T, const bool IsConst>
  class GenericSpecDocument : public GenericSpecObject<IsConst> {
  public:
    static constexpr const auto kTypePropertyName = "type";
    static constexpr const auto kNamePropertyName = "name";
    static constexpr const auto kDataPropertyName = "data";
  private:
    typedef json::GenericObject<IsConst, Document::ValueType> ObjectType;
  public:
    GenericSpecDocument() = default;
    explicit GenericSpecDocument(const ObjectType& value):
      GenericSpecObject<IsConst>(value) {
    }
    GenericSpecDocument(const GenericSpecDocument<T, IsConst>& rhs) = default;
    ~GenericSpecDocument() override = default;

    std::optional<const Value*> GetTypeProperty() const {
      if(!GenericSpecObject<IsConst>::value_.HasMember(kTypePropertyName))
        return std::nullopt;
      return { &GenericSpecObject<IsConst>::value_[kTypePropertyName] };
    }

    std::string GetType() const {
      const auto type = GetTypeProperty();
      PRT_ASSERT((*type)->IsString());
      return std::string((*type)->GetString(), (*type)->GetStringLength());
    }

    std::optional<const Value*> GetNameProperty() const {
      if(!GenericSpecObject<IsConst>::value_.HasMember(kNamePropertyName))
        return std::nullopt;
      return { &GenericSpecObject<IsConst>::value_[kNamePropertyName] };
    }

    std::string GetName() const {
      const auto name = GetNameProperty();
      PRT_ASSERT(name && (*name)->IsString());
      return std::string((*name)->GetString(), (*name)->GetStringLength());
    }

    std::optional<const Value*> GetDataProperty() const {
      if(!GenericSpecObject<IsConst>::value_.HasMember(kDataPropertyName))
        return std::nullopt;
      return { &GenericSpecObject<IsConst>::value_[kDataPropertyName] };
    }

    T GetData() const {
      const auto data = GetDataProperty();
      PRT_ASSERT(data && (*data)->IsObject());
      const auto value = (*data)->GetObject();
      return T(value);
    }

    GenericSpecDocument<T, IsConst>& operator=(const GenericSpecDocument<T, IsConst>& rhs) = default;
  };

  template<class T>
  class ConstSpecDocument : public GenericSpecDocument<T, true> {
  private:
    typedef json::GenericObject<true, Document::ValueType> ObjectType;
  public:
    ConstSpecDocument() = default;
    explicit ConstSpecDocument(const ObjectType& value):
      GenericSpecDocument<T, true> (value) {
    }
    ConstSpecDocument(const ConstSpecDocument<T>& rhs) = default;
    ~ConstSpecDocument() override = default;
    ConstSpecDocument<T>& operator=(const ConstSpecDocument<T>& rhs) = default;
  };

  template<class T>
  class SpecDocument : public GenericSpecDocument<T, false> {
  private:
    typedef json::GenericObject<false, Document::ValueType> ObjectType;
  public:
    SpecDocument() = default;
    explicit SpecDocument(const ObjectType& value):
      GenericSpecDocument<T, false> (value) {
    }
    SpecDocument(const SpecDocument<T>& rhs) = default;
    ~SpecDocument() override = default;
    SpecDocument<T>& operator=(const SpecDocument<T>& rhs) = default;
  };
}

#endif //PRT_JSON_SPEC_H