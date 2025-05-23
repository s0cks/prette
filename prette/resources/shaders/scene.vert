#version 460 core
#extension GL_ARB_shading_language_include : require
#extension GL_GOOGLE_include_directive : require

#include "camera.glsl"

layout(std140, binding = 0) uniform CameraBlock {
  Camera camera;
};

layout(location = 0) in vec3 In_Pos;
layout(location = 1) in vec3 In_Color;
layout(location = 2) in vec2 In_TexCoord;

layout(location = 0) out vec3 Frag_Color;
layout(location = 1) out vec2 Frag_TexCoord;

void main() {
  gl_Position = camera.projection * camera.view * vec4(In_Pos, 1.0);
  Frag_Color = In_Color;
  Frag_TexCoord = In_TexCoord;
}