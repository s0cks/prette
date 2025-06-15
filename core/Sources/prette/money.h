#ifndef PRT_MONEY_H
#define PRT_MONEY_H

#include <ostream>
namespace prt {
static constexpr const auto kMoneyPrefix = "$";

class Money {
 private:
  double value_;

 public:
  constexpr Money(const double value = 0.0f) :
    value_(value) {}
  ~Money() = default;

  constexpr auto Get() const -> double {
    return value_;
  }

  constexpr operator double() const {
    return Get();
  }

  constexpr auto operator+(const Money& rhs) -> Money {
    return Get() + rhs.Get();
  }

  constexpr auto operator+(const double& rhs) -> Money {
    return Get() + rhs;
  }

  auto operator+=(const Money& rhs) -> Money& {
    value_ += rhs.Get();
    return *this;
  }

  auto operator+=(const double& rhs) -> Money& {
    value_ += rhs;
    return *this;
  }

  constexpr auto operator-(const Money& rhs) -> Money {
    return Get() - rhs.Get();
  }

  constexpr auto operator-(const double& rhs) -> Money {
    return Get() - rhs;
  }

  auto operator-=(const Money& rhs) -> Money& {
    value_ -= rhs.Get();
    return *this;
  }

  auto operator-=(const double& rhs) -> Money& {
    value_ -= rhs;
    return *this;
  }

  friend auto operator<<(std::ostream& stream, const Money& rhs) -> std::ostream& {
    return stream << rhs.Get() << kMoneyPrefix;
  }
};
}  // namespace prt

#endif  // PRT_MONEY_H
