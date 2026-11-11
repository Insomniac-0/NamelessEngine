#version 430 core

in vec3 v2f_color;

out vec4 final_color;


void main()
{
	final_color = vec4(v2f_color, 1.0f);
}
