#version 330 core
out vec4 Frag_Color;

in vec2 vUv;

uniform sampler2D tex;
uniform sampler2D bloomTex;
uniform bool hdr;
uniform bool bloom;
uniform float gamma;
uniform float exposure;

void main() {
  vec3 color = texture(tex, vUv).rgb;
  if(bloom) {
    vec3 bloomColor = texture(bloomTex, vUv).rgb;
    color += bloomColor;
  }
  
  vec3 result;
  if(hdr) {
    result = vec3(1.0f) - exp(-color * exposure);
    result = pow(result, vec3(1.0f / gamma));
  } else {
    result = pow(color, vec3(1.0f / gamma));
  }
  Frag_Color = vec4(result, 1.0f);
}