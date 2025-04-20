#ifndef PRT_TO_STRING_H
#define PRT_TO_STRING_H

#include <glog/logging.h>

#include <functional>
#include <string>
#include <type_traits>
#include <utility>

#include "prette/common.h"

namespace prt {
class Window;

namespace tostring {
template <typename T>
struct has_to_string {
  static constexpr const auto value = false;
};

#define DECLARE_HAS_TO_STRING(Name)           \
  template <>                                 \
  struct has_to_string<Name> {                \
    static constexpr const auto value = true; \
  };

DECLARE_HAS_TO_STRING(prt::Window);
#undef DECLARE_HAS_TO_STRING

class ToStringHelperBase {
  DEFINE_DEFAULT_COPYABLE_TYPE(ToStringHelperBase);

 public:
  class Field {
    DEFINE_DEFAULT_COPYABLE_TYPE(Field);

   public:
    using NameSupplier = std::function<std::string()>;
    using ValueSupplier = std::function<std::string()>;

   private:
    NameSupplier name_;
    ValueSupplier value_;

   public:
    Field() = default;
    Field(NameSupplier name, ValueSupplier value) :
      name_(std::move(name)),
      value_(std::move(value)) {}
    ~Field() = default;

    auto name() const -> const NameSupplier& {
      return name_;
    }

    auto GetName() const -> std::string {
      return name_();
    }

    auto value() const -> const ValueSupplier& {
      return value_;
    }

    auto GetValue() const -> std::string {
      return value_();
    }

    friend auto operator<<(std::ostream& stream, const Field& rhs) -> std::ostream& {
      return stream << rhs.GetName() << "=" << rhs.GetValue();
    }
  };

  using FieldList = std::vector<Field>;

 private:
  FieldList fields_;

 protected:
  explicit ToStringHelperBase(FieldList fields) :
    fields_(std::move(fields)) {}
  explicit ToStringHelperBase(const size_t num_fields = 0) :
    fields_(num_fields) {}

  void EmplaceBackField(const Field::NameSupplier& name, const Field::ValueSupplier& value) {
    fields_.emplace_back(name, value);
  }

  virtual auto GetTypename() const -> std::string = 0;

 public:
  virtual ~ToStringHelperBase() = default;
  virtual auto ToString() const -> std::string;
};

template <typename T>
class ToStringHelper : public ToStringHelperBase {
  DEFINE_DEFAULT_COPYABLE_TYPE(ToStringHelper<T>);

 private:
  template <typename V>
  static inline auto Identity(V value) -> std::function<V()> {
    return [value]() {
      return value;
    };
  }

 protected:
  auto GetTypename() const -> std::string override {
    return prt::GetTypename<T>();
  }

 public:
  ToStringHelper() = default;
  explicit ToStringHelper(const FieldList& fields) :
    ToStringHelperBase(fields) {}
  explicit ToStringHelper(const size_t num_fields) :
    ToStringHelperBase(num_fields) {}
  ~ToStringHelper() override = default;

  auto AddField(std::string name, std::string value) -> ToStringHelper<T>& {
    ASSERT(!name.empty());
    EmplaceBackField(Identity(name), Identity(value));
    return *this;
  }

  inline auto AddField(std::string name, const std::stringstream& ss) -> ToStringHelper<T>& {
    return AddField(std::move(name), ss.str());
  }

  auto AddFieldPtr(std::string name, const void* value) -> ToStringHelper<T>& {
    ASSERT(!name.empty());
    EmplaceBackField(Identity(name), [value]() -> std::string {
      if (value == nullptr)
        return "nullptr";
      std::stringstream ss;
      ss << value;
      return ss.str();
    });
    return *this;
  }

  template <typename V>
  auto AddFieldPtr(std::string name, const V* value, std::enable_if_t<has_to_string<V>::value>* = nullptr) -> ToStringHelper<T>& {
    ASSERT(!name.empty());
    EmplaceBackField(Identity(name), [value]() -> std::string {
      if (value == nullptr)
        return "nullptr";
      std::stringstream ss;
      ss << value->ToString();
      return ss.str();
    });
    return *this;
  }

  template <typename V>
  auto AddFieldRef(std::string name, const V& value) -> ToStringHelper<T>& {
    ASSERT(!name.empty());
    EmplaceBackField(Identity(name), [value]() -> std::string {
      std::stringstream ss;
      ss << value;
      return ss.str();
    });
    return *this;
  }

  operator std::string() const {
    return ToString();
  }

  friend auto operator<<(std::ostream& stream, const ToStringHelper<T>& rhs) -> std::ostream& {
    return stream << (std::string)rhs;
  }
};
}  // namespace tostring

using tostring::ToStringHelper;
}  // namespace prt

#endif  // PRT_TO_STRING_H
