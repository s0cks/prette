#include "prette/vbo/vbo_builder.h"

namespace prt::vbo {
  class VboAttributeBinder : public AttributeVisitor {
  private:
    uword stride_;
    uword offset_;

    inline void Bind(Attribute* attr) {
      glVertexAttribPointer(attr->GetIndex(), attr->GetSize(), attr->GetType()->GetGlType(), attr->IsNormalized(), GetStride(), (const GLvoid*) offset_);
      CHECK_GL(FATAL);
    }

    inline void Enable(Attribute* attr) {
      glEnableVertexAttribArray(attr->GetIndex());
      CHECK_GL(FATAL);
    }
  public:
    explicit VboAttributeBinder(const uword stride):
      AttributeVisitor(),
      stride_(stride),
      offset_(0) {
    }
    ~VboAttributeBinder() override = default;

    uword GetStride() const {
      return stride_;
    }

    bool VisitAttribute(Attribute* attr) override {
      PRT_ASSERT(attr);
      DLOG(INFO) << "binding " << attr->ToString() << ".....";
      Bind(attr);
      Enable(attr);
      offset_ += attr->GetAllocationSize();
      return true;
    }
  public:
    static inline void
    BindAll(Class* cls) {
      PRT_ASSERT(cls);
      const auto stride = cls->GetAllocationSize();
      VboAttributeBinder binder(stride);
      LOG_IF(ERROR, !cls->VisitAllAttributes(&binder)) << "failed to bind all attributes for: " << cls->ToString();
    }
  };

  Vbo* VboBuilderBase::Build() const {
    const auto source = GetSource();
    PRT_ASSERT(source.GetStartingAddress() >= 0);
    PRT_ASSERT(!source.IsEmpty());

    const auto id = GenerateVboId();
    PRT_ASSERT(IsValidVboId(id));
    Vbo::BindVbo(id);
    VboAttributeBinder::BindAll(GetClass());
    Vbo::InitData(source, GetUsage());
    Vbo::Unbind();
    return Vbo::New(id, GetClass(), GetLength(), GetUsage());
  }
}