#version 410 core

layout(triangles, equal_spacing, ccw) in;

uniform mat4 V, P;
uniform sampler2D texHeight;

in vec3 esInWorldPos[];
in vec2 esInUv[];
in vec3 esInN[];

out vec3 worldPos;
out vec2 uv;
out vec3 worldN;

vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2) {
  return vec2(gl_TessCoord.x) * v0 + vec2(gl_TessCoord.y) * v1 +
         vec2(gl_TessCoord.z) * v2;
}

vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2) {
  return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 +
         vec3(gl_TessCoord.z) * v2;
}

void main() {
  // Interpolate the attributes of the output vertex using the barycentric
  // coordinates
  uv = interpolate2D(esInUv[0], esInUv[1], esInUv[2]);
  worldN = interpolate3D(esInN[0], esInN[1], esInN[2]);
  worldN = normalize(worldN);
  worldPos = interpolate3D(esInWorldPos[0], esInWorldPos[1], esInWorldPos[2]);

  // Displace the vertex along the normal
  // float scale = 0.1;
  // float offset = texture(texHeight, uv).r * 2.0 - 1.0;
  // worldPos.y += offset * scale;

  gl_Position = P * V * vec4(worldPos, 1.0);
}
