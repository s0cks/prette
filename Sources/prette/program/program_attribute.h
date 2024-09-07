#ifndef PRT_PROGRAM_ATTRIBUTE_H
#define PRT_PROGRAM_ATTRIBUTE_H

#include <fmt/format.h>
#include "prette/program/program_id.h"

namespace prt::program {
  class ProgramAttribute {
    friend class AttributeIterator;
    DEFINE_DEFAULT_COPYABLE_TYPE(ProgramAttribute);
  private:
    GLint index_{};
    GLenum type_{};
    GLint size_{};
    std::string name_{};
  public:
    ProgramAttribute() = default;
    ProgramAttribute(const GLint index,
                     const GLenum type,
                     const GLint size,
                     const char* name,
                     const GLint name_length):
      index_(index),
      type_(type),
      size_(size),
      name_(name, name_length) {
    }
    ~ProgramAttribute() = default;

    auto GetIndex() const -> uword {
      return index_;
    }

    auto GetType() const -> GLenum {
      return type_;
    }

    auto GetSize() const -> uword {
      return size_;
    }

    auto GetName() const -> const std::string& {
      return name_;
    }

    auto operator==(const ProgramAttribute& rhs) const -> bool {
      return index_ == rhs.index_
          && type_ == rhs.type_
          && size_ == rhs.size_
          && name_ == rhs.name_;
    }

    auto operator!=(const ProgramAttribute& rhs) const -> bool {
      return index_ != rhs.index_
          || type_ != rhs.type_
          || size_ != rhs.size_
          || name_ != rhs.name_;
    }

    auto operator<(const ProgramAttribute& rhs) const -> bool {
      return index_ < rhs.index_;
    }

    auto operator>(const ProgramAttribute& rhs) const -> bool {
      return index_ > rhs.index_;
    }

    friend auto operator<<(std::ostream& stream, const ProgramAttribute& rhs) -> std::ostream& {
      stream << "ProgramAttribute(";
      stream << "index=" << rhs.GetIndex() << ", ";
      stream << "type=" << rhs.GetType() << ", ";
      stream << "size=" << rhs.GetSize() << ", ";
      stream << "name=" << rhs.GetName();
      stream << ")";
      return stream;
    }
  };

  class AttributeVisitor {
    DEFINE_NON_COPYABLE_TYPE(AttributeVisitor);
  protected:
    AttributeVisitor() = default;
  public:
    virtual ~AttributeVisitor() = default;
    virtual auto VisitAttribute(const ProgramAttribute& attr) -> bool = 0;
  };

  class Program;
  class AttributeIterator {
    DEFINE_NON_COPYABLE_TYPE(AttributeIterator);
  private:
    const Program* program_;
    uword num_attrs_{};
    GLint index_{};
    GLenum type_{};
    GLsizei length_{};
    GLint size_{};
    std::vector<GLchar> name_;
  public:
    explicit AttributeIterator(const Program* program);
    ~AttributeIterator() = default;

    auto GetProgram() const -> const Program* {
      return program_;
    }

    auto GetProgramId() const -> ProgramId;
    auto HasNext() const -> bool;
    auto Next() -> ProgramAttribute;
  };
}

namespace fmt {
  using prt::program::ProgramAttribute;

  template<>
  struct formatter<ProgramAttribute> : formatter<std::string_view> {
    auto format(const ProgramAttribute& uniform, format_context& ctx) -> decltype(ctx.out()) const {
      return formatter<std::string_view>::format(uniform.GetName(), ctx);
    }
  };
}


#endif //PRT_PROGRAM_ATTRIBUTE_H