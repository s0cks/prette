#version 460 core
#extension GL_ARB_shading_language_include : require
#extension GL_GOOGLE_include_directive : require

layout(location = 0) in vec2 In_Pos;
layout(location = 1) in vec4 In_Color;

#include "camera.glsl"

layout(std140, binding = 0) uniform CameraBlock {
  Camera camera;
};

layout(location = 0) out vec4 Frag_Color;

void main() {
  gl_Position = camera.projection * camera.view * vec4(In_Pos, 1.1, 1.0);
  Frag_Color = In_Color;
}