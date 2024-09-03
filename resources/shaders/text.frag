#version 330 core
out vec4 Frag_Color;

uniform sampler2D text;
in vec2 TexCoords;

void main() {
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
    Frag_Color = vec4(0.0f, 0.0f, 0.0f, 1.0f) * sampled;
}