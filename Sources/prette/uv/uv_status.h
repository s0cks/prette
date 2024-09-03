#ifndef PRT_UV_STATUS_H
#define PRT_UV_STATUS_H

#include <uv.h>

namespace prt::uv {
#ifndef UV_OK
#define UV_OK 0
#endif //UV_OK

  typedef int StatusId;
  class Status {
  private:
    StatusId id_;
  public:
    constexpr Status(const StatusId id = UV_OK):
      id_(id) {
    }
    constexpr Status(const Status& rhs) = default;
    ~Status() = default;

    inline constexpr StatusId id() const {
      return id_;
    }

    inline constexpr bool IsOk() const {
      return id() >= UV_OK; //TODO: this might be wrong
    }

    const char* message() const {
      return IsOk() ? "Ok" : uv_strerror(id());
    }
    
    constexpr operator StatusId () const {
      return id();
    }

    constexpr operator bool () const {
      return IsOk();
    }

    Status& operator=(const Status& rhs) = default;

    friend std::ostream& operator<<(std::ostream& stream, const Status& rhs) {
      stream << "uv::Status(";
      stream << "id=" << rhs.id() << ", ";
      stream << "is_ok=" << rhs.IsOk();
      if(!rhs)
        stream << ", message=\"" << rhs.message() << "\"";
      stream << ")";
      return stream;
    }
  public:
    static inline constexpr Status
    New(const StatusId raw) {
      return Status(raw);
    }

    static inline constexpr Status
    Ok() {
      return New(UV_OK);
    }
  };
}

#endif //PRT_UV_STATUS_H