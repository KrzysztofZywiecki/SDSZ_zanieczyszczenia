#version 450 core

layout(location = 0) out vec2 tex_coord;

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in mat4 transform;

void main()
{
    gl_Position = transform * vec4(position, 1.0f);
    tex_coord = texCoord;
}