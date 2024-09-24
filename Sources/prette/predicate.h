#ifndef PRT_PREDICATE_H
#define PRT_PREDICATE_H

#include <functional>

namespace prt {
template <class T> class Predicate {
public:
  using Function = std::function<bool(const T &)>;

public:
  static inline auto And(const Function &first, const Function &second,
                         const Function &more...) -> Function {
    return [&](const T &value) {
      if (!(first(value) & second(value)))
        return false;
      for (const auto &fn : more) {
        if (!fn(value))
          return false;
      }
      return true;
    };
  }

  static inline auto Or(const Function &lhs, const Function &rhs) -> Function {
    return [&](const T &value) { return lhs(value) | rhs(value); };
  }
};
} // namespace prt

#endif // PRT_PREDICATE_H