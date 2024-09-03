#ifndef PRT_PROPERTY_H
#define PRT_PROPERTY_H

#include <string>
#include <cstdint>

namespace prt::properties {
#define FOR_EACH_PROPERTY_TYPE(V)       \
  V(String)                             \
  V(Long)                               \
  V(Double)                             \
  V(Bool)

  class Property;
#define FORWARD_DECLARE(Name) class Name##Property;
  FOR_EACH_PROPERTY_TYPE(FORWARD_DECLARE)
#undef FORWARD_DECLARE

  class PropertyVisitor {
  protected:
    PropertyVisitor() = default;
  public:
    virtual ~PropertyVisitor() = default;
    virtual bool Visit(Property* property) = 0;
  };

  class Property {
  protected:
    Property() = default;
  public:
    virtual ~Property() = default;
    virtual std::string ToString() const = 0;
    virtual const std::string& GetName() const = 0;

#define DEFINE_TYPE_CHECK(Name)                               \
    virtual Name##Property* As##Name() { return nullptr; }    \
    bool Is##Name() { return As##Name() != nullptr; }
    FOR_EACH_PROPERTY_TYPE(DEFINE_TYPE_CHECK)
#undef DEFINE_TYPE_CHECK

    friend std::ostream& operator<<(std::ostream& stream, const Property& rhs) {
      return stream << rhs.ToString();
    }

    friend std::ostream& operator<<(std::ostream& stream, const Property* rhs) {
      return stream << (*rhs);
    }
  };

  template<typename T>
  class PropertyTemplate : public Property {
  protected:
    std::string name_;
    T value_;

    PropertyTemplate(const std::string& name,
                     const T& value):
      Property(),
      name_(name),
      value_(value) {
    }
  public:
    ~PropertyTemplate() override = default;

    const std::string& GetName() const override {
      return name_;
    }

    const T& GetValue() const {
      return value_;
    }

    operator T() const {
      return GetValue();
    }
  };

#define DECLARE_PROPERTY_TYPE(Name)                         \
  public:                                                   \
    std::string ToString() const override;                  \
    Name##Property* As##Name() override { return this; }

  class StringProperty : public PropertyTemplate<std::string> {
  protected:
    StringProperty(const std::string& name,
                   const std::string& value):
      PropertyTemplate(name, value) {
    }
  public:
    ~StringProperty() override = default;

    DECLARE_PROPERTY_TYPE(String);
  public:
    static inline StringProperty*
    New(const std::string& name, const std::string& value) {
      return new StringProperty(name, value);
    }
  };

  class LongProperty : public PropertyTemplate<uint64_t> {
  protected:
    LongProperty(const std::string& name, const uint64_t value):
      PropertyTemplate(name, value) {
    }
  public:
    ~LongProperty() override = default;

    DECLARE_PROPERTY_TYPE(Long);
  public:
    static inline LongProperty*
    New(const std::string& name, const uint64_t value) {
      return new LongProperty(name, value);
    }
  };

  class DoubleProperty : public PropertyTemplate<double> {
  protected:
    DoubleProperty(const std::string& name,
                   const double value):
      PropertyTemplate(name, value) {
    }
  public:
    ~DoubleProperty() override = default;

    DECLARE_PROPERTY_TYPE(Double);
  public:
    static inline DoubleProperty*
    New(const std::string& name, const double value) {
      return new DoubleProperty(name, value);
    }
  };

  class BoolProperty : public PropertyTemplate<bool> {
  protected:
    BoolProperty(const std::string& name, const bool value):
      PropertyTemplate(name, value) {
    }
  public:
    ~BoolProperty() override = default;

    DECLARE_PROPERTY_TYPE(Bool);
  public:
    static inline BoolProperty*
    New(const std::string& name, const bool value) {
      return new BoolProperty(name, value);
    }

    static inline BoolProperty*
    True(const std::string& name) {
      return New(name, true);
    }

    static inline BoolProperty*
    False(const std::string& name) {
      return New(name, false);
    }
  };
#undef DECLARE_PROPERTY_TYPE
}

#endif //PRT_PROPERTY_H