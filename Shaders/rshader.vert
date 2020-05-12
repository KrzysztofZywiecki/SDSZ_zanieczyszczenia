#version 450 core

layout(location = 0) out vec2 tex_coord;

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in mat4 transform;
layout(location = 6) in vec2 textureScale;
layout(location = 7) in vec2 textureOffset;

void main()
{
    gl_Position = transform * vec4(position, 1.0f);
    tex_coord = vec2(texCoord.x * textureScale.x, texCoord.y * textureScale.y) + textureOffset;
}