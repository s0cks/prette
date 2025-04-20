#version 450
layout(location = 0) in vec2 In_Pos;
layout(location = 1) in vec3 In_Color;

layout(location = 0) out vec3 Frag_Color;

void main() {
  gl_Position = vec4(In_Pos, 0.0, 1.0);
  Frag_Color = In_Color;
}