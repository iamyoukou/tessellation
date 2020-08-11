#version 330

layout(location = 0) in vec3 vtxCoord;
layout(location = 1) in vec2 texUv;
layout(location = 2) in vec3 vtxN;

out vec2 uv;
out vec3 worldPos;
out vec3 worldN;

uniform mat4 M;
// uniform vec2 numQuads, quadIdx;

void main() {
  // float quadSize = 1.0 / numQuads.x;
  //
  // uv = texUv * quadSize + quadIdx * quadSize;
  uv = texUv;

  worldPos = (M * vec4(vtxCoord, 1.0)).xyz;

  worldN = (vec4(vtxN, 1.0) * inverse(M)).xyz;
  worldN = normalize(worldN);
}
