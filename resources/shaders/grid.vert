#version 450
layout(location = 0) in vec2 InPos;

layout(std140, binding = 0) uniform CameraData {
  vec2 viewport_size;
  mat4 projection;
  mat4 view;
  vec3 pos;
  vec3 up;
  vec3 right;
  vec3 direction;
  float zoom;
  float speed;
}
camera;

layout(std140, binding = 1) uniform Grid {
  mat4 projection;
  mat4 view;
  vec3 color;
}
grid;

layout(location = 0) out vec4 Grid_Color;

void main() {
  gl_Position = grid.projection * grid.view * vec4(InPos, 3.0f, 1.0f);
  Grid_Color = vec4(grid.color, 1.0f);
}