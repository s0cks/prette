struct Camera {
  vec2 viewport_size;
  mat4 projection;
  mat4 view;
  vec3 pos;
  vec3 up;
  vec3 right;
  vec3 direction;
  float zoom;
  float speed;
};