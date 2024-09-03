#version 330 core
out vec4 FragColor;

in vec3 outColor;
in vec2 outUv;
in vec3 outNormal;
in vec3 curPos;

uniform sampler2D tex0;
uniform vec4 lightColor;
uniform vec3 lightPos;
uniform vec3 cameraPos;

void main() {
  float ambient = 0.20f;
  vec3 normal = normalize(outNormal);
  vec3 lightDirection = normalize(lightPos - curPos);
  float diffuse = max(dot(outNormal, lightDirection), 0.0f);

  float specularLight = 0.50f;
  vec3 viewDirection = normalize(camPos - curPos);
  vec3 reflectionDirection = reflect(-lightDirection, outNormal);
  float specularAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 8);
  float specular = specularAmount * specularLight;

  FragColor = texture(tex0, outUv) * lightColor * (diffuse + ambient + specular);
}