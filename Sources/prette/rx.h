#ifndef PRT_RX_H
#define PRT_RX_H

#include <rxcpp/rx.hpp>

namespace prt {
  namespace rxsub = rxcpp::subjects;
  namespace rx {
    using rxcpp::subjects::subject;
    using namespace rxcpp;
    using error_ptr = rxcpp::util::error_ptr;
    using rxcpp::util::what;

    template<typename T>
    static inline T identity(const T& value) {
      return value;
    }
  }
}

#endif //PRT_RX_H