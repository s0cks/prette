#version 460 core
#extension GL_ARB_shading_language_include : require

layout(location = 0) in vec2 Frag_TexPos;
layout(location = 1) in flat uint Frag_Material;
layout(location = 2) in flat uint Frag_Hovering;

layout(binding = 3) uniform sampler2D texSampler;

layout(location = 0) out vec4 Out_Color;

void main() {
  vec4 color = texture(texSampler, Frag_TexPos);
  // if (Frag_Material == 0) {
  //   return;
  // } else if (Frag_Material == 1) {
  //   Out_Color = vec4(0.0f, 1.0f, 0.0f, 1.0f);
  // } else if (Frag_Material == 2) {
  //   Out_Color = vec4(0.0f, 0.0f, 1.0f, 1.0f);
  // }

  if (Frag_Hovering == 1) {
    vec4 tint = vec4(1.0f, 0.0f, 1.0f, 1.0f);
    color = mix(color, tint, 0.5);
  }

  Out_Color = color;
}