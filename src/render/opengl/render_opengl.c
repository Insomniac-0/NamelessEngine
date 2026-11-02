internal void
r_ogl_load_functions()
{
	glewInit();
}

internal void
r_ogl_init(CmdLine *cmdline)
{
	glViewport(0, 0, W_WIDTH, W_HEIGHT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(r_ogl_debug_message_callback, 0);
	glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_ERROR, GL_DEBUG_SEVERITY_HIGH, 0, 0, GL_TRUE);
}


internal void
r_ogl_debug_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
	Temp scratch = scratch_begin(0, 0);
	fprintf(stderr, "[OpenGL] %.*s\n", (int)length, message);
}





