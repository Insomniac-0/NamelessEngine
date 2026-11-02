#version 430 core

layout (location = 0) in vec3 i2v_vpos;
layout (location = 1) in vec3 i2v_color;


out vec3 v2f_color;

uniform mat3 u_mat3;



void main()
{

	vec3 pos = u_mat3 * vec3(i2v_vpos.xy, 1.0f);
	v2f_color = i2v_color;

	gl_Position = vec4(pos.xy, 0.0, 1.0f);
}
