#version 450 core

layout(location = 0) in vec2 tex_coord;
layout(location = 0) out vec4 outColor;

void main()
{
	outColor = vec4(tex_coord, 0.0f, 1.0f);
}