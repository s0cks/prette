#version 460 core
#extension GL_ARB_shading_language_include : require
#extension GL_GOOGLE_include_directive : require

#include "camera.glsl"
#include "transform.glsl"

layout(std140, binding = 0) uniform CameraBlock {
  Camera camera;
};

layout(std140, push_constant) uniform PushConstants {
  Transform transform;
};

layout(location = 0) in vec2 In_Pos;
layout(location = 1) in vec4 In_Color;

layout(location = 0) out vec4 Frag_Color;

void main() {
  gl_Position = vec4(In_Pos, 0.0, 1.0);
  Frag_Color = In_Color;
}