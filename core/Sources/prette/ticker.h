#ifndef PRT_TICKER_H
#define PRT_TICKER_H

#include <utility>

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/platform.h"
#include "prette/rx.h"
#include "prette/tick.h"
#include "prette/uv/utils.h"

namespace prt {
class Ticker {
 private:
  // state
  Tick epoch_{};
  Tick current_{};
  Tick previous_{};
  rx::subject<Tick> ticks_;
  // handles
  uv::Loop* loop_;
  uv::Idle idle_;
  uv::Prepare prepare_;
  uv::Check check_;
  // stats
  TicksPerSecond count_{};
  uint64_t duration_ = 0;
  TickDurationSeries durations_{};

 protected:
  auto GetDuration() const -> uint64_t {
    return duration_;
  }

  virtual void Idle() {
    current_ = Tick(((uint64_t)count_));
    count_.Increment(1, current_.GetTimestamp());
  }

  virtual void Prepare() {
    const auto& subscriber = ticks_.get_subscriber();
    subscriber.on_next(GetCurrentTick());
  }

  virtual void Check() {
    duration_ = GetTimeSinceCurrentTick().value();
    durations_.Append(duration_);
    std::swap(previous_, current_);
  }

 private:
  static inline void OnIdle(uv_idle_t* handle) {
    const auto ticker = uv::GetHandleData<uv_idle_t, Ticker>(handle);
    ASSERT(ticker);
    return ticker->Idle();
  }

  static inline void OnPrepare(uv_prepare_t* handle) {
    const auto ticker = uv::GetHandleData<uv_prepare_t, Ticker>(handle);
    ASSERT(ticker);
    return ticker->Prepare();
  }

  static inline void OnCheck(uv_check_t* handle) {
    const auto ticker = uv::GetHandleData<uv_check_t, Ticker>(handle);
    ASSERT(ticker);
    return ticker->Check();
  }

  inline void SetEpoch(const Tick& rhs) {
    epoch_ = previous_ = current_ = rhs;
  }

  inline void SetEpoch(const uint64_t start_ns) {
    return SetEpoch(Tick(0, start_ns));
  }

 public:
  explicit Ticker(uv::Loop* loop, const uint64_t start_ns = uv::Now()) :
    loop_(loop),
    idle_(loop, &OnIdle, this),
    prepare_(loop, &OnPrepare, this),
    check_(loop, &OnCheck, this) {
    SetEpoch(start_ns);
  }
  virtual ~Ticker() = default;

  void Start() {
    idle_.Start();
    prepare_.Start();
    check_.Start();
  }

  void Stop() {
    idle_.Stop();
    prepare_.Stop();
    check_.Stop();
  }

  void Close() {
    idle_.Close();
    prepare_.Close();
    check_.Close();
  }

  auto GetTickSubject() const -> const rx::subject<Tick>& {
    return ticks_;
  }

  auto OnTick() const -> rx::observable<Tick> {
    return GetTickSubject().get_observable();
  }

  auto GetEpochTick() const -> const Tick& {
    return epoch_;
  }

  auto GetCurrentTick() const -> const Tick& {
    return current_;
  }

  auto GetPreviousTick() const -> const Tick& {
    return previous_;
  }

  auto GetTotalTicks() const -> uint64_t {
    return (uint64_t)count_;
  }

  auto GetTimeSinceCurrentTick(const uint64_t ts = uv_hrtime()) const -> TickDelta {
    return (ts - GetCurrentTick());
  }

  auto GetTimeSinceLastTick(const uint64_t ts = uv_hrtime()) const -> TickDelta {
    return (ts - GetPreviousTick());
  }

  auto GetTicksPerSecond() const -> const TicksPerSecond& {
    return count_;
  }

  auto GetTickDurationSeries() const -> const TickDurationSeries& {
    return durations_;
  }
};

template <const uword Rate>
class RateLimitedTicker : public Ticker {
 private:
  uint64_t remaining_ = 0;
  bool skipped_{};

 protected:
  inline void SetSkipped(const bool skipped = true) {
    skipped_ = skipped;
  }

  inline void ClearSkipped() {
    return SetSkipped(false);
  }

  inline auto IsSkipped() -> bool {
    return skipped_;
  }

  void Idle() override {
    remaining_ += GetTimeSinceLastTick();
    while (remaining_ >= Rate) {
      Ticker::Idle();
      remaining_ -= Rate;
    }
  }

  void Prepare() override {
    if (IsSkipped())
      return;
    return Ticker::Prepare();
  }

  void Check() override {
    ClearSkipped();
    Ticker::Check();
  }

 public:
  explicit RateLimitedTicker(uv::Loop* loop, const uword start_ns = uv::Now()) :
    Ticker(loop, start_ns) {}
  ~RateLimitedTicker() override = default;

  auto GetRate() const -> uword {
    return Rate;
  }
};
}  // namespace prt

#endif  // PRT_TICKER_H
