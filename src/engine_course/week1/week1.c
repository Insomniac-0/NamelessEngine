internal void ogl_load_functions()
{
	glewInit();
}

internal void ogl_init()
{
	ogl_load_functions();

	Arena *arena = arena_alloc();
	OGL_State *state = push_struct(arena, OGL_State);
	state->arena = arena;

	glViewport(0, 0, W_WIDTH, W_HEIGHT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(ogl_debug_message_callback, 0);
	glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_ERROR, GL_DEBUG_SEVERITY_HIGH, 0, 0, GL_TRUE);

	Temp scratch = scratch_begin(0, 0);

  String8 build_dir_path   = get_process_info()->binary_path;
  String8 data_dir_path    = str8f(scratch.arena, "%S/data", build_dir_path);
	String8 shaders_dir_path  = str8f(scratch.arena, "%S/shaders", data_dir_path);

	String8 vshader_path = str8f(scratch.arena, "%S/week1_vshader.glsl", shaders_dir_path);
	String8 fshader_path = str8f(scratch.arena, "%S/week1_fshader.glsl", shaders_dir_path);

	printf("[Path Loaded]:    %s\n", build_dir_path.str);
	printf("[Path Loaded]:    %s\n", data_dir_path.str);
	printf("[Path Loaded]:    %s\n", shaders_dir_path.str);

	printf("[Shader Loaded]: %s\n", vshader_path.str);
	printf("[Shader Loaded]: %s\n", fshader_path.str);

	week1_vshad_src = shader_source_from_file_path(state->arena, vshader_path);
	week1_fshad_src = shader_source_from_file_path(state->arena, fshader_path);

	scratch_end(scratch);


	// Build Shaders
	// Compile
	struct {GLenum type; String8 *src; GLuint out; String8 errors;} stages[] =
	{
		{GL_VERTEX_SHADER, &week1_vshad_src},
		{GL_FRAGMENT_SHADER, &week1_fshad_src},
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

	// link / validate / store
	glLinkProgram(program);
	glValidateProgram(program);
	state->default_program = program;


	glUseProgram(program);

	GLuint vao;
	GLuint vbo;

	// default vao
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	state->default_vao = vao;

	// default vbo
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vbuffer), triangle_vbuffer, GL_STATIC_DRAW);


	U64 off = 0;
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(F32), (void *)(off));
	off += 6*sizeof(F32);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(F32), (void *)(off));
	state->default_vbo = vbo;
	off += 6*sizeof(F32);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	ogl_state = state;
}

internal void ogl_draw_triangle(Vec2F32 position, F32 turns, Vec2F32 scale, Vec3F32 color)
{
	Mat3x3F32 trs = {0};

	Mat3x3F32 t = MakeTranslate3x3F32(position);
	Mat3x3F32 r = MakeRotate3x3F32(turns);
	Mat3x3F32 s = MakeScale3x3F32(scale);

	Vec3F32 u_color = color;

	trs = Mul3x3F32(t, r);
	trs = Mul3x3F32(trs, s);


	glBindVertexArray(ogl_state->default_vao);
	glUseProgram(ogl_state->default_program);

	glUniformMatrix3fv(glGetUniformLocation(ogl_state->default_program, "u_mat3"), 1, 0, &trs.elements[0][0]);
	glUniform3fv(glGetUniformLocation(ogl_state->default_program, "u_color"), 1, &u_color.v[0]);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glBindVertexArray(0);
	glUseProgram(0);

}

internal void ogl_draw_rectangle(Vec2F32 position, F32 turns, Vec2F32 scale, Vec3F32 color)
{
	ogl_draw_triangle(position, turns, scale, color);
	ogl_draw_triangle(position, turns+0.5f, scale, color);
}

// callbacks

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

internal void
ogl_debug_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
	Temp scratch = scratch_begin(0, 0);
	fprintf(stderr, "[OpenGL] %.*s\n", (int)length, message);
}
