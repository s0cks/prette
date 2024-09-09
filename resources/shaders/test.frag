#version 450

layout(location = 0) in vec3 Frag_Color;

layout(location = 0) out vec4 Out_Color;

void main() {
  // this is weird
  Out_Color = vec4(Frag_Color, 1.0);
}