#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 color;
layout (location = 3) in vec2 uv;

out vec3 outColor;
out vec2 outUv;
out vec3 outNormal;
out vec3 curPos;

uniform mat4 camera;
uniform mat4 model;

void main() {
  curPos = vec3(model * vec4(pos, 1.0f));
  gl_Position = camera * vec4(curPos, 1.0f);
  outColor = color;
  outUv = uv;
  outNormal = normal;
}