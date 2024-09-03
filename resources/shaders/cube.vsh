#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 color;
layout (location = 3) in vec2 uv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 vPos;
out vec3 vNormal;
out vec2 vUv;
out vec3 vColor;

void main() {
  vUv = uv;
  vNormal = normal;
  vPos = vec3(model * vec4(pos, 1.0f));
  vColor = color;
  gl_Position = projection * view * model * vec4(pos, 1.0f);
}