// External Includes

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Layer Includes [H]
#include "base/base_inc.h"
#include "render/render_core.h"

// Layer Includes [C]
#include "base/base_inc.c"


typedef struct OGL_Attribute OGL_Attribute;
struct OGL_Attribute
{
	U64 index;
	String8 name;
	GLenum type;
	U64 count;
};

typedef struct OGL_AttributeArray OGL_AttributeArray;
struct OGL_AttributeArray
{
	OGL_Attribute *v;
	U64 count;
};


#include "generated/engine_course_week1.meta.h"


// Globals
global B32 quit = 0;

global read_only OGL_Attribute g_ogl_triangle_input_attributes[] =
{
	{0,	str8_lit_comp("i2v_pos"),				GL_FLOAT, 4},
	{1, str8_lit_comp("i2v_color"),			GL_FLOAT, 4},
};

global read_only OGL_Attribute g_ogl_single_color_output_attributes[] =
{
	{0, str8_lit_comp("final_color")},
};


#include "generated/engine_course_week1.meta.c"

#define W_WIDTH 800
#define W_HEIGHT 800


// Callbacks
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	switch (key)
	{
		default:{}break;
		case GLFW_KEY_ESCAPE:
		{
			quit = 1;
		}
	}
}

internal void
ogl_debug_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
	fprintf(stderr, "[OpenGL] %.*s\n", (int)length, message);
}



internal void
entry_point(CmdLine *cmdline)
{
	Temp scratch = scratch_begin(0, 0);
  String8 build_dir_path   = get_process_info()->binary_path;
  String8 data_dir_path    = str8f(scratch.arena, "%S/data", build_dir_path);
	String8 shaders_dir_path  = str8f(scratch.arena, "%S/shaders", data_dir_path);

	printf("[ PATH LOADED ]:    %s\n", build_dir_path.str);
	printf("[ PATH LOADED ]:    %s\n", data_dir_path.str);
	printf("[ PATH LOADED ]:    %s\n", shaders_dir_path.str);

	glfwInit();


	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
	glfwWindowHint(GLFW_MAXIMIZED, GLFW_FALSE);

	GLFWwindow* window = 0;

	window = glfwCreateWindow(W_WIDTH, W_HEIGHT, "DotsEngine", NULL, NULL);

	if (window == NULL)
	{
		fprintf(stderr, "Failed to create GLFW window\n");
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);

	glewInit();

	glViewport(0, 0, W_WIDTH, W_HEIGHT);
	glEnable(GL_DEPTH_TEST);

  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(ogl_debug_message_callback, 0);



	// Build Shaders

	// Compile
	struct {GLenum type; String8 *src; GLuint out; String8 errors;} stages[] =
	{
		{GL_VERTEX_SHADER, g_ogl_shader_kind_vshad_src_table[0]},
		{GL_FRAGMENT_SHADER, g_ogl_shader_kind_fshad_src_table[0]},
	};

	for EachElement(idx, stages)
	{
		stages[idx].out = glCreateShader(stages[idx].type);
		GLint src_size = stages[idx].src->size;
		glShaderSource(stages[idx].out, 1, (char **)&stages[idx].src->str, &src_size);
		glCompileShader(stages[idx].out);
		GLint info_log_length = 0;
		GLint status = 0;
		glGetShaderiv(stages[idx].out, GL_COMPILE_STATUS, &status);
		glGetShaderiv(stages[idx].out, GL_INFO_LOG_LENGTH, &info_log_length);

		if (info_log_length != 0)
		{
			stages[idx].errors.str = push_array(scratch.arena, U8, info_log_length+1);
			stages[idx].errors.size = info_log_length;
			glGetShaderInfoLog(stages[idx].out, info_log_length, 0, (char *)stages[idx].errors.str);
			fprintf(stderr, "%s\n", stages[idx].errors.str);
		}
	}

	// Attach compilations
	GLuint program = glCreateProgram();
	for EachElement(idx, stages)
	{
		glAttachShader(program, stages[idx].out);
	}

	// Bind inputs
	OGL_AttributeArray inputs = g_ogl_shader_kind_input_attributes_table[0];
	for EachIndex(idx, inputs.count) 
	{
		glBindAttribLocation(program, inputs.v[idx].index, (char *)inputs.v[idx].name.str);
	}

	// Bind outputs
	OGL_AttributeArray outputs = g_ogl_shader_kind_output_attributes_table[0];
	for EachIndex(idx, outputs.count)
	{
		glBindFragDataLocation(program, outputs.v[idx].index, (char *)outputs.v[idx].name.str);
	}



	// link / validate / store
	glLinkProgram(program);
	glValidateProgram(program);



	// buffer
	F32 vertex_buffer[] =
	{
//    X			 Y			Z			 W		  R      G      B      A
		-0.5f, -0.5f,  0.0f,  1.0f,	 1.0f,  0.0f,	 0.0f,  1.0f,
		 0.5f, -0.5f,  0.0f,  1.0f,	 0.0f,  1.0f,	 0.0f,  1.0f,
		 0.0f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,	 1.0f,  1.0f,
	};


	U32 VBO;
	glGenBuffers(1, &VBO);

	U32 VAO;
	glGenVertexArrays(1, &VAO);


	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer), vertex_buffer, GL_STATIC_DRAW);

	
	// bind input attributes
	{
		OGL_AttributeArray inputs = g_ogl_shader_kind_input_attributes_table[0];
		U64 off = 0;
		for EachIndex(idx, inputs.count)
		{
			glEnableVertexAttribArray(inputs.v[idx].index);
			glVertexAttribDivisor(inputs.v[idx].index, 0);
			glVertexAttribPointer(inputs.v[idx].index, inputs.v[idx].count, inputs.v[idx].type, GL_FALSE, 8*sizeof(F32), (void *)(off));
			off += inputs.v[idx].count*sizeof(F32);
		}
	}

	Mat4x4F32 mat = MakeMat4x4F32(1.0f);
	// Uniform
	{
		glUniformMatrix4fv(glGetUniformLocation(program, "u_mat4"), 1, 0, &mat.v[0][0]);
	}

	for (;!quit;)
	{


		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(program);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	scratch_end(scratch);
}
