#version 450 core

layout(location = 0) in vec3 color;
layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 1) uniform sampler2D tex;

void main()
{
	outColor = texture(tex, vec2(color.x, color.y));
}