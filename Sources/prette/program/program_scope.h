#ifndef PRT_PROGRAM_SCOPE_H
#define PRT_PROGRAM_SCOPE_H

#include "prette/ubo/ubo.h"
#include "prette/program/program_id.h"

namespace prt::program {
  class Program;
  class ProgramScope {
    DEFINE_NON_COPYABLE_TYPE(ProgramScope);
  private:
    Program* program_;
  protected:
    explicit ProgramScope(Program* program):
      program_(program) {
    }
  public:
    virtual ~ProgramScope() = default;

    auto GetProgram() const -> Program* {
      return program_;
    }

    auto GetProgramId() const -> ProgramId;
  };

  class ProgramUboBindScope : public ProgramScope {
    DEFINE_NON_COPYABLE_TYPE(ProgramUboBindScope);
  public:
    struct BoundUbo {
      GLint index;
      std::string name;
      Ubo* ubo;

      friend auto operator<<(std::ostream& stream, const BoundUbo& rhs) -> std::ostream& {
        stream << "BoundUbo(";
        stream << "index=" << rhs.index << ", ";
        stream << "name=" << rhs.name << ", ";
        stream << "ubo=" << rhs.ubo->ToString();
        stream << ")";
        return stream;
      }

      auto operator==(const BoundUbo& rhs) const -> bool {
        return index == rhs.index;
      }

      auto operator!=(const BoundUbo& rhs) const -> bool {
        return index != rhs.index;
      }

      auto operator<(const BoundUbo& rhs) const -> bool {
        return index < rhs.index;
      }

      auto operator>(const BoundUbo& rhs) const -> bool {
        return index > rhs.index;
      }
    };
  private:
    std::set<BoundUbo> bound_;
    GLint index_{};
  public:
    explicit ProgramUboBindScope(Program* program):
      ProgramScope(program),
      bound_() {
    }
    ~ProgramUboBindScope() override = default;
    void Bind(const std::string& name, Ubo* ubo);
  };
}

#include "prette/program/program_scope_link.h"
#include "prette/program/program_scope_apply.h"

#endif //PRT_PROGRAM_SCOPE_H