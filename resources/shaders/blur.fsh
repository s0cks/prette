#version 330 core
out vec4 Frag_Color;

in vec2 vUv;

uniform sampler2D tex;
uniform bool horizontal;
uniform float weight[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

void main() {
  vec2 uv_offset = 1.0 / textureSize(tex, 0);
  vec3 result = texture(tex, vUv).rgb * weight[0];
  if(horizontal) {
    for(int i = 1; i < 5; i++) {
      result += texture(tex, vUv + vec2(uv_offset.x * i, 0.0)).rgb * weight[i];
      result += texture(tex, vUv - vec2(uv_offset.x * i, 0.0)).rgb * weight[i];
    }
  } else {
    for(int i = 1; i < 5; i++) {
      result += texture(tex, vUv + vec2(0.0, uv_offset.y * i)).rgb * weight[i];
      result += texture(tex, vUv - vec2(0.0, uv_offset.y * i)).rgb * weight[i];
    }
  }
  Frag_Color = vec4(result, 1.0f);
}