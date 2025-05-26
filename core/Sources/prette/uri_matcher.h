#ifndef PRT_URI_MATCHER_H
#define PRT_URI_MATCHER_H

#include <functional>
#include <type_traits>

#include "prette/uri.h"

namespace prt::uri {
using UriPredicate = std::function<bool(const Uri& uri)>;

template <typename T>
struct is_uri_matcher_t : std::false_type {};

template <>
struct is_uri_matcher_t<UriPredicate> : std::true_type {};
}  // namespace prt::uri

#endif  // PRT_URI_MATCHER_H
