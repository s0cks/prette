#ifndef PRT_RX_H
#define PRT_RX_H

// IWYU pragma: begin_exports
// clang-format off
#include <rxcpp/rx.hpp>
#include <rxcpp/rx-util.hpp>
#include <rxcpp/rx-observable.hpp>
#include <rxcpp/rx-subscriber.hpp>
#include <rxcpp/rx-subscription.hpp>
#include <rxcpp/subjects/rx-subject.hpp>
// clang-format on
// IWYU pragma: end_exports

// IWYU pragma: no_include "subjects/rx-*.hpp"
// IWYU pragma: no_include "rx-util.hpp"

namespace prt::rx {
using rxcpp::subjects::subject;
using namespace rxcpp;
using error_ptr = rxcpp::util::error_ptr;
using rxcpp::util::what;

template <typename T>
static inline auto identity(const T& value) -> T {
  return value;
}

#define SUBSCRIBE_AND_LOG(Observable, Level)    (Observable).subscribe(LogEvent<Event>(google::Level, __FILE__, __LINE__))
#define _SUBSCRIBE_AND_LOG_EVENTS(Event, Level) SUBSCRIBE_AND_LOG(Get##Event##Observable(), Level)
#define SUBSCRIBE_AND_LOG_EVENTS(Event)         _SUBSCRIBE_AND_LOG_EVENTS(Event, INFO)
}  // namespace prt::rx

#endif  // PRT_RX_H
