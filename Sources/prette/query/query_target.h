#ifndef PRT_QUERY_TARGET_H
#define PRT_QUERY_TARGET_H

#include "prette/gfx.h"

namespace prt::query {
  enum Target : GLenum {
    kSamplesPassed                          = GL_SAMPLES_PASSED,
    kAnySamplesPassed                       = GL_ANY_SAMPLES_PASSED,
    kPrimitivesGenerated                    = GL_PRIMITIVES_GENERATED,
    kTransformFeedbackPrimitivesWritten     = GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN,
    kTimeElapsed                            = GL_TIME_ELAPSED,
  };

  static inline std::ostream&
  operator<<(std::ostream& stream, const Target& rhs) {
    switch(rhs) {
      case kSamplesPassed:
        return stream << "SamplesPassed";
      case kAnySamplesPassed:
        return stream << "AnySamplesPassed";
      case kPrimitivesGenerated:
        return stream << "PrimitivesGenerated";
      case kTransformFeedbackPrimitivesWritten:
        return stream << "TransformFeedbackPrimitivesWritten";
      case kTimeElapsed:
        return stream << "TimeElapsed";
      default:
        return stream << "Unknown query::Target";
    }
  }
}

#endif //PRT_QUERY_TARGET_H