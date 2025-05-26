#ifndef PRT_NEXT_TICK_H
#define PRT_NEXT_TICK_H

#include <deque>
#include <functional>

#include "prette/tick.h"

namespace prt {
using NextTickCallback = std::function<void(const Tick& current, const Tick& previous)>;
using NextTickQueue = std::deque<NextTickCallback>;
}  // namespace prt

#endif  // PRT_NEXT_TICK_H
