#version 460 core
#extension GL_ARB_shading_language_include : require
#extension GL_GOOGLE_include_directive : require

#include "camera.glsl"

layout(std140, binding = 0) uniform CameraBlock {
  Camera camera;
};

layout(location = 0) in vec2 In_Pos;
layout(location = 1) in vec2 In_Tex;

layout(location = 0) out vec2 Frag_Tex;

void main() {
  gl_Position = camera.projection * camera.view * vec4(In_Pos.xy, 1.1f, 1.0f);
  Frag_Tex = In_Tex;
}