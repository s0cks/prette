#version 460 core
#extension GL_ARB_shading_language_include : require
#extension GL_GOOGLE_include_directive : require

layout(location = 0) in vec2 In_Pos;
layout(location = 1) in vec4 In_Color;

layout(std140, push_constant) uniform push_constants {
  mat4 projection;
  vec4 color;
}
PushConstants;

layout(location = 0) out vec4 Frag_Color;

void main() {
  gl_Position = PushConstants.projection * vec4(In_Pos.xy, 1.1, 1.0f);
  Frag_Color = PushConstants.color;
}