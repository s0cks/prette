#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec4 color;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec2 vUv;
out vec4 vColor;

void main() {
  vUv = uv;
  vColor = color;
  gl_Position = projection * view * model * vec4(pos.xyz, 1.0f);
}