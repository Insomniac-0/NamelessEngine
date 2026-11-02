// External Includes

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Layer Includes [H]
#include "base/base_inc.h"
#include "render/render_core.h"
#include "week1.h"

// Layer Includes [C]
#include "base/base_inc.c"
#include "week1.c"


#include "generated/engine_course_week1.meta.h"


// Globals

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



internal void
entry_point(CmdLine *cmdline)
{

	Temp scratch = scratch_begin(0, 0);
	// glfw
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);  
	glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
	glfwWindowHint(GLFW_MAXIMIZED, GLFW_FALSE);

	GLFWwindow* window = 0;
	window = glfwCreateWindow(W_WIDTH, W_HEIGHT, "Week1", NULL, NULL);

	if (window == NULL)
	{
		fprintf(stderr, "Failed to create GLFW window\n");
		glfwTerminate();
		return;
	}

	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);


	ogl_init();


	Vec2F32 ndcpos 	= V2F32(0.0f, -0.3f);
	Vec2F32 scale		= V2F32(0.5f, 0.5f);
	F32			turns		= 0.0f;

	Vec3F32 red			= V3F32(1.0f, 0.0f, 0.0f);
	Vec3F32 green		= V3F32(0.0f, 1.0f, 0.0f);
	Vec3F32 blue		= V3F32(0.0f, 0.0f, 1.0f);
	Vec3F32 orange  = V3F32(1.0f, 0.5f, 0.0f);

	Vec2F32 t_ndcpos = V2F32(0.0f, 0.0f);


	Vec2F32 w_scale =	V2F32(0.1f, 0.1f);
	Vec2F32 w0_off = V2F32( 0.3f,  0.1f);
	Vec2F32 w1_off = V2F32(-0.3f, 0.1f);

	Vec2F32 w0_pos = Add2F32(ndcpos, w0_off);
	Vec2F32 w1_pos = Add2F32(ndcpos, w1_off);

	Vec2F32 d_scale = Mul2F32(scale, V2F32(0.2f, 0.5f));
	Vec2F32 d_off = V2F32(0.0f, -0.25f);
	Vec2F32 d_pos = Add2F32(ndcpos, d_off);


	for (;!quit;)
	{

		Vec3F32 roof_color = space_is_down ? orange : green;

		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		ogl_draw_triangle(t_ndcpos, turns+0.625, scale, roof_color);

		ogl_draw_rectangle(w0_pos, turns, w_scale, blue); 
		ogl_draw_rectangle(w1_pos, turns, w_scale, blue); 

		ogl_draw_rectangle(d_pos, turns, d_scale, blue); 

		ogl_draw_rectangle(ndcpos, turns, scale, red); 

		//ogl_draw_rectangle(ndcpos, turns+0.25f, scale, green); 

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	scratch_end(scratch);
}

