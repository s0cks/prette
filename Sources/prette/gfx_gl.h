#ifndef PRT_GFX_GL_H
#define PRT_GFX_GL_H

#include "prette/common.h"
#ifdef OS_IS_OSX

#define GL_SILENCE_DEPRECATION 1
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>

#else

#include <GL/gl3.h>
#include <glew.h>

#endif  // OS_IS_OSX

namespace prt {}

#endif  // PRT_GFX_GL_H
