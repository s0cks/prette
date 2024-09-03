#ifndef PRT_PROGRAM_SCOPE_H
#define PRT_PROGRAM_SCOPE_H

#include "prette/ubo/ubo.h"
#include "prette/program/program_id.h"

namespace prt::program {
  class Program;
  class ProgramScope {
  protected:
    Program* program_;

    explicit ProgramScope(Program* program):
      program_(program) {
    }
  public:
    virtual ~ProgramScope() = default;

    Program* GetProgram() const {
      return program_;
    }

    ProgramId GetProgramId() const;
  };

  class ProgramUboBindScope : public ProgramScope {
  public:
    struct BoundUbo {
      GLint index;
      std::string name;
      Ubo* ubo;

      friend std::ostream& operator<<(std::ostream& stream, const BoundUbo& rhs) {
        stream << "BoundUbo(";
        stream << "index=" << rhs.index << ", ";
        stream << "name=" << rhs.name << ", ";
        stream << "ubo=" << rhs.ubo->ToString();
        stream << ")";
        return stream;
      }

      bool operator==(const BoundUbo& rhs) const {
        return index == rhs.index;
      }

      bool operator!=(const BoundUbo& rhs) const {
        return index != rhs.index;
      }

      bool operator<(const BoundUbo& rhs) const {
        return index < rhs.index;
      }

      bool operator>(const BoundUbo& rhs) const {
        return index > rhs.index;
      }
    };
  protected:
    std::set<BoundUbo> bound_;
    GLint index_;
  public:
    explicit ProgramUboBindScope(Program* program):
      ProgramScope(program),
      bound_(),
      index_(0) {
    }
    ~ProgramUboBindScope() override = default;
    void Bind(const std::string& name, Ubo* ubo);
  };
}

#include "prette/program/program_scope_link.h"
#include "prette/program/program_scope_apply.h"

#endif //PRT_PROGRAM_SCOPE_H