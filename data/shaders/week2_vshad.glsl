#version 430 core

in vec3 i2v_pos;
in vec3 i2v_color;

out vec3 v2f_color;

uniform mat4 u_transform;
uniform mat4 u_projection;
uniform mat4 u_view;



void main()
{
	v2f_color = i2v_color;
	gl_Position = u_projection * u_view * u_transform * vec4(i2v_pos, 1.0f);
}
