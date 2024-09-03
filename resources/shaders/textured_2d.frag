#version 150
out vec4 Frag_Color;

uniform sampler2D tex;
in vec2 vUv;
in vec4 vColor;

void main() {
  vec3 texel = texture(tex, vUv).rgb;
  Frag_Color = vec4(texel, 1.0f);
}