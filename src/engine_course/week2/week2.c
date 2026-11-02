void
framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void
key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	switch (key)
	{
		default:{}break;
		case GLFW_KEY_ESCAPE:
		{
			quit = 1;
		}
		case GLFW_KEY_SPACE:
		{
			if (action == GLFW_PRESS)		{ space_is_down = 1; }
			if (action == GLFW_RELEASE) { space_is_down = 0; }
		}break;
	}
}
