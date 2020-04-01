#version 450 core

layout(location = 0) in vec3 color;
layout(location = 0) out vec4 outColor;

void main()
{
    float col = length(color);
    float col1 = sin(col * 10);
    float col2 = cos(col * 10);
    float col3 = -sin(col * 10);
    outColor = vec4(col1, col2, col3, 1.0);
}