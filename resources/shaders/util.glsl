#ifndef PRT_UTIL_GLSL
#define PRT_UTIL_GLSL

mat4 translate(vec3 translation) {
  // clang-format off
  return mat4(
    1.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    translation.xyz, 1.0);
  // clang-format on
}

mat4 scale(vec3 rhs) {
  // clang-format off
  return mat4(
    rhs.x, 0.0, 0.0, 0.0,
    0.0, rhs.y, 0.0, 0.0,
    0.0, 0.0, rhs.z, 0.0,
    0.0, 0.0, 0.0, 1.0);
  // clang-format on
}

#endif  // PRT_UTIL_GLSL