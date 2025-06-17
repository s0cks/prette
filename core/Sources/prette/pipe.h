#ifndef PRT_PIPE_H
#define PRT_PIPE_H

#include <functional>

namespace prt::pipe {
template <typename... Ts>
struct sink : std::function<void(Ts...)> {
  using std::function<void(Ts...)>::function;
};

template <typename... Ts>
using source = sink<sink<Ts...>>;

template <typename I, typename O>
using phase = sink<source<I>, sink<O>>;

template <typename I, typename O>
static inline auto operator|(phase<I, O> phase, sink<O> s) -> sink<I> {
  return [phase, s](I in) {
    return phase(
        [&in](sink<I> z) mutable {
          z(std::forward<I>(in));
        },
        s);
  };
}

template <typename I, typename O>
static inline auto operator|(source<I> src, phase<I, O> phase) -> source<O> {
  return [src, phase](sink<O> out) {
    return phase(src, out);
  };
}

template <typename I, typename T, typename O>
static inline auto operator|(phase<I, T> a, phase<T, O> b) -> phase<I, O> {
  return [a, b](source<I> in, sink<O> out) {
    return a(in, b | out);
  };
}

template <typename... Ts>
static inline auto operator|(source<Ts...> a, sink<Ts...> b) -> sink<> {
  return [a, b] {
    return a(b);
  };
}
}  // namespace prt::pipe

#endif  // PRT_PIPE_H
