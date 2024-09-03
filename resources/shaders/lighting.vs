#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 color;
layout (location = 3) in vec2 uv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 FragNormal;

void main() {
  FragPos = vec3(model * vec4(pos, 1.0f));
  FragNormal = mat3(transpose(inverse(model))) * normal;
  gl_Position = projection * view * vec4(FragPos, 1.0f);
}