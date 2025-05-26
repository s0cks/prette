#version 460 core
#extension GL_ARB_shading_language_include : require
#extension GL_GOOGLE_include_directive : require

layout(binding = 0) uniform sampler2D fontSampler;

layout(location = 0) in vec2 inUV;
layout(location = 1) in vec4 inColor;

layout(location = 0) out vec4 outColor;

void main() {
  outColor = inColor * texture(fontSampler, inUV);
}
