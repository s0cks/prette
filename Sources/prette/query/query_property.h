#ifndef PRT_QUERY_PROPERTY_H
#define PRT_QUERY_PROPERTY_H

#include "prette/gfx.h"

namespace prt::query {
  enum Property : GLenum {
    kResult               = GL_QUERY_RESULT,
    kResultAvailable      = GL_QUERY_RESULT_AVAILABLE,
  };

  static inline std::ostream& 
  operator<<(std::ostream& stream, const Property& rhs) {
    switch(rhs) {
      case kResult:
        return stream << "Result";
      case kResultAvailable:
        return stream << "ResultAvailable";
      default:
        return stream << "Unknown query::Property";
    }
  }
}

#endif //PRT_QUERY_PROPERTY_H