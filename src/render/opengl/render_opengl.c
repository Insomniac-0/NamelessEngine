internal void
r_ogl_load_functions()
{
	glewInit();
}

internal void
r_ogl_init(CmdLine *cmdline)
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


internal void
r_ogl_debug_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
	Temp scratch = scratch_begin(0, 0);
	fprintf(stderr, "[OpenGL] %.*s\n", (int)length, message);
}



// Hooks

internal void r_init(CmdLine *cmdline)
{
	r_ogl_init(cmdline);
}
