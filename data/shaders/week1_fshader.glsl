#version 430 core

in vec3 v2f_color;


uniform vec3 u_color;

out vec4 final_color;

void main()
{
	vec4 color = vec4(1.0f);
	final_color = vec4(u_color, 1.0f);
}
