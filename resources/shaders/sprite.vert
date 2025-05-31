#version 460 core
#extension GL_ARB_shading_language_include : require
#extension GL_GOOGLE_include_directive : require

#include "camera.glsl"

layout(location = 0) in vec2 In_Pos;
layout(location = 1) in vec2 In_TexPos;

layout(std140, binding = 0) uniform CameraBlock {
  Camera camera;
};

struct SpriteData {
  mat4 model;
  vec2 pos;
};

layout(std140, binding = 1) readonly buffer SpriteBlock {
  SpriteData sprites[];
};

layout(location = 0) out vec2 Frag_TexPos;

mat4 translate(vec3 translation) {
  // clang-format off
  return mat4(
    1.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    translation.xyz, 1.0);
  // clang-format on
}

void main() {
  SpriteData sprite = sprites[gl_InstanceIndex];
  gl_Position = camera.projection * camera.view * translate(vec3(sprite.pos, 1.0f)) * vec4(In_Pos.xy, 1.1f, 1.0f);
  Frag_TexPos = In_TexPos;
}