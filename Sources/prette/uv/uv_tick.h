#ifndef PRT_UV_TICK_H
#define PRT_UV_TICK_H

#include <uv.h>
#include <units.h>
#include <cstdint>

#include "prette/rx.h"
#include "prette/series.h"
#include "prette/counter.h"
#include "prette/timestamp.h"

namespace prt::uv {
  using TickId = uword;

  class TickDelta {
  private:
    uword value_;
  public:
    constexpr TickDelta(const uword value):
      value_(value) {
    }
    constexpr TickDelta(const TickDelta& rhs) = default;
    ~TickDelta() = default;

    constexpr auto value() const -> uword {
      return value_;
    }

    auto operator=(const uword& rhs) -> TickDelta& {
      value_ = rhs;
      return *this;
    }

    auto operator=(const TickDelta& rhs) -> TickDelta& = default;

    auto operator==(const TickDelta& rhs) const -> bool {
      return value() == rhs.value();
    }

    auto operator==(const uword& rhs) const -> bool {
      return value() == rhs;
    }

    auto operator!=(const TickDelta& rhs) const -> bool {
      return value() != rhs.value();
    }

    auto operator!=(const uword& rhs) const -> bool {
      return value() != rhs;
    }

    auto operator<(const TickDelta& rhs) const -> bool {
      return value() < rhs.value();
    }

    auto operator<(const uword& rhs) const -> bool {
      return value() < rhs;
    }

    auto operator>(const TickDelta& rhs) const -> bool {
      return value() > rhs.value();
    }

    auto operator>(const uword& rhs) const -> bool {
      return value() > rhs;
    }

    friend auto operator<<(std::ostream& stream, const TickDelta& rhs) -> std::ostream& {
      return stream << units::time::nanosecond_t(rhs.value()); // NOLINT(cppcoreguidelines-narrowing-conversions)
    }
  };

  class Tick {
  public:
    static inline auto
    Compare(const Tick& lhs, const Tick& rhs) -> int {
      if(lhs.GetId() < rhs.GetId()) {
        return -1;
      } else if(lhs.GetId() > rhs.GetId()) {
        return +1;
      }

      if(lhs.GetTimestamp() < rhs.GetTimestamp()) {
        return -1;
      } else if(lhs.GetTimestamp() > rhs.GetTimestamp()) {
        return +1;
      }
      return 0;
    }
  private:
    TickId id_;
    uword ts_;
  public:
    constexpr Tick(const TickId id,
                   const uword ts):
      id_(id),
      ts_(ts) {
    }
    constexpr Tick():
      Tick(0, 0) {
    }
    constexpr Tick(const Tick& rhs) = default;
    ~Tick() = default;

    constexpr auto GetId() const -> TickId {
      return id_;
    }

    constexpr auto GetTimestamp() const -> uword {
      return ts_;
    }

    constexpr operator uword () const {
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

    friend constexpr auto operator-(const Tick& lhs, const Tick& rhs) -> TickDelta {
      return {lhs.GetTimestamp() - rhs.GetTimestamp()};
    }

    friend constexpr auto operator-(const Tick& lhs, const uword& rhs) -> TickDelta {
      return {lhs.GetTimestamp() - rhs};
    }

    friend auto operator<<(std::ostream& stream, const Tick& rhs) -> std::ostream& {
      stream << "Tick(";
      stream << "id=" << rhs.GetId() << ", ";
      stream << "timestamp=" << rhs.GetTimestamp();
      stream << ")";
      return stream;
    }
  };

  static inline constexpr auto
  operator-(const uword& lhs, const Tick& rhs) -> TickDelta {
    return {lhs - rhs.GetTimestamp()};
  }

  using TickSubject = rx::subject<Tick>;
  using TickDurationSeries = TimeSeries<10>;
  using TicksPerSecond = PerSecondCounter<uint64_t>;
}

#endif //PRT_UV_TICK_H