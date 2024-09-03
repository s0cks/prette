#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 color;
layout (location = 3) in vec2 uv;

layout (std140) uniform Camera {
  vec4 pos;
  mat4 projection;
  mat4 view;
} camera;

uniform int entity;
uniform mat4 model;
out vec3 vPos;
out vec3 vNormal;
out vec2 vUv;
out vec3 vColor;
flat out int vEntity;

void main() {
  vEntity = entity;
  vPos = pos;
  vNormal = normal;
  gl_Position = camera.projection * camera.view * model * vec4(pos, 1.0);
}