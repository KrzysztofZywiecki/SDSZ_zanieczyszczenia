#version 450 core

layout(location = 0) out vec3 color;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 colour;

layout(binding = 0, set = 0) uniform UniformBufferObject {
    vec3 offset;
} offset;

void main()
{
    gl_Position = vec4(position + offset.offset, 1.0f);
    color = colour;
}