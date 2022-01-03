#version 330
in vec3 fragColor;
out vec4 outColor;

void main()
{
    // ---------------------------------------
    // Draw round point
    // ---------------------------------------
    // gl_PointCoord contains the coordinate of a fragment within a point
    // On display, a point is a NxN square
    // Discard points that are outside a unit circle to make a approximate round point

    // Convert point to a unit cirlce: [0, 1] -> [-1, 1]
    vec2 unitCirclePos = 2.0 * gl_PointCoord - 1.0;

    // Compute distance between point and circle center
    // Based on the distance, reserve or discard the point fragment
    if (dot(unitCirclePos, unitCirclePos) > 1.0)
    {
        discard;
    }

    outColor = vec4(fragColor, 1.0);
}
