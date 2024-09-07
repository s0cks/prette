#ifndef PRT_PROGRAM_STATUS_H
#define PRT_PROGRAM_STATUS_H

#include "prette/program/program_id.h"

namespace prt::program {
  class ProgramInfoLog { //TODO: extract & cleanup
  private:
    ProgramId id_;
    uint8_t* data_;
    uint32_t length_;
  protected:
    inline uint8_t* data() const {
      return data_;
    }

    void DeleteData() {
      if(IsEmpty())
        return;
      free(data_);
      data_ = nullptr;
      length_ = 0;
    }

    void CopyFrom(const uint8_t* src, const uint32_t length) {

    }

    inline void
    CopyFrom(const ProgramInfoLog& rhs) {
      if(!IsEmpty())
        DeleteData();
      CopyFrom(rhs.data(), rhs.length());
    }

    static void GetProgramInfoLog(const ProgramId id, uint8_t** data, uint32_t* length);
  public:
    explicit ProgramInfoLog(const ProgramId id = kInvalidProgramId):
      id_(id),
      data_(nullptr),
      length_(0) {
      if(id)
        GetProgramInfoLog(id, &data_, &length_); 
    }
    ProgramInfoLog(const ProgramInfoLog& rhs):
      id_(rhs.id()),
      data_(nullptr),
      length_(0) {
      if(!rhs.IsEmpty())
        CopyFrom(rhs);
    }
    virtual ~ProgramInfoLog() = default;

    ProgramId id() const {
      return id_;
    }

    uint32_t length() const {
      return length_;
    }

    std::string str() const {
      return std::string((const char*) data(), length());
    }

    bool IsEmpty() const {
      return data() == nullptr
          && length() == 0;
    }

    operator std::string() const {
      return str();
    }

    void operator=(const ProgramInfoLog& rhs) {
      if(rhs.IsEmpty())
        return;
      CopyFrom(rhs);
    }

    friend std::ostream& operator<<(std::ostream& stream, const ProgramInfoLog& rhs) {
      return stream << rhs.str();
    }
  };

#define FOR_EACH_PROGRAM_STATUS(V) \
  V(Link, GL_LINK_STATUS)          \
  V(Delete, GL_DELETE_STATUS)      \
  V(Validate, GL_VALIDATE_STATUS)

  class ProgramStatus {
    friend class Program;
  public:
    enum Type : GLenum {
#define DEFINE_STATUS(Name, GlValue) k##Name##Status = (GlValue),
    FOR_EACH_PROGRAM_STATUS(DEFINE_STATUS)
#undef DEFINE_STATUS
    };

    friend std::ostream& operator<<(std::ostream& stream, const Type& rhs) {
      switch(rhs) {
#define DEFINE_TO_STRING(Name, GlValue)                                                   \
        case Type::k##Name##Status: return stream << #Name << "(" << (#GlValue) << ")";
        FOR_EACH_PROGRAM_STATUS(DEFINE_TO_STRING)
#undef DEFINE_TO_STRING
        default:
          return stream << "unknown";
      }
    }
  protected:
    ProgramId id_;
    ProgramInfoLog info_;

    explicit ProgramStatus(const ProgramId id):
      id_(id),
      info_(id) {
    }
    ProgramStatus() = default;
    ProgramStatus(const ProgramStatus& rhs) = default;
  public:
    virtual ~ProgramStatus() = default;

    ProgramId id() const {
      return id_;
    }

    const ProgramInfoLog& info() const {
      return info_;
    }
  };

  template<const ProgramStatus::Type StatusType>
  class ProgramStatusTemplate : public ProgramStatus {
    DEFINE_DEFAULT_COPYABLE_TYPE(ProgramStatusTemplate<StatusType>);
  private:
    GLint status_{};
  public:
    ProgramStatusTemplate() = default;
    explicit ProgramStatusTemplate(const ProgramId id):
      ProgramStatus(id) {
      glGetProgramiv(id, StatusType, &status_);
      CHECK_GL;
    }
    ~ProgramStatusTemplate() override = default;

    auto status() const -> GLint {
      return status_;
    }

    auto type() const -> Type {
      return StatusType;
    }
  };

#define DEFINE_PROGRAM_STATUS(Name, GlValue)                                                            \
  class Program##Name##Status : public ProgramStatusTemplate<ProgramStatus::k##Name##Status> {          \
    DEFINE_DEFAULT_COPYABLE_TYPE(Program##Name##Status);                                                \
  public:                                                                                               \
    explicit Program##Name##Status(const ProgramId id = kInvalidProgramId):                             \
      ProgramStatusTemplate<ProgramStatus::k##Name##Status>(id) {                                       \
    }                                                                                                   \
    ~Program##Name##Status() = default;                                                                 \
    auto Is##Name() const -> bool {                                                                     \
      return status() == GL_TRUE;                                                                       \
    }                                                                                                   \
    operator bool () const {                                                                            \
      return Is##Name();                                                                                \
    }                                                                                                   \
    friend auto operator<<(std::ostream& stream, const Program##Name##Status& rhs) -> std::ostream& {   \
      stream << "Program" << #Name << "Status(";                                                        \
      stream << "id=" << rhs.id() << ", ";                                                              \
      stream << "status=" << (rhs.Is##Name() ? 'y' : 'n');                                              \
      const auto& info = rhs.info();                                                                    \
      if(!info.IsEmpty())                                                                               \
        stream << ", info=" << (const std::string&) info;                                               \
      stream << ")";                                                                                    \
      return stream;                                                                                    \
    }                                                                                                   \
  };                                                                         
  FOR_EACH_PROGRAM_STATUS(DEFINE_PROGRAM_STATUS)
#undef DEFINE_PROGRAM_STATUS
}

#endif //PRT_PROGRAM_STATUS_H