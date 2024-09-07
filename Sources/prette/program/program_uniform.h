#ifndef PRT_PROGRAM_UNIFORM_H
#define PRT_PROGRAM_UNIFORM_H

#include <fmt/format.h>
#include "prette/gfx.h"
#include "prette/common.h"
#include "prette/program/program_id.h"

namespace prt::program {
  class ProgramUniform {
    DEFINE_DEFAULT_COPYABLE_TYPE(ProgramUniform);
  private:
    uword index_{};
    GLenum type_{};
    uword size_{};
    std::string name_{};
  public:
    ProgramUniform() = default;
    ProgramUniform(const GLint index,
                   const GLenum type,
                   const GLint size,
                   const char* name,
                   const int name_length):
      index_(static_cast<uword>(index)),
      type_(type),
      size_(static_cast<uword>(size)),
      name_(name, name_length) {
    }
    ~ProgramUniform() = default;

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

    auto operator==(const ProgramUniform& rhs) const -> bool {
      return index_ == rhs.index_
          && type_ == rhs.type_
          && size_ == rhs.size_
          && name_ == rhs.name_;
    }

    auto operator!=(const ProgramUniform& rhs) const -> bool {
      return index_ != rhs.index_
          || type_ != rhs.type_
          || size_ != rhs.size_
          || name_ != rhs.name_;
    }

    auto operator<(const ProgramUniform& rhs) const -> bool {
      return index_ < rhs.index_;
    }

    auto operator>(const ProgramUniform& rhs) const -> bool {
      return index_ > rhs.index_;
    }

    friend auto operator<<(std::ostream& stream, const ProgramUniform& rhs) -> std::ostream& {
      stream << "ProgramUniform(";
      stream << "index=" << rhs.GetIndex() << ", ";
      stream << "type=" << rhs.GetType() << ", ";
      stream << "size=" << rhs.GetSize() << ", ";
      stream << "name=" << rhs.GetName();
      stream << ")";
      return stream;
    }
  };

  class UniformVisitor {
    DEFINE_NON_COPYABLE_TYPE(UniformVisitor);
  protected:
    UniformVisitor() = default;
  public:
    virtual ~UniformVisitor() = default;
    virtual auto VisitUniform(const ProgramUniform& uniform) -> bool = 0;
  };

  class Program;
  class UniformIterator {
    DEFINE_NON_COPYABLE_TYPE(UniformIterator);
  private:
    const Program* program_;
    uword cur_uniform_;
    uword num_uniforms_;
    GLint length_;
    GLint size_;
    GLenum type_;
    uword name_asize_;
    char* name_;
  public:
    explicit UniformIterator(const Program* program);
    ~UniformIterator();

    auto GetProgram() const -> const Program* {
      return program_;
    }

    auto GetProgramId() const -> ProgramId;

    auto HasNext() const -> bool;
    auto Next() -> ProgramUniform;
  };
}

namespace fmt {
  using prt::program::ProgramUniform;

  template<>
  struct formatter<ProgramUniform> : formatter<std::string_view> {
    auto format(const ProgramUniform& uniform, format_context& ctx) -> decltype(ctx.out()) const {
      return formatter<std::string_view>::format(uniform.GetName(), ctx);
    }
  };
}

#endif //PRT_PROGRAM_UNIFORM_H