#version 460 core
#extension GL_ARB_shading_language_include : require
#extension GL_GOOGLE_include_directive : require

layout(location = 0) in vec2 Frag_Pos;

#include "camera.glsl"

layout(std140, set = 0, binding = 0) uniform CameraBlock {
  Camera camera;
};

#include "util.glsl"

struct TileData {
  vec2 pos;
  uint material;
  bool hovering;
};

layout(std140, set = 2, binding = 0) readonly buffer tiles_data {
  TileData tiles[];
};

layout(location = 0) out vec2 Frag_TexCoord;
layout(location = 1) out vec3 World_Pos;
layout(location = 2) out uint Tile_Index;

void main() {
  TileData tile = tiles[gl_InstanceIndex];
  mat4 model = translate(vec3(tile.pos, 1.0f));
  mat4 inverse_model = inverse(model);

  vec3 world = vec3(model * vec4(Frag_Pos, 1.1, 1.0f));
  World_Pos = world;
  Tile_Index = gl_InstanceIndex;

  gl_Position = camera.projection * camera.view * vec4(world, 1.0f);

  if (gl_VertexIndex == 0) {
    Frag_TexCoord = vec2(0.0f, 0.0f);
  } else if (gl_VertexIndex == 1) {
    Frag_TexCoord = vec2(1.0f, 0.0f);
  } else if (gl_VertexIndex == 2) {
    Frag_TexCoord = vec2(1.0f, 1.0f);
  } else if (gl_VertexIndex == 3) {
    Frag_TexCoord = vec2(0.0f, 1.0f);
  }
}