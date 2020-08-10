#version 400

layout(vertices = 4) out;

uniform vec3 eyePoint;

in vec3 worldPos[];
in vec2 uv[];
in vec3 worldN[];

out vec3 esInWorldPos[];
out vec2 esInUv[];
out vec3 esInN[];

float getTessLevel(float d) {
  if (d <= 2.0) {
    return 8.0;
  } else if (d <= 5.0) {
    return 4.0;
  } else {
    return 2.0;
  }
}

void main() {
  esInUv[gl_InvocationID] = uv[gl_InvocationID];
  esInN[gl_InvocationID] = worldN[gl_InvocationID];
  esInWorldPos[gl_InvocationID] = worldPos[gl_InvocationID];

  if (gl_InvocationID == 0) {
    vec3 midPoint =
        (worldPos[0] + worldPos[1] + worldPos[2] + worldPos[3]) * 0.25;
    float dist = distance(eyePoint, midPoint);
    float level = getTessLevel(dist);

    gl_TessLevelOuter[0] = level;
    gl_TessLevelOuter[1] = level;
    gl_TessLevelOuter[2] = level;
    gl_TessLevelOuter[3] = level;

    gl_TessLevelInner[0] = level;
    gl_TessLevelInner[1] = level;
  }
}
