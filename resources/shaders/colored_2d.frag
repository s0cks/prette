#version 330 core
layout (location = 0) out vec4 Frag_Color;

in vec4 vColor;

void main() {
  Frag_Color = vColor;
}