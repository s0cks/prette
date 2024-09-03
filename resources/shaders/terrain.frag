#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

layout (std140) uniform Camera {
  vec4 pos;
  mat4 projection;
  mat4 view;
} camera;

struct Material {
  sampler2D albedo;
  sampler2D ao;
  sampler2D metallic;
  sampler2D normal;
  sampler2D roughness;
};

uniform vec3 lightColor;
uniform vec3 lightPos;
in vec2 vUv;
in vec3 vColor;
in vec3 vPos;
in vec3 vNormal;

uniform Material material;

const float PI = 3.14159265359;

vec3 getNormalFromMap() {
  vec3 tangentNormal = texture(material.normal, vUv).xyz * 2.0 - 1.0;

  vec3 Q1  = dFdx(vPos);
  vec3 Q2  = dFdy(vPos);
  vec2 st1 = dFdx(vUv);
  vec2 st2 = dFdy(vUv);

  vec3 N   = normalize(vNormal);
  vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
  vec3 B  = -normalize(cross(N, T));
  mat3 TBN = mat3(T, B, N);

  return normalize(TBN * tangentNormal);
}

float DistributionGGX(vec3 N, vec3 H, float roughness) {
  float a = roughness*roughness;
  float a2 = a*a;
  float NdotH = max(dot(N, H), 0.0);
  float NdotH2 = NdotH*NdotH;

  float nom   = a2;
  float denom = (NdotH2 * (a2 - 1.0) + 1.0);
  denom = PI * denom * denom;

  return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
  float r = (roughness + 1.0);
  float k = (r*r) / 8.0;

  float nom   = NdotV;
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
  vec3 albedo     = pow(texture(material.albedo, vUv).rgb, vec3(2.2));
  float metallic  = texture(material.metallic, vUv).r;
  float roughness = texture(material.roughness, vUv).r;
  float ao        = texture(material.ao, vUv).r;

  vec3 N = getNormalFromMap();
  vec3 V = normalize(camera.pos.xyz - vPos);

  // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
  // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
  vec3 F0 = vec3(0.04); 
  F0 = mix(F0, albedo, metallic);

  // reflectance equation
  vec3 Lo = vec3(0.0);
  // calculate per-light radiance
  vec3 L = normalize(lightPos - vPos);
  vec3 H = normalize(V + L);
  float distance = length(lightPos - vPos);
  float attenuation = 1.0 / (distance * distance);
  vec3 radiance = lightColor * attenuation;

  // Cook-Torrance BRDF
  float NDF = DistributionGGX(N, H, roughness);   
  float G   = GeometrySmith(N, V, L, roughness);      
  vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
      
  vec3 numerator    = NDF * G * F; 
  float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
  vec3 specular = numerator / denominator;
  
  // kS is equal to Fresnel
  vec3 kS = F;
  vec3 kD = vec3(1.0) - kS;
  kD *= 1.0 - metallic;	  
  float NdotL = max(dot(N, L), 0.0);        
  Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
  vec3 ambient = vec3(0.03) * albedo * ao;
  vec3 color = ambient + Lo;
  color = color / (color + vec3(1.0));
  color = pow(color, vec3(1.0/2.2)); 

  float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
  if(brightness > 1.0)
    BrightColor = vec4(FragColor.rgb, 1.0);
	else
		BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
  FragColor = vec4(color, 1.0);
}