#ifndef PRT_UV_STATUS_H
#define PRT_UV_STATUS_H

#include <uv.h>
#include <ostream>

namespace prt::uv {
#ifndef UV_OK
#define UV_OK 0
#endif //UV_OK

  using StatusId = int;
  class Status {
  private:
    StatusId id_;
  public:
    constexpr Status(const StatusId id = UV_OK):
      id_(id) {
    }
    constexpr Status(const Status& rhs) = default;
    ~Status() = default;

    inline constexpr auto id() const -> StatusId {
      return id_;
    }

    inline constexpr auto IsOk() const -> bool {
      return id() >= UV_OK; //TODO: this might be wrong
    }

    auto message() const -> const char* {
      return IsOk() ? "Ok" : uv_strerror(id());
    }

    constexpr operator StatusId () const {
      return id();
    }

    constexpr operator bool () const {
      return IsOk();
    }

    auto operator=(const Status& rhs) -> Status& = default;

    friend auto operator<<(std::ostream& stream, const Status& rhs) -> std::ostream& {
      stream << "uv::Status(";
      stream << "id=" << rhs.id() << ", ";
      stream << "is_ok=" << rhs.IsOk();
      if(!rhs)
        stream << ", message=\"" << rhs.message() << "\"";
      stream << ")";
      return stream;
    }
  public:
    static inline constexpr auto
    New(const StatusId raw) -> Status {
      return {raw};
    }

    static inline constexpr auto
    Ok() -> Status {
      return New(UV_OK);
    }
  };
}

#endif //PRT_UV_STATUS_H