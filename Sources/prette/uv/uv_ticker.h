#ifndef PRT_UV_TICKER_H
#define PRT_UV_TICKER_H

#include <units.h>

#include "prette/uv/uv_tick.h"
#include "prette/uv/uv_status.h"
#include "prette/uv/uv_handle.h"

namespace prt::uv {
  class Ticker {
  protected:
    // state
    Tick first_;
    Tick current_;
    Tick previous_;
    TickSubject ticks_;
    // handles
    Loop* loop_;
    Idle idle_;
    Prepare prepare_;
    uv_async_t on_tick_;
    Check check_;
    // stats
    TicksPerSecond count_;
    TickDurationSeries duration_;

    virtual void OnIdle() {
      // publish tick
      std::swap(previous_, current_);
      current_ = uv::Tick(((uint64_t) count_), uv::Now());
      count_.Increment(1, current_.GetTimestamp());
      duration_.Append((current_ - previous_).value());
    }

    virtual void OnPrepare() {
      const auto status = uv::AsyncSend(&on_tick_);
      LOG_IF(FATAL, !status) << "failed to call on_tick_: " << status;
    }

    virtual void OnCheck() {
      //TODO:
      // - check for stop
    }

    static inline void
    OnTick(uv_async_t* handle) {
      const auto ticker = uv::GetHandleData<Ticker>(handle);
      const auto& subscriber = ticker->ticks_.get_subscriber();
      subscriber.on_next(ticker->GetCurrentTick());
    }
  public:
    explicit Ticker(Loop* loop,
                    const uword start_ns = uv::Now(),
                    const bool start = true):
      loop_(loop),
      idle_(loop, [this](Idle* idle) { return OnIdle(); }, start),
      prepare_(loop, [this](Prepare* prepare) { return OnPrepare(); }, start),
      on_tick_(),
      check_(loop, [this](Check* check) { return OnCheck(); }, start),
      count_(0),
      first_(0, start_ns),
      current_(0, start_ns),
      previous_(0, start_ns),
      duration_(),
      ticks_() {
      {
        uv::SetHandleData(on_tick_, this);
        const auto status = uv::InitAsync(loop, &on_tick_, &OnTick);
        LOG_IF(FATAL, !status) << "failed to initialize on_tick_: " << status;
      }
    }
    virtual ~Ticker() {
      Stop();
    }

    const Tick& GetFirstTick() const {
      return first_;
    }

    const Tick& GetCurrentTick() const {
      return current_;
    }

    const Tick& GetPreviousTick() const {
      return previous_;
    }

    uint64_t GetTotalTicks() const {
      return (uint64_t) count_;
    }

    TickDelta GetTimeSinceCurrentTick(const uword ts = uv_hrtime()) const {
      return (ts - GetCurrentTick());
    }

    TickDelta GetTimeSinceLastTick(const uword ts = uv_hrtime()) const {
      return (ts - GetPreviousTick());
    }

    const TicksPerSecond& GetTicksPerSecond() const {
      return count_;
    }

    const TickDurationSeries& GetTickDurationSeries() const {
      return duration_;
    }
    
    rx::observable<Tick> ToObservable() const {
      return ticks_.get_observable();
    }

    void Stop() {
      idle_.Stop();
      prepare_.Stop();
      check_.Stop();
    }
  };

  template<const uword Rate>
  class RateLimitedTicker : public Ticker {
  protected:
    bool skipped_;

    inline void
    SetSkipped(const bool skipped = true) {
      skipped_ = skipped;
    }

    inline void
    ClearSkipped() {
      return SetSkipped(false);
    }

    inline bool
    IsSkipped() {
      return skipped_;
    }

    void OnIdle() override {
      if(GetTimeSinceLastTick() < Rate)
        return SetSkipped();
      return Ticker::OnIdle();
    }

    void OnPrepare() override {
      if(IsSkipped())
        return;
      return Ticker::OnPrepare();
    }

    void OnCheck() override {
      if(IsSkipped())
        return ClearSkipped();
      return Ticker::OnCheck();
    }
  public:
    RateLimitedTicker(Loop* loop,
                      const uword start_ns = uv::Now(),
                      const bool start = true):
      Ticker(loop, start_ns, start),
      skipped_(false) {
    }
    ~RateLimitedTicker() override = default;

    uword GetRate() const {
      return Rate;
    }
  };
}

#endif //PRT_UV_TICKER_H