#version 450 core

layout(location = 0) in vec2 tex_coord;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D tex;

float step = 0.2;

vec3 green = vec3(0.0, 1.0, 0.0);
vec3 red = vec3(1.0, 0.0, 0.0);

vec3 sm(vec3 a, vec3 b, float val)
{
	val = (3 * val * val) - (2 * val * val);
	val = min(1.0, max(0.0, val));
	return (1 - val) * a + val * b;
}

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
		float value = max(0.2,texture(tex, tex_coord).r);
		outColor = vec4(sm(green, red, value), 1.0) * value;
	}
	//outColor = texture(tex, tex_coord);
}