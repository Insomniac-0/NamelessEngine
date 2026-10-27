//- GENERATED CODE

#ifndef ENGINE_COURSE_WEEK1_META_H
#define ENGINE_COURSE_WEEK1_META_H

typedef enum OGL_ShaderKind
{
OGL_ShaderKind_Triangle,
OGL_ShaderKind_COUNT,
} OGL_ShaderKind;

C_LINKAGE_BEGIN
extern String8 g_ogl_shader_kind_table[1];
extern String8 * g_ogl_shader_kind_vshad_src_table[1];
extern String8 * g_ogl_shader_kind_fshad_src_table[1];
extern OGL_AttributeArray g_ogl_shader_kind_input_attributes_table[1];
extern OGL_AttributeArray g_ogl_shader_kind_output_attributes_table[1];
read_only global String8 g_ogl_triangle_vshad_src =
str8_lit_comp(
""
"\n"
"#version 430 core\n"
"\n"
"in vec4 i2v_pos;\n"
"in vec4 i2v_color;\n"
"\n"
"out vec4 v2f_color;\n"
"\n"
"uniform mat4 u_mat;\n"
"\n"
"\n"
"void main()\n"
"{\n"
"	gl_Position = i2v_pos;\n"
"	v2f_color = i2v_color;\n"
"}\n"
"\n"
""
);

read_only global String8 g_ogl_triangle_fshad_src =
str8_lit_comp(
""
"\n"
"#version 430 core\n"
"\n"
"in vec4 v2f_color;\n"
"out vec4 final_color;\n"
"\n"
"void main()\n"
"{\n"
"	final_color = v2f_color;\n"
"} \n"
""
);


C_LINKAGE_END

#endif // ENGINE_COURSE_WEEK1_META_H
