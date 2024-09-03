#ifndef PRT_RUNTIME_H
#define PRT_RUNTIME_H

#include "prette/vao/vao.h"
#include "prette/vbo/vbo.h"
#include "prette/ibo/ibo.h"
#include "prette/fbo/fbo.h"
#include "prette/shader/shader.h"
#include "prette/program/program.h"
#include "prette/texture/texture.h"

namespace prt {
#ifdef PRT_DEBUG

  void PrintRuntimeInfo(const google::LogSeverity severity = google::INFO,
                        const char* file = __FILE__,
                        const int line = __LINE__,
                        const int indent = 0);

#endif //PRT_DEBUG
}

#endif //PRT_RUNTIME_H