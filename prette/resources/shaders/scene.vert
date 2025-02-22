#version 450
layout(location = 0) in vec3 In_Pos;
layout(location = 1) in vec3 In_Color;
layout(location = 2) in vec2 In_TexCoord;

layout(binding = 0) uniform CameraData {
  mat4 model;
  mat4 view;
  mat4 projection;
  vec3 pos;
  vec3 up;
  vec3 right;
  vec3 direction;
}
camera;

layout(location = 0) out vec3 Frag_Color;
layout(location = 1) out vec2 Frag_TexCoord;

void main() {
  gl_Position = camera.projection * camera.view * camera.model * vec4(In_Pos, 1.0);
  Frag_Color = In_Color;
  Frag_TexCoord = In_TexCoord;
}