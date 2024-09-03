#ifndef PRT_CLASS_H
#define PRT_CLASS_H

#include <set>
#include <unordered_map>
#include "prette/gfx.h"
#include "prette/platform.h"

namespace prt {
  class Class;
  class Attribute;
  class AttributeVisitor {
  protected:
    AttributeVisitor() = default;
  public:
    virtual ~AttributeVisitor() = default;
    virtual bool VisitAttribute(Attribute* attr) = 0;
  };

  class Attribute {
    friend class Class;
  public:
    struct IndexComparator {
      bool operator()(Attribute* lhs, Attribute* rhs) const {
        return lhs->GetIndex() < rhs->GetIndex();
      }
    };
  protected:
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

    Class* GetOwner() const {
      return owner_;
    }

    const std::string& GetName() const {
      return name_;
    }

    uword GetIndex() const {
      return index_;
    }

    uword GetSize() const {
      return size_;
    }

    Class* GetType() const {
      return type_;
    }

    uword GetAllocationSize() const;

    bool IsNormalized() const {
      return normalized_;
    }

    bool Accept(AttributeVisitor* vis) {
      PRT_ASSERT(vis);
      return vis->VisitAttribute(this);
    }

    int Compare(Attribute* rhs) const;
    bool Equals(Attribute* rhs) const;
    std::string ToString() const;
  };

  typedef std::set<Attribute*, Attribute::IndexComparator> AttributeSet;

  static inline std::ostream&
  operator<<(std::ostream& stream, const AttributeSet& rhs) {
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
  private:
    google::LogSeverity severity_;
  public:
    explicit AttributePrinter(const google::LogSeverity severity = google::INFO):
      AttributeVisitor(),
      severity_(severity) {
    }
    ~AttributePrinter() override = default;

    google::LogSeverity GetSeverity() const {
      return severity_;
    }

    bool VisitAttribute(Attribute* attr) override {
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
  protected:
    ClassVisitor() = default;
  public:
    virtual ~ClassVisitor() = default;
    virtual bool VisitClass(Class* cls) = 0;
  };

  class Class {
  protected:
    ClassId id_;
    std::string name_;
    AttributeSet attrs_;

    Class(const ClassId id,
          const std::string& name);

    uword CalculateAllocationSize() const;
  public:
    ~Class();

    ClassId GetId() const {
      return id_;
    }

    const std::string& GetName() const {
      return name_;
    }

    const AttributeSet& GetAttributes() const {
      return attrs_;
    }

    bool Accept(ClassVisitor* vis) {
      PRT_ASSERT(vis);
      return vis->VisitClass(this);
    }

    Attribute* CreateAttribute(const std::string& name,
                               Class* type,
                               const uword size,
                               const bool normalized);
    
    inline Attribute* CreateByteAttr(const std::string& name,
                                     const uword size,
                                     const bool normalized) {
      return CreateAttribute(name, Class::kByte, size, normalized);
    }

    inline Attribute* CreateUByteAttr(const std::string& name,
                                      const uword size,
                                      const bool normalized) {
      return CreateAttribute(name, Class::kUnsignedByte, size, normalized);
    }

    inline Attribute* CreateShortAttr(const std::string& name,
                                      const uword size,
                                      const bool normalized = false) {
      return CreateAttribute(name, Class::kShort, size, normalized);
    }

    inline Attribute* CreateUShortAttr(const std::string& name,
                                       const uword size,
                                       const bool normalized = false) {
      return CreateAttribute(name, Class::kUnsignedShort, size, normalized);
    }

    inline Attribute* CreateIntAttr(const std::string& name,
                                    const uword size,
                                    const bool normalized = false) {
      return CreateAttribute(name, Class::kInt, size, normalized);
    }

    inline Attribute* CreateUIntAttr(const std::string& name,
                                     const uword size,
                                     const bool normalized = false) {
      return CreateAttribute(name, Class::kUnsignedInt, size, normalized);
    }

    inline Attribute* CreateFloatAttr(const std::string& name,
                                      const uword size,
                                      const bool normalized = false) {
      return CreateAttribute(name, Class::kFloat, size, normalized);
    }

    inline Attribute* CreateVec2fAttr(const std::string& name,
                                      const bool normalized = false) {
      return CreateFloatAttr(name, 2, normalized);
    }

    inline Attribute* CreateVec3fAttr(const std::string& name,
                                      const bool normalized = false) {
      return CreateFloatAttr(name, 3, normalized);
    }

    inline Attribute* CreateColorAttr(const std::string& name) {
      return CreateUByteAttr(name, 4, true);
    }

    GLenum GetGlType() const;
    uword GetAllocationSize() const;
    bool IsPrimative() const;
    bool Equals(const Class* rhs) const;
    bool VisitAllAttributes(AttributeVisitor* vis) const;
    std::string ToString() const;

#ifdef PRT_DEBUG
    bool PrintAttributes(const google::LogSeverity severity = google::INFO) const;
#endif //PRT_DEBUG
  private:
    static inline Class* New(const ClassId id, const std::string& name) {
      return new Class(id, name);
    }
  public:
    static void Init();
    static Class* Find(const std::string& name);
    static Class* New(const std::string& name);

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

  typedef std::unordered_map<std::string, Class*> ClassMap;
}

#endif //PRT_CLASS_H