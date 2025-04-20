#ifndef PRT_ENGINE_STATE_H
#define PRT_ENGINE_STATE_H

#include <memory>

#include "prette/common.h"
#include "prette/crash_report.h"
#include "prette/series.h"
#include "prette/tick.h"

namespace prt {
class Engine;
#define FOR_EACH_ENGINE_STATE(V) \
  V(Init)                        \
  V(Running)                     \
  V(Paused)                      \
  V(Terminated)                  \
  V(Error)

class EngineState;

using EngineStatePtr = std::unique_ptr<EngineState>;

#define FORWARD_DECLARE_STATE(Name) class Name##State;
FOR_EACH_ENGINE_STATE(FORWARD_DECLARE_STATE)
#undef FORWARD_DECLARE_STATE

#define DECLARE_ENGINE_STATE_TYPE(Name)                                                    \
 protected:                                                                                \
  virtual void EnterState(Engine* engine) override;                                        \
  virtual void OnTick(Engine* engine, const Tick& current, const Tick& previous) override; \
  virtual void ExitState(Engine* engine) override;                                         \
                                                                                           \
 public:                                                                                   \
  auto GetStateName() const -> const char* override {                                      \
    return #Name;                                                                          \
  }                                                                                        \
  auto As##Name##State()->Name##State* override {                                          \
    return this;                                                                           \
  }

#define DECLARE_ENGINE_STATE(Name)                             \
  class Name##State : public EngineState {                     \
   public:                                                     \
    Name##State() = default;                                   \
    ~Name##State() override = default;                         \
    DECLARE_ENGINE_STATE_TYPE(Name);                           \
                                                               \
   public:                                                     \
    static inline auto New() -> std::unique_ptr<Name##State> { \
      return std::make_unique<Name##State>();                  \
    }                                                          \
  };

class EngineState {
  friend class Engine;
  DEFINE_NON_COPYABLE_TYPE(EngineState);

 public:
  using DurationSeries = TimeSeries<>;

 protected:
  EngineState() = default;
  virtual void EnterState(Engine* engine) = 0;
  virtual void OnTick(Engine* engine, const Tick& current, const Tick& previous) = 0;
  virtual void ExitState(Engine* engine) = 0;

 public:
  virtual ~EngineState() = default;
  virtual auto GetStateName() const -> const char* = 0;

  virtual auto Equals(EngineState* rhs) const -> bool {
    ASSERT(rhs);
    return strcmp(GetStateName(), rhs->GetStateName()) == 0;
  }

#define DEFINE_TYPE_CHECK(Name)                  \
  virtual auto As##Name##State()->Name##State* { \
    return nullptr;                              \
  }                                              \
  auto Is##Name##State()->bool {                 \
    return As##Name##State() != nullptr;         \
  }
  FOR_EACH_ENGINE_STATE(DEFINE_TYPE_CHECK)
#undef DEFINE_TYPE_CHECK
};

DECLARE_ENGINE_STATE(Init);

class RunningState : public EngineState {
  friend class Engine;

 private:
  rx::subscription on_tick_{};

 public:
  RunningState() = default;
  ~RunningState() override = default;

  DECLARE_ENGINE_STATE_TYPE(Running);

 public:
  static inline auto New() -> std::unique_ptr<RunningState> {
    return std::make_unique<RunningState>();
  }
};

DECLARE_ENGINE_STATE(Paused);
DECLARE_ENGINE_STATE(Terminated);

class ErrorState : public EngineState {
 private:
  std::shared_ptr<CrashReportCause> cause_;

 public:
  explicit ErrorState(const std::shared_ptr<CrashReportCause>& cause) :
    EngineState(),
    cause_(cause) {}
  ~ErrorState() override = default;

  auto GetCause() const -> const std::shared_ptr<CrashReportCause>& {
    return cause_;
  }

  DECLARE_ENGINE_STATE_TYPE(Error);

 public:
  static inline auto New(const std::shared_ptr<CrashReportCause>& cause) -> std::unique_ptr<ErrorState> {
    return std::make_unique<ErrorState>(cause);
  }
};

#define ENGINE_STATE_ENTER_F(Name) void Name##State::EnterState(Engine* engine)
#define ENGINE_STATE_TICK_F(Name)  void Name##State::OnTick(Engine* engine, const Tick& current, const Tick& previous)
#define ENGINE_STATE_EXIT_F(Name)  void Name##State::ExitState(Engine* engine)

}  // namespace prt

#endif  // PRT_ENGINE_STATE_H
