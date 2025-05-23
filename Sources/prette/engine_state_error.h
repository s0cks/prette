#ifndef PRT_ENGINE_STATE_ERROR_H
#define PRT_ENGINE_STATE_ERROR_H

#ifndef PRT_ENGINE_STATE_H
#error "Please #include <prette/engine_state.h> instead."
#endif  // PRT_ENGINE_STATE_H

#include <memory>

#include "prette/crash_report.h"

namespace prt {
class ErrorState : public EngineState {
 private:
  std::shared_ptr<CrashReportCause> cause_;

 protected:
  void EnterState(Engine* engine) override;
  void ExitState(Engine* engine) override;

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
}  // namespace prt

#endif  // PRT_ENGINE_STATE_ERROR_H
