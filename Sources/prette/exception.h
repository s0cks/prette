#ifndef PRT_EXCEPTION_H
#define PRT_EXCEPTION_H

#include <exception>
#include <string>
#include <utility>

namespace prt {
class Exception : public std::exception {
 private:
  std::string message_;

 public:
  explicit Exception(std::string message) :
    std::exception(),
    message_(std::move(message)) {}
  ~Exception() override = default;

  auto message() const -> const std::string& {
    return message_;
  }

  auto what() const noexcept -> const char* override {
    return message_.c_str();
  }

 public:
  static inline auto New(const std::string& message) -> std::exception_ptr {
    return std::make_exception_ptr(Exception(message));
  }
};
}  // namespace prt

#endif  // PRT_EXCEPTION_H
