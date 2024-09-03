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
  typedef uword TickId;

  class TickDelta {
  protected:
    uword value_;
  public:
    constexpr TickDelta(const uword value):
      value_(value) {
    }
    constexpr TickDelta(const TickDelta& rhs) = default;
    ~TickDelta() = default;

    constexpr uword value() const {
      return value_;
    }

    void operator=(const uword& rhs) {
      value_ = rhs;
    }

    TickDelta& operator=(const TickDelta& rhs) = default;

    bool operator==(const TickDelta& rhs) const {
      return value() == rhs.value();
    }

    bool operator==(const uword& rhs) const {
      return value() == rhs;
    }

    bool operator!=(const TickDelta& rhs) const {
      return value() != rhs.value();
    }

    bool operator!=(const uword& rhs) const {
      return value() != rhs;
    }

    bool operator<(const TickDelta& rhs) const {
      return value() < rhs.value();
    }

    bool operator<(const uword& rhs) const {
      return value() < rhs;
    }

    bool operator>(const TickDelta& rhs) const {
      return value() > rhs.value();
    }

    bool operator>(const uword& rhs) const {
      return value() > rhs;
    }

    friend std::ostream& operator<<(std::ostream& stream, const TickDelta& rhs) {
      return stream << units::time::nanosecond_t(rhs.value());
    }
  };

  class Tick {
  public:
    static inline int
    Compare(const Tick& lhs, const Tick& rhs) {
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
  protected:
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

    constexpr TickId GetId() const {
      return id_;
    }

    constexpr uword GetTimestamp() const {
      return ts_;
    }

    constexpr operator uword () const {
      return ts_;
    }

    Tick& operator=(const Tick& rhs) = default;

    constexpr bool operator==(const Tick& rhs) const {
      return Compare(*this, rhs) == 0;
    }

    constexpr bool operator!=(const Tick& rhs) const {
      return Compare(*this, rhs) != 0;
    }

    constexpr bool operator<(const Tick& rhs) const {
      return Compare(*this, rhs) < 0;
    }

    constexpr bool operator>(const Tick& rhs) const {
      return Compare(*this, rhs) > 0;
    }

    friend constexpr TickDelta operator-(const Tick& lhs, const Tick& rhs) {
      return TickDelta(lhs.GetTimestamp() - rhs.GetTimestamp());
    }

    friend constexpr TickDelta operator-(const Tick& lhs, const uword& rhs) {
      return TickDelta(lhs.GetTimestamp() - rhs);
    }

    friend std::ostream& operator<<(std::ostream& stream, const Tick& rhs) {
      stream << "Tick(";
      stream << "id=" << rhs.GetId() << ", ";
      stream << "timestamp=" << rhs.GetTimestamp();
      stream << ")";
      return stream;
    }
  };

  static inline constexpr TickDelta
  operator-(const uword& lhs, const Tick& rhs) {
    return TickDelta(lhs - rhs.GetTimestamp());
  }

  typedef rx::subject<Tick> TickSubject;

  typedef TimeSeries<10> TickDurationSeries;
  typedef PerSecondCounter<uint64_t> TicksPerSecond;
}

#endif //PRT_UV_TICK_H