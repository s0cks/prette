#version 460 core
#extension GL_ARB_shading_language_include : require
layout(location = 1) in flat uint Frag_Material;
layout(location = 0) in vec2 Frag_TexPos;

layout(binding = 3) uniform sampler2D texSampler;

layout(location = 0) out vec4 Out_Color;

void main() {
  if (Frag_Material == 0) {
    Out_Color = texture(texSampler, Frag_TexPos);
  } else if (Frag_Material == 1) {
    Out_Color = vec4(0.0f, 1.0f, 0.0f, 1.0f);
  } else if (Frag_Material == 2) {
    Out_Color = vec4(0.0f, 0.0f, 1.0f, 1.0f);
  } else if (Frag_Material == 3) {
    Out_Color = vec4(1.0f, 0.0f, 1.0f, 1.0f);
  }
}