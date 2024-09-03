#version 330 core
out vec4 FragColor;

uniform samplerCube tex;
in vec3 vPos;

void main() {
  FragColor = texture(tex, vPos);
}