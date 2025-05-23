#version 460 core
#extension GL_ARB_shading_language_include : require
#extension GL_GOOGLE_include_directive : require

layout(location = 0) in vec3 Frag_Color;
layout(location = 1) in vec2 Frag_TexCoord;

layout(location = 0) out vec4 Out_Color;

void main() {
  Out_Color = vec4(Frag_Color, 1.0f);
}