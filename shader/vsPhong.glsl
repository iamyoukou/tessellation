#version 330

layout(location = 0) in vec3 vtxCoord;
layout(location = 1) in vec2 texUv;
layout(location = 2) in vec3 vtxN;

out vec2 uv;
out vec3 worldPos;
out vec3 worldN;

uniform mat4 M;

void main()
{
    uv = texUv;
    worldPos = (M * vec4(vtxCoord, 1.0)).xyz;
    worldN = normalize((vec4(vtxN, 1.0) * inverse(M)).xyz);
}
