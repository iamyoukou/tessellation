#version 410 core

// define the number of CPs in the output patch
layout(vertices = 3) out;

uniform vec3 eyePoint;

// attributes of the input CPs
in vec3 worldPos[];
in vec2 uv[];
in vec3 worldN[];

// attributes of the output CPs
out vec3 esInWorldPos[];
out vec2 esInUv[];
out vec3 esInN[];

float GetTessLevel(float Distance0, float Distance1) {
  float AvgDistance = (Distance0 + Distance1) / 2.0;

  if (AvgDistance <= 2.0) {
    return 10.0;
  } else if (AvgDistance <= 5.0) {
    return 7.0;
  } else {
    return 3.0;
  }
}

void main() {
  // Set the control points of the output patch
  esInUv[gl_InvocationID] = uv[gl_InvocationID];
  esInN[gl_InvocationID] = worldN[gl_InvocationID];
  esInWorldPos[gl_InvocationID] = worldPos[gl_InvocationID];

  // Calculate the distance from the camera to the three control points
  // to not do same stuff 3 times
  if (gl_InvocationID == 0) {
    float EyeToVertexDistance0 = distance(eyePoint, esInWorldPos[0]);
    float EyeToVertexDistance1 = distance(eyePoint, esInWorldPos[1]);
    float EyeToVertexDistance2 = distance(eyePoint, esInWorldPos[2]);

    // Calculate the tessellation levels
    gl_TessLevelOuter[0] =
        GetTessLevel(EyeToVertexDistance1, EyeToVertexDistance2);
    gl_TessLevelOuter[1] =
        GetTessLevel(EyeToVertexDistance2, EyeToVertexDistance0);
    gl_TessLevelOuter[2] =
        GetTessLevel(EyeToVertexDistance0, EyeToVertexDistance1);
    gl_TessLevelInner[0] = gl_TessLevelOuter[2];
  }
}
