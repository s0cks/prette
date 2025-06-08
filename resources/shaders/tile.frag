#version 460 core
#extension GL_ARB_shading_language_include : require
#extension GL_GOOGLE_include_directive : require

layout(location = 0) in vec2 Frag_TexCoord;
layout(location = 1) in vec3 World_Pos;
layout(location = 2) in flat uint Tile_Index;

#include "camera.glsl"

layout(std140, set = 0, binding = 0) uniform CameraBlock {
  Camera camera;
};

layout(set = 1, binding = 0) uniform sampler2D albedo;
layout(set = 1, binding = 1) uniform sampler2D normal;
layout(set = 1, binding = 2) uniform sampler2D metallic;
layout(set = 1, binding = 3) uniform sampler2D roughness;
layout(set = 1, binding = 4) uniform sampler2D ao;

layout(location = 0) out vec4 Out_Color;

const vec3 Light_Pos = vec3(0.0, 0.0, 1.1);
const vec3 Light_Color = vec3(1.0f, 1.0f, 1.0f);
const vec3 vNormal = vec3(0.0f, 1.0f, 0.0f);

const float PI = 3.14159265359;

struct TileData {
  vec2 pos;
  uint material;
  bool hovering;
};

layout(std140, set = 2, binding = 0) readonly buffer tiles_data {
  TileData tiles[];
};

vec3 getNormalFromMap() {
  vec3 tangentNormal = texture(normal, Frag_TexCoord).xyz * 2.0 - 1.0;

  vec3 Q1 = dFdx(World_Pos);
  vec3 Q2 = dFdy(World_Pos);
  vec2 st1 = dFdx(Frag_TexCoord);
  vec2 st2 = dFdy(Frag_TexCoord);

  vec3 N = normalize(vNormal);
  vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
  vec3 B = -normalize(cross(N, T));
  mat3 TBN = mat3(T, B, N);

  return normalize(TBN * tangentNormal);
}

float DistributionGGX(vec3 N, vec3 H, float roughness) {
  float a = roughness * roughness;
  float a2 = a * a;
  float NdotH = max(dot(N, H), 0.0);
  float NdotH2 = NdotH * NdotH;

  float nom = a2;
  float denom = (NdotH2 * (a2 - 1.0) + 1.0);
  denom = PI * denom * denom;

  return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
  float r = (roughness + 1.0);
  float k = (r * r) / 8.0;

  float nom = NdotV;
  float denom = NdotV * (1.0 - k) + k;

  return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
  float NdotV = max(dot(N, V), 0.0);
  float NdotL = max(dot(N, L), 0.0);
  float ggx2 = GeometrySchlickGGX(NdotV, roughness);
  float ggx1 = GeometrySchlickGGX(NdotL, roughness);

  return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
  return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main() {
  vec3 albedo = pow(texture(albedo, Frag_TexCoord).rgb, vec3(2.2));
  float metallic = texture(metallic, Frag_TexCoord).r;
  float roughness = texture(roughness, Frag_TexCoord).r;
  float ao = texture(ao, Frag_TexCoord).r;

  vec3 N = getNormalFromMap();
  vec3 V = normalize(camera.pos.xyz - World_Pos);

  // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
  // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
  vec3 F0 = vec3(0.04);
  F0 = mix(F0, albedo, metallic);

  // reflectance equation
  vec3 Lo = vec3(0.0);
  // calculate per-light radiance
  vec3 L = normalize(Light_Pos - World_Pos);
  vec3 H = normalize(V + L);
  float distance = length(camera.pos - World_Pos);
  float attenuation = 1.0 / (distance * distance);
  vec3 radiance = Light_Color * attenuation;

  // Cook-Torrance BRDF
  float NDF = DistributionGGX(N, H, roughness);
  float G = GeometrySmith(N, V, L, roughness);
  vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

  vec3 numerator = NDF * G * F;
  float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
  vec3 specular = numerator / denominator;

  // kS is equal to Fresnel
  vec3 kS = F;
  vec3 kD = vec3(1.0) - kS;
  kD *= 1.0 - metallic;
  float NdotL = max(dot(N, L), 0.0);
  Lo += (kD * albedo / PI + specular) * radiance *
        NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
  vec3 ambient = vec3(0.03) * albedo * ao;
  vec3 color = ambient + Lo;
  color = color / (color + vec3(1.0));
  color = pow(color, vec3(1.0 / 2.2));
  Out_Color = vec4(color, 1.0);
}