#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 color;

layout (std140) uniform Camera {
  vec4 pos;
  mat4 projection;
  mat4 view;
} camera;

layout (std140) uniform DirectionalLight {
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
  vec4 direction;
} dir_light;

uniform mat4 model;

out vec2 vUv;
out vec3 vColor;
out vec3 vPos;
out vec3 vNormal;

void main() {
  vPos = pos;
  vNormal = vec3(0.0f, 1.0f, 0.0f);
  vColor = color;
  vUv = uv;
  gl_Position = camera.projection * camera.view * model * vec4(pos, 1.0f);
}