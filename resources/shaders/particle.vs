#version 150
in vec2 position;

layout (std140) uniform Camera {
  vec4 pos;
  mat4 projection;
  mat4 view;
} camera;

void main() {
  gl_Position = camera.projection * camera.view_transposed * vec4(position, 0.0f, 1.0f);
}