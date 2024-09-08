#ifndef PRT_CLASS_H
#define PRT_CLASS_H

#include <set>
#include <unordered_map>
#include "prette/gfx.h"
#include "prette/common.h"

namespace prt {
  class Class;
  class Attribute;
  class AttributeVisitor {
    DEFINE_NON_COPYABLE_TYPE(AttributeVisitor);
  protected:
    AttributeVisitor() = default;
  public:
    virtual ~AttributeVisitor() = default;
    virtual auto VisitAttribute(Attribute* attr) -> bool = 0;
  };

  class Attribute {
    friend class Class;
    DEFINE_NON_COPYABLE_TYPE(Attribute);
  public:
    struct IndexComparator {
      auto operator()(Attribute* lhs, Attribute* rhs) const -> bool {
        return lhs->GetIndex() < rhs->GetIndex();
      }
    };
  private:
    Class* owner_;
    std::string name_;
    uword index_;
    uword size_;
    Class* type_;
    bool normalized_;
  public:
    Attribute(Class* owner,
              const std::string& name,
              const uword index,
              const uword size,
              Class* type,
              const bool normalized);
    ~Attribute();

    auto GetOwner() const -> Class* {
      return owner_;
    }

    auto GetName() const -> const std::string& {
      return name_;
    }

    auto GetIndex() const -> uword {
      return index_;
    }

    auto GetSize() const -> uword {
      return size_;
    }

    auto GetType() const -> Class* {
      return type_;
    }

    auto GetAllocationSize() const -> uword;

    auto IsNormalized() const -> bool {
      return normalized_;
    }

    auto Accept(AttributeVisitor* vis) -> bool {
      PRT_ASSERT(vis);
      return vis->VisitAttribute(this);
    }

    auto Compare(Attribute* rhs) const -> int;
    auto Equals(Attribute* rhs) const -> bool;
    auto ToString() const -> std::string;
  };

  using AttributeSet = std::set<Attribute *, Attribute::IndexComparator>;

  static inline auto
  operator<<(std::ostream& stream, const AttributeSet& rhs) -> std::ostream& {
    stream << "[";
    auto idx = 0;
    for(const auto& attr : rhs) {
      if(idx++ > 0)
        stream << ", ";
      stream << attr;
    }
    stream << "]";
    return stream;
  }

#ifdef PRT_DEBUG
  class AttributePrinter : public AttributeVisitor {
    DEFINE_NON_COPYABLE_TYPE(AttributePrinter);
  private:
    google::LogSeverity severity_;
  public:
    explicit AttributePrinter(const google::LogSeverity severity = google::INFO):
      AttributeVisitor(),
      severity_(severity) {
    }
    ~AttributePrinter() override = default;

    auto GetSeverity() const -> google::LogSeverity {
      return severity_;
    }

    auto VisitAttribute(Attribute* attr) -> bool override {
      PRT_ASSERT(attr);
      LOG_AT_LEVEL(GetSeverity()) << " - " << attr->ToString();
      return true;
    }
  };
#endif //PRT_DEBUG

  enum ClassId : uword {
    // byte (8 bits)
    kByteClassId                        = 0,
    kUnsignedByteClassId                = 1,
    // short (16 bits)
    kShortClassId                       = 2,
    kUnsignedShortClassId               = 3,
    // int (32 bits)
    kIntClassId                         = 4,
    kUnsignedIntClassId                 = 5,
    // float (32 bits)
    kFloatClassId                       = 6,
    // aliases
    kTotalNumberOfPrimativeClassIds,
  };

  class ClassVisitor {
    DEFINE_NON_COPYABLE_TYPE(ClassVisitor);
  protected:
    ClassVisitor() = default;
  public:
    virtual ~ClassVisitor() = default;
    virtual auto VisitClass(Class* cls) -> bool = 0;
  };

  class Class {
    DEFINE_NON_COPYABLE_TYPE(Class);
  private:
    ClassId id_;
    std::string name_;
    AttributeSet attrs_;

    Class(const ClassId id,
          const std::string& name);

    auto CalculateAllocationSize() const -> uword;
  public:
    ~Class();

    auto GetId() const -> ClassId {
      return id_;
    }

    auto GetName() const -> const std::string& {
      return name_;
    }

    auto GetAttributes() const -> const AttributeSet& {
      return attrs_;
    }

    auto Accept(ClassVisitor* vis) -> bool {
      PRT_ASSERT(vis);
      return vis->VisitClass(this);
    }

    auto CreateAttribute(const std::string& name,
                               Class* type,
                               const uword size,
                               const bool normalized) -> Attribute*;
    
    inline auto CreateByteAttr(const std::string& name,
                                     const uword size,
                                     const bool normalized) -> Attribute* {
      return CreateAttribute(name, Class::kByte, size, normalized);
    }

    inline auto CreateUByteAttr(const std::string& name,
                                      const uword size,
                                      const bool normalized) -> Attribute* {
      return CreateAttribute(name, Class::kUnsignedByte, size, normalized);
    }

    inline auto CreateShortAttr(const std::string& name,
                                      const uword size,
                                      const bool normalized = false) -> Attribute* {
      return CreateAttribute(name, Class::kShort, size, normalized);
    }

    inline auto CreateUShortAttr(const std::string& name,
                                       const uword size,
                                       const bool normalized = false) -> Attribute* {
      return CreateAttribute(name, Class::kUnsignedShort, size, normalized);
    }

    inline auto CreateIntAttr(const std::string& name,
                                    const uword size,
                                    const bool normalized = false) -> Attribute* {
      return CreateAttribute(name, Class::kInt, size, normalized);
    }

    inline auto CreateUIntAttr(const std::string& name,
                                     const uword size,
                                     const bool normalized = false) -> Attribute* {
      return CreateAttribute(name, Class::kUnsignedInt, size, normalized);
    }

    inline auto CreateFloatAttr(const std::string& name,
                                      const uword size,
                                      const bool normalized = false) -> Attribute* {
      return CreateAttribute(name, Class::kFloat, size, normalized);
    }

    inline auto CreateVec2fAttr(const std::string& name,
                                      const bool normalized = false) -> Attribute* {
      return CreateFloatAttr(name, 2, normalized);
    }

    inline auto CreateVec3fAttr(const std::string& name,
                                      const bool normalized = false) -> Attribute* {
      return CreateFloatAttr(name, 3, normalized);
    }

    inline auto CreateColorAttr(const std::string& name) -> Attribute* {
      return CreateUByteAttr(name, 4, true);
    }

    auto GetAllocationSize() const -> uword;
    auto IsPrimative() const -> bool;
    auto Equals(const Class* rhs) const -> bool;
    auto VisitAllAttributes(AttributeVisitor* vis) const -> bool;
    auto ToString() const -> std::string;

#ifdef PRT_DEBUG
    auto PrintAttributes(const google::LogSeverity severity = google::INFO) const -> bool;
#endif //PRT_DEBUG
  private:
    static inline auto
    New(const ClassId id, const std::string& name) -> Class* {
      return new Class(id, name);
    }
  public:
    static void Init();
    static auto Find(const std::string& name) -> Class*;
    static auto New(const std::string& name) -> Class*;

    // byte (8 bits)
    static Class* kUnsignedByte;
    static Class* kByte;
    // short (16 bits)
    static Class* kUnsignedShort;
    static Class* kShort;
    // int (32 bits)
    static Class* kUnsignedInt;
    static Class* kInt;
    // float (32 bits)
    static Class* kFloat;
  };

  using ClassMap = std::unordered_map<std::string, Class*>;
}

#endif //PRT_CLASS_H