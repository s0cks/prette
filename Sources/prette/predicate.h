#ifndef PRT_PREDICATE_H
#define PRT_PREDICATE_H

#include <functional>

namespace prt {
  template<class T>
  class Predicate {
  public:
    typedef std::function<bool(const T& value)> Function;
  public:
    static inline Function
    And(const Function& first, const Function& second, const Function& more...) {
      return [&](const T& value) {
        if(!(first(value) & second(value)))
          return false;
        for(const auto& fn : more) {
          if(!fn(value))
            return false;
        }
        return true;
      };
    }

    static inline Function
    Or(const Function& lhs, const Function& rhs) {
      return [&](const T& value) {
        return lhs(value) | rhs(value);
      };
    }
  };
}

#endif //PRT_PREDICATE_H