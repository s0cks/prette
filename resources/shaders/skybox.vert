#version 330 core
layout (location = 0) in vec3 pos;

layout (std140) uniform Camera {
  mat4 projection;
  mat4 view;
} camera;

uniform mat4 model;
out vec3 vPos;

void main() {
  vPos = pos;
  gl_Position = camera.projection * mat4(mat3(camera.view)) * vec4(pos, 1.0f);
}