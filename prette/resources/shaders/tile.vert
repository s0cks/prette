#version 460 core
#extension GL_ARB_shading_language_include : require
#extension GL_GOOGLE_include_directive : require

#include "camera.glsl"

layout(location = 0) in vec2 In_Pos;
layout(location = 1) in uvec4 In_Color;

layout(std140, binding = 0) uniform CameraBlock {
  Camera camera;
};

layout(std140, binding = 1) uniform ChunkData {
  vec2 chunk_pos;
}
chunk;

struct TileData {
  vec2 pos;
  uint material;
  mat4 model;
  bool hovering;
};

layout(binding = 2) uniform tiles_data {
  TileData tiles[32];
};

layout(location = 0) out uint Frag_Material;

void main() {
  TileData tile = tiles[gl_InstanceIndex];
  gl_Position = camera.projection * camera.view * tile.model * vec4(In_Pos.xy, 1.1f, 1.0f);
  if (tile.hovering) {
    Frag_Material = 3;
  } else {
    Frag_Material = tile.material;
  }
}