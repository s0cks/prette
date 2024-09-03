#version 150
out vec4 Frag_Color;

precision mediump float;
uniform sampler2D tex;

in vec2 vUv;
in vec4 vColor;

void main() {
  Frag_Color = texture(tex, vUv.st) * vColor;
}