#version 330

in vec2 uv;
in vec3 worldPos;
in vec3 worldN;

out vec4 outputColor;

uniform vec3 lightPosition;

void main()
{
    vec3 L = normalize(lightPosition - worldPos);
    outputColor = vec4(max(dot(worldN, L), 0.0));
}
