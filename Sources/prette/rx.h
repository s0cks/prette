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

#define SUBSCRIBE_AND_LOG(Observable, Event, Level) \
  (Observable).subscribe(LogEvent<Event>(google::Level, __FILE__, __LINE__))
}  // namespace prt::rx

#endif  // PRT_RX_H
