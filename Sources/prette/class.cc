#include "prette/class.h"
#include "prette/relaxed_atomic.h"

namespace prt {
  static ClassMap all_;
  static RelaxedAtomic<uword> current_id_(kTotalNumberOfPrimativeClassIds + 1);

  static inline void
  Register(Class* cls) {
    PRT_ASSERT(cls);
    const auto [iter,success] = all_.insert({ cls->GetName(), cls });
    LOG_IF(ERROR, !success) << "failed to register: " << cls->ToString();
  }

  static inline void
  Deregister(Class* cls) {
    PRT_ASSERT(cls);
    const auto removed = all_.erase(cls->GetName());
    LOG_IF(ERROR, removed != 1) << "failed to deregister: " << cls->ToString();
  }

  static inline auto
  GetNextClassId() -> ClassId {
    return static_cast<ClassId>(current_id_ += 1);
  }

  Attribute::Attribute(Class* owner,
                       const std::string& name,
                       const uword index,
                       const uword size,
                       Class* type,
                       const bool normalized):
    owner_(owner),
    name_(name),
    index_(index),
    size_(size),
    type_(type),
    normalized_(normalized) {
    PRT_ASSERT(owner);
    PRT_ASSERT(type);
  }

  Attribute::~Attribute() {
    // do nothing 
  }

  auto Attribute::GetAllocationSize() const -> uword {
    return GetSize() * GetType()->GetAllocationSize();
  }

  auto Attribute::Compare(Attribute* rhs) const -> int {
    if(GetIndex() < rhs->GetIndex()) {
      return -1;
    } else if(GetIndex() > rhs->GetIndex()) {
      return +1;
    }

    if(GetSize() < rhs->GetSize()) {
      return -1;
    } else if(GetSize() > rhs->GetSize()) {
      return +1;
    }

    return 0;
  }

  auto Attribute::Equals(Attribute* rhs) const -> bool {
    return GetIndex() == rhs->GetIndex()
        && GetSize() == rhs->GetSize()
        && GetType() == rhs->GetType()
        && IsNormalized() == rhs->IsNormalized()
        && GetName() == rhs->GetName()
        && GetOwner()->Equals(rhs->GetOwner());
  }

  auto Attribute::ToString() const -> std::string {
    using units::data::byte_t;
    std::stringstream ss;
    ss << "vertex::Attribute(";
    ss << "owner=" << GetOwner()->ToString() << ", ";
    ss << "name=" << GetName() << ", ";
    ss << "index=" << GetIndex() << ", ";
    ss << "size=" << byte_t(GetSize()) << ", ";
    ss << "type=" << GetType() << ", ";
    ss << "normalized=" << (IsNormalized() ? 'y' : 'n');
    ss << ")";
    return ss.str();
  }

  Class::Class(const ClassId id,
               const std::string& name):
    id_(id),
    name_(name) {
    Register(this);
  }

  Class::~Class() {
    Deregister(this);
  }

  auto Class::CreateAttribute(const std::string& name,
                                    Class* type,
                                    const uword size,
                                    const bool normalized) -> Attribute* {
    const auto index = attrs_.size();
    const auto attr = new Attribute(this, name, index, size, type, normalized);
    PRT_ASSERT(attr);
    const auto [iter,success] = attrs_.insert(attr);
    LOG_IF(ERROR, !success) << "failed to register " << attr->ToString() << " in " << ToString();
    return attr;
  }

  auto Class::CalculateAllocationSize() const -> uword {
    PRT_ASSERT(!IsPrimative());
    uword total_size = 0;
    for(const auto& attr : attrs_)
      total_size += attr->GetAllocationSize();
    return total_size;
  }

  auto Class::GetAllocationSize() const -> uword {
    switch(GetId()) {
      case kByteClassId:
      case kUnsignedByteClassId:
        return sizeof(int8_t);
      case kShortClassId:
      case kUnsignedShortClassId:
        return sizeof(int16_t);
      case kIntClassId:
      case kUnsignedIntClassId:
        return sizeof(int32_t);
      case kFloatClassId:
        return sizeof(float);
      default:
        return CalculateAllocationSize();
    }
  }

  auto Class::GetGlType() const -> GLenum {
    switch(GetId()) {
      case kByteClassId:
        return GL_BYTE;
      case kUnsignedByteClassId:
        return GL_UNSIGNED_BYTE;
      case kShortClassId:
        return GL_SHORT;
      case kUnsignedShortClassId:
        return GL_UNSIGNED_SHORT;
      case kIntClassId:
        return GL_INT;
      case kUnsignedIntClassId:
        return GL_UNSIGNED_INT;
      case kFloatClassId:
        return GL_FLOAT;
      default:
        return GL_NONE;
    }
  }

  auto Class::IsPrimative() const -> bool {
    switch(GetId()) {
      default:
        return false;
    }
  }

  auto Class::Equals(const Class* rhs) const -> bool {
    return rhs
        && GetId() == rhs->GetId();
  }

  auto Class::VisitAllAttributes(AttributeVisitor* vis) const -> bool {
    PRT_ASSERT(vis);
    for(const auto& attr : attrs_) {
      if(!attr->Accept(vis))
        return false;
    }
    return true;
  }

  auto Class::ToString() const -> std::string {
    std::stringstream ss;
    ss << "Class(";
    ss << "id=" << GetId() << ", ";
    ss << "name=" << GetName();
    ss << ")";
    return ss.str();
  }

  // byte (8 bits)
  Class* Class::kUnsignedByte = nullptr;
  Class* Class::kByte = nullptr;
  // short (16 bits)
  Class* Class::kUnsignedShort = nullptr;
  Class* Class::kShort = nullptr;
  // int (32 bits)
  Class* Class::kUnsignedInt = nullptr;
  Class* Class::kInt = nullptr;
  // float (32 bits)
  Class* Class::kFloat = nullptr;

  void Class::Init() {
    // byte (8 bits)
    kUnsignedByte = New(kUnsignedByteClassId, "UnsignedByte");
    kByte = New(kByteClassId, "Byte");
    // short (16 bits)
    kUnsignedShort = New(kUnsignedShortClassId, "UnsignedShort");
    kShort = New(kShortClassId, "Short");
    // int (32 bits)
    kUnsignedInt = New(kUnsignedIntClassId, "UnsignedInt");
    kInt = New(kIntClassId, "Int");
    // float (32 bits)
    kFloat = New(kFloatClassId, "Float");
  }

  auto Class::New(const std::string& name) -> Class* {
    return New(GetNextClassId(), name);
  }
}