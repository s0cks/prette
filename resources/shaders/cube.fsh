#version 330 core
out vec4 FragColor;

struct Material {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float shininess;
};

struct Light {
  vec3 pos;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

struct Camera {
  vec3 pos;
};

uniform sampler2D tex;
uniform Camera camera;
uniform Material material;
uniform Light light;

in vec3 vPos;
in vec3 vNormal;
in vec2 vUv;
in vec3 vColor;

void main() {
  vec3 ambient = light.ambient * material.ambient;
  vec3 lightDir = normalize(light.pos - vPos);
  float diff = max(dot(vNormal, lightDir), 0.0f);
  vec3 diffuse = light.diffuse * (diff * material.diffuse);

  vec3 viewDir = normalize(camera.pos - vPos);
  vec3 reflectDir = reflect(-lightDir, vNormal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
  vec3 specular = light.specular * (spec * material.specular);
  
  FragColor = texture(tex, vUv) * vec4(vColor, 1.0f) * vec4(ambient + diffuse + specular, 1.0f);
}