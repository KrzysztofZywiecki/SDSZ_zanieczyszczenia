#version 450 core

layout(location = 0) in vec2 tex_coord;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D tex;

float step = 0.2;

void main()
{
	float x_samp1 = ceil(texture(tex, tex_coord + vec2(0.0015, 0.0)).r/step);
	float x_samp2 = ceil(texture(tex, tex_coord + vec2(-0.0015, 0.0)).r/step);
	float y_samp1 = ceil(texture(tex, tex_coord + vec2(0.0, 0.0015)).r/step);
	float y_samp2 = ceil(texture(tex, tex_coord + vec2(0.0, -0.0015)).r/step);

	if((x_samp1 - x_samp2) != 0)
	{
		outColor = vec4(1.0, 1.0, 1.0, 1.0);
	}
	else if((y_samp1 - y_samp2) != 0)
	{
		outColor = vec4(1.0, 1.0, 1.0, 1.0);	
	}	
	else
	{
		outColor = texture(tex, tex_coord);
	}
}