#ifndef PRT_TIMESTAMP_H
#define PRT_TIMESTAMP_H

#include <chrono>
#include <string>

namespace prt {
  using RawTimestamp = uint64_t;

  using SysClock = std::chrono::system_clock;
  using Timestamp = SysClock::time_point;
  using Duration = SysClock::duration;

  static inline auto
  Now() -> Timestamp {
    return SysClock::now();
  }

  static inline auto
  GetElapsedTimeMilliseconds(const Timestamp start,
                             const Timestamp stop = Now()) -> uint64_t {
    using namespace std::chrono;
    return duration_cast<milliseconds>(stop - start).count();
  }

  static inline auto
  ToUnixTimestamp(const Timestamp ts = Now()) -> RawTimestamp{
    using namespace std::chrono;
    return duration_cast<milliseconds>(ts.time_since_epoch()).count();
  }

  static inline auto
  FromUnixTimestamp(const RawTimestamp ms) -> Timestamp{
    using namespace std::chrono;
    return Timestamp(milliseconds(ms));
  }

  auto FormatTimestamp(const Timestamp ts, const std::string& format) -> std::string;

  static inline auto
  FormatTimestampReadable(const Timestamp ts) -> std::string {
    return FormatTimestamp(ts, "%m/%d/%Y %H:%M:%S");
  }

  static inline auto
  FormatTimestampFileSafe(const Timestamp ts) -> std::string {
    return FormatTimestamp(ts, "%Y%m%d-%H%M%S");
  }
}

#endif //PRT_TIMESTAMP_H