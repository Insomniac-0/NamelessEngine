// External Includes
#include <GL/glew.h>
#include <GLFW/glfw3.h>


// Layer Includes [H]
#include "base/base_inc.h"
#include "render/render_inc.h"
#include "week2.h"

// Layer Includes [C]
#include "base/base_inc.c"
#include "render/render_inc.c"
#include "week2.c"


// None generated right now
/* #include "generated/engine_course_week2.meta.h" */
/* #include "generated/engine_course_week2.meta.c" */


// defines & globals
#define W_WIDTH 1280
#define W_HEIGHT 960


internal void
entry_point(CmdLine *cmdline)
{
	// This should eventually be replaced with abstraction layer for window handling
	// and renderer will have a way to "attach" to the window
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);  
	glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
	glfwWindowHint(GLFW_MAXIMIZED, GLFW_FALSE);

	GLFWwindow* window = 0;
	window = glfwCreateWindow(W_WIDTH, W_HEIGHT, "Week2", NULL, NULL);

	if (window == NULL)
	{
		fprintf(stderr, "Failed to create GLFW window\n");
		glfwTerminate();
		return;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);

	r_ogl_load_functions();
	r_ogl_init(cmdline);

	/* Arena *arena = arena_alloc(); */
	/* OGL_State *state = push_struct(arena, OGL_State); */
	/* state->arena = arena; */

	//

	// Build Shaders
	// Compile
	/* struct {GLenum type; String8 *src; GLuint out; String8 errors;} stages[] = */
	/* { */
	/* 	{GL_VERTEX_SHADER, &week1_vshad_src}, */
	/* 	{GL_FRAGMENT_SHADER, &week1_fshad_src}, */
	/* }; */

	/* for EachElement(idx, stages) */
	/* { */
	/* 	stages[idx].out = glCreateShader(stages[idx].type); */
	/* 	GLint src_size = stages[idx].src->size; */
	/* 	glShaderSource(stages[idx].out, 1, (char **)&stages[idx].src->str, &src_size); */
	/* 	glCompileShader(stages[idx].out); */
	/* 	GLint info_log_length = 0; */
	/* 	GLint status = 0; */
	/* 	glGetShaderiv(stages[idx].out, GL_COMPILE_STATUS, &status); */
	/* 	glGetShaderiv(stages[idx].out, GL_INFO_LOG_LENGTH, &info_log_length); */

	/* 	if (info_log_length != 0) */
	/* 	{ */
	/* 		stages[idx].errors.str = push_array(scratch.arena, U8, info_log_length+1); */
	/* 		stages[idx].errors.size = info_log_length; */
	/* 		glGetShaderInfoLog(stages[idx].out, info_log_length, 0, (char *)stages[idx].errors.str); */
	/* 		fprintf(stderr, "%s\n", stages[idx].errors.str); */
	/* 	} */
	/* } */

	// Attach compilations
	/* GLuint program = glCreateProgram(); */
	/* for EachElement(idx, stages) */
	/* { */
	/* 	glAttachShader(program, stages[idx].out); */
	/* } */




	// link / validate / store
	/* glLinkProgram(program); */
	/* glValidateProgram(program); */
	/* state->default_program = program; */


	/* glUseProgram(program); */

	/* GLuint vao; */
	/* GLuint vbo; */

	/* // default vao */
	/* glGenVertexArrays(1, &vao); */
	/* glBindVertexArray(vao); */
	/* state->default_vao = vao; */

	/* // default vbo */
	/* glGenBuffers(1, &vbo); */
	/* glBindBuffer(GL_ARRAY_BUFFER, vbo); */
	/* //glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vbuffer), triangle_vbuffer, GL_STATIC_DRAW); */


	/* U64 off = 0; */
	/* glEnableVertexAttribArray(0); */
	/* glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(F32), (void *)(off)); */
	/* off += 6*sizeof(F32); */

	/* glEnableVertexAttribArray(1); */
	/* glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(F32), (void *)(off)); */
	/* state->default_vbo = vbo; */
	/* off += 6*sizeof(F32); */

	/* glBindBuffer(GL_ARRAY_BUFFER, 0); */

	/* ogl_state = state; */




	for (;!quit;)
	{


		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}
