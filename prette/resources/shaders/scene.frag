#version 450
layout(location = 0) in vec3 Frag_Color;
layout(location = 1) in vec2 Frag_TexCoord;

layout(location = 0) out vec4 Out_Color;

layout(binding = 1) uniform sampler2D tex;

void main() {
  Out_Color = texture(tex, Frag_TexCoord);
}