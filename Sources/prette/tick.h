#ifndef PRT_TICK_H
#define PRT_TICK_H

#include <units.h>

#include <cstdint>
#include <iostream>

#include "prette/counter.h"
#include "prette/rx.h"
#include "prette/series.h"

namespace prt {
using TickId = uint64_t;

class TickDelta {
 private:
  uint64_t value_;

 public:
  constexpr TickDelta(const uint64_t value) :
    value_(value) {}
  constexpr TickDelta(const TickDelta& rhs) = default;
  ~TickDelta() = default;

  constexpr auto value() const -> uint64_t {
    return value_;
  }

  auto operator=(const uint64_t& rhs) -> TickDelta& {
    value_ = rhs;
    return *this;
  }

  auto operator=(const TickDelta& rhs) -> TickDelta& = default;

  auto operator==(const TickDelta& rhs) const -> bool {
    return value() == rhs.value();
  }

  auto operator==(const uint64_t& rhs) const -> bool {
    return value() == rhs;
  }

  auto operator!=(const TickDelta& rhs) const -> bool {
    return value() != rhs.value();
  }

  auto operator!=(const uint64_t& rhs) const -> bool {
    return value() != rhs;
  }

  auto operator<(const TickDelta& rhs) const -> bool {
    return value() < rhs.value();
  }

  auto operator<(const uint64_t& rhs) const -> bool {
    return value() < rhs;
  }

  auto operator>(const TickDelta& rhs) const -> bool {
    return value() > rhs.value();
  }

  auto operator>(const uint64_t& rhs) const -> bool {
    return value() > rhs;
  }

  auto operator/(const uint64_t& rhs) const -> float {
    return static_cast<float>(value()) / static_cast<float>(rhs);
  }

  friend auto operator<<(std::ostream& stream, const TickDelta& rhs) -> std::ostream& {
    return stream << units::time::nanosecond_t(rhs.value());  // NOLINT(cppcoreguidelines-narrowing-conversions)
  }
};

class Tick {
 public:
  static inline auto Compare(const Tick& lhs, const Tick& rhs) -> int {
    if (lhs.GetId() < rhs.GetId()) {
      return -1;
    } else if (lhs.GetId() > rhs.GetId()) {
      return +1;
    }

    if (lhs.GetTimestamp() < rhs.GetTimestamp()) {
      return -1;
    } else if (lhs.GetTimestamp() > rhs.GetTimestamp()) {
      return +1;
    }
    return 0;
  }

 private:
  TickId id_;
  uint64_t ts_;

 public:
  constexpr Tick(const TickId id, const uint64_t ts = uv_hrtime()) :
    id_(id),
    ts_(ts) {}
  constexpr Tick() :
    Tick(0, 0) {}
  constexpr Tick(const Tick& rhs) = default;
  ~Tick() = default;

  constexpr auto GetId() const -> TickId {
    return id_;
  }

  constexpr auto GetTimestamp() const -> uint64_t {
    return ts_;
  }

  constexpr operator uint64_t() const {
    return ts_;
  }

  auto operator=(const Tick& rhs) -> Tick& = default;

  constexpr auto operator==(const Tick& rhs) const -> bool {
    return Compare(*this, rhs) == 0;
  }

  constexpr auto operator!=(const Tick& rhs) const -> bool {
    return Compare(*this, rhs) != 0;
  }

  constexpr auto operator<(const Tick& rhs) const -> bool {
    return Compare(*this, rhs) < 0;
  }

  constexpr auto operator>(const Tick& rhs) const -> bool {
    return Compare(*this, rhs) > 0;
  }

  friend constexpr auto operator+(const Tick& lhs, const Tick& rhs) -> TickDelta {
    return {lhs.GetTimestamp() + rhs.GetTimestamp()};
  }

  friend constexpr auto operator+(const Tick& lhs, const uint64_t& rhs) -> TickDelta {
    return {lhs.GetTimestamp() + rhs};
  }

  void operator+=(const uint64_t& rhs) {
    ts_ += rhs;
  }

  friend constexpr auto operator-(const Tick& lhs, const Tick& rhs) -> TickDelta {
    return {lhs.GetTimestamp() - rhs.GetTimestamp()};
  }

  friend constexpr auto operator-(const Tick& lhs, const uint64_t& rhs) -> TickDelta {
    return {lhs.GetTimestamp() - rhs};
  }

  void operator-=(const uint64_t& rhs) {
    ts_ -= rhs;
  }

  friend auto operator<<(std::ostream& stream, const Tick& rhs) -> std::ostream& {
    stream << "Tick(";
    stream << "id=" << rhs.GetId() << ", ";
    stream << "timestamp=" << rhs.GetTimestamp();
    stream << ")";
    return stream;
  }
};

static inline constexpr auto operator+(const uint64_t& lhs, const Tick& rhs) -> TickDelta {
  return lhs + rhs.GetTimestamp();
}

static inline auto operator+=(uint64_t& lhs, const TickDelta& rhs) -> uint64_t& {
  lhs += rhs.value();
  return lhs;
}

static inline constexpr auto operator-(const uint64_t& lhs, const Tick& rhs) -> TickDelta {
  return lhs - rhs.GetTimestamp();
}

static inline auto operator-=(uint64_t& lhs, const TickDelta& rhs) -> uint64_t& {
  lhs -= rhs.value();
  return lhs;
}

using TickSubject = rx::subject<Tick>;
using TickObservable = rx::observable<Tick>;
using TickDurationSeries = TimeSeries<>;
using TicksPerSecond = PerSecondCounter<uint64_t>;
}  // namespace prt

#endif  // PRT_TICK_H
