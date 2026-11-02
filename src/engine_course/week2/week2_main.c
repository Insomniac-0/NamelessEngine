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


	for (;!quit;)
	{

		Vec3F32 roof_color = space_is_down ? orange : green;

		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}
