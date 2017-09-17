#version 120

vec4 qneg(vec4 q) {
  return vec4(-q.xyz, q.w);
}

vec3 qrot(vec3 v, vec4 q) {
  return v + 2.0 * cross(q.xyz, cross(q.xyz, v) + v * q.w);
}

vec4 proj(vec3 mv, vec4 p) {
  return vec4(mv.xy * p.xy, mv.z * p.z + p.w, -mv.z);
}

uniform vec3 cam_vec3;
uniform vec4 cam_quat;
uniform mat3 cam_m3x3;
uniform vec4 cam_proj;

attribute vec3 vertex_pos;

void main() {
  vec3 mv = vertex_pos;
  mv -= cam_vec3;
  // mv *= inverse(cam_m3x3);
  mv = qrot(mv, qneg(cam_quat));
  gl_Position = proj(mv, cam_proj);
}
