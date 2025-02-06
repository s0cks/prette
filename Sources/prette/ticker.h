#ifndef PRT_TICKER_H
#define PRT_TICKER_H

#include <uv.h>

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
  TickDurationSeries duration_{};

 protected:
  virtual void Idle() {
    // publish tick
    std::swap(previous_, current_);
    current_ = Tick(((uint64_t)count_));
    count_.Increment(1, current_.GetTimestamp());
    duration_.Append((current_ - previous_).value());
  }

  virtual void Prepare() {
    const auto& subscriber = ticks_.get_subscriber();
    subscriber.on_next(GetCurrentTick());
  }

  virtual void Check() {
    // TODO:
    //  - check for stop
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
    return duration_;
  }
};

template <const uword Rate>
class RateLimitedTicker : public Ticker {
 private:
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
    if (GetTimeSinceLastTick() < Rate)
      return SetSkipped();
    return Ticker::Idle();
  }

  void Prepare() override {
    if (IsSkipped())
      return;
    return Ticker::Prepare();
  }

  void Check() override {
    if (IsSkipped())
      return ClearSkipped();
    return Ticker::Check();
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
