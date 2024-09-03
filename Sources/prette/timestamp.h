#ifndef PRT_TIMESTAMP_H
#define PRT_TIMESTAMP_H

#include <chrono>
#include <string>

namespace prt {
  typedef uint64_t RawTimestamp;

  typedef std::chrono::system_clock SysClock;
  typedef SysClock::time_point Timestamp;
  typedef SysClock::duration Duration;

  static inline Timestamp
  Now() {
    return SysClock::now();
  }

  static inline uint64_t
  GetElapsedTimeMilliseconds(const Timestamp start,
                             const Timestamp stop = Now()) {
    using namespace std::chrono;
    return duration_cast<milliseconds>(stop - start).count();
  }

  static inline RawTimestamp
  ToUnixTimestamp(const Timestamp ts = Now()){
    using namespace std::chrono;
    return duration_cast<milliseconds>(ts.time_since_epoch()).count();
  }

  static inline Timestamp
  FromUnixTimestamp(const RawTimestamp ms){
    using namespace std::chrono;
    return Timestamp(milliseconds(ms));
  }

  std::string FormatTimestamp(const Timestamp ts, const std::string& format);

  static inline std::string
  FormatTimestampReadable(const Timestamp ts) {
    return FormatTimestamp(ts, "%m/%d/%Y %H:%M:%S");
  }

  static inline std::string
  FormatTimestampFileSafe(const Timestamp ts) {
    return FormatTimestamp(ts, "%Y%m%d-%H%M%S");
  }
}

#endif //PRT_TIMESTAMP_H