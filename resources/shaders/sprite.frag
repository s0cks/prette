#version 460 core
#extension GL_ARB_shading_language_include : require
layout(location = 0) in vec2 Frag_TexPos;

layout(binding = 2) uniform sampler2D texSampler;

layout(location = 0) out vec4 Out_Color;

void main() {
  Out_Color = texture(texSampler, Frag_TexPos);
}