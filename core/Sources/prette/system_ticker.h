#ifndef PRT_SYSTEM_TICKER_H
#define PRT_SYSTEM_TICKER_H

#include <cstdint>
#include <utility>

#include "prette/system.h"
#include "prette/tick.h"

namespace prt {
class SystemTicker : public SystemVisitor {
 private:
  Tick current_;
  Tick previous_;
  uint64_t num_ticked_ = 0;

  inline void IncrementCounter() {
    num_ticked_ += 1;
  }

  inline void DecrementCounter() {
    num_ticked_ -= 1;
  }

 public:
  SystemTicker(const Tick current, const Tick previous) :
    SystemVisitor(),
    current_(std::move(current)),
    previous_(std::move(previous)) {}
  ~SystemTicker() override = default;

  auto GetNumberOfSystemsTicked() const -> uint64_t {
    return num_ticked_;
  }

  auto GetCurrentTick() const -> const Tick& {
    return current_;
  }

  auto GetPreviousTick() const -> const Tick& {
    return previous_;
  }

  auto Visit(System* rhs) -> bool override;

 public:
  static void TickAll(System* systems, const Tick& current, const Tick& previous);
};
}  // namespace prt

#endif  // PRT_SYSTEM_TICKER_H
