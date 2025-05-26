#include "prette/system_ticker.h"

#include <chrono>

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/system.h"
#include "prette/tick.h"

namespace prt {
auto SystemTicker::Visit(System* rhs) -> bool {
  ASSERT(rhs);
  const auto sys_name = rhs->GetSystemName();
  PRT_PROFILING_BEGIN(sys);
  rhs->OnTick(GetCurrentTick(), GetPreviousTick());
  PRT_PROFILING_END(sys);
  DVLOG(2) << "updated " << sys_name << " in "
           << std::chrono::duration_cast<std::chrono::milliseconds>(sys_duration).count() << "ms.";
  return true;
}

void SystemTicker::TickAll(System* systems, const Tick& current, const Tick& previous) {
  SystemListIterator iter(systems);
  SystemTicker ticker(current, previous);
  PRT_PROFILING_BEGIN(all);
  while (iter.HasNext()) {
    const auto next = iter.Next();
    LOG_IF(FATAL, !ticker.Visit(next)) << "failed to tick: " << next->GetSystemName();
  }
  PRT_PROFILING_END(all);
  DVLOG_IF(2, ticker.GetNumberOfSystemsTicked() > 0)
      << "ticked " << ticker.GetNumberOfSystemsTicked() << " systems in "
      << std::chrono::duration_cast<std::chrono::milliseconds>(all_duration).count() << "ms.";
}
}  // namespace prt