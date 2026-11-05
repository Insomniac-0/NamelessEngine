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
		case GLFW_KEY_F1:
		{
		}break;
		case GLFW_KEY_F2:
		{
			if (action == GLFW_PRESS) { ortho_camera = !ortho_camera; }
		}break;
		case GLFW_KEY_F3:
		{
			if (action == GLFW_PRESS) { ogl_toggle_wireframe(); }
		}break;
		case GLFW_KEY_F4:
		{
		}break;
		case GLFW_KEY_F5:
		{
		}break;
		case GLFW_KEY_F6:
		{
		}break;
		case GLFW_KEY_F7:
		{
		}break;
		case GLFW_KEY_F8:
		{
		}break;
		case GLFW_KEY_F9:
		{
		}break;
		case GLFW_KEY_F10:
		{
		}break;
		case GLFW_KEY_F11:
		{
		}break;
		case GLFW_KEY_F12:
		{
			if (action == GLFW_PRESS) { demo_index = MOD(demo_index + 1, DEMO_COUNT); } 
		}break;
		case GLFW_KEY_A:
		{
			if (action == GLFW_PRESS) 	{ key_a_is_down = 1; }
			if (action == GLFW_RELEASE) { key_a_is_down = 0; }
		}break;
		case GLFW_KEY_D:
		{
			if (action == GLFW_PRESS)		{ key_d_is_down = 1; } 
			if (action == GLFW_RELEASE) { key_d_is_down = 0; } 
		}break;
		case GLFW_KEY_W:
		{
			if (action == GLFW_PRESS)		{ key_w_is_down = 1; } 
			if (action == GLFW_RELEASE) { key_w_is_down = 0; } 
		}break;
		case GLFW_KEY_S:
		{
			if (action == GLFW_PRESS)		{ key_s_is_down = 1; } 
			if (action == GLFW_RELEASE) { key_s_is_down = 0; } 
		}break;
		case GLFW_KEY_LEFT_SHIFT:
		{
			if (action == GLFW_PRESS)		{ key_shift_is_down = 1; } 
			if (action == GLFW_RELEASE) { key_shift_is_down = 0; } 
		}break;
		case GLFW_KEY_LEFT:
		{
			if (action == GLFW_PRESS)		{ key_left_is_down = 1; } 
			if (action == GLFW_RELEASE) { key_left_is_down = 0; } 
		}break;
		case GLFW_KEY_RIGHT:
		{
			if (action == GLFW_PRESS)		{ key_right_is_down = 1; } 
			if (action == GLFW_RELEASE) { key_right_is_down = 0; } 
		}break;
		case GLFW_KEY_UP:
		{
			if (action == GLFW_PRESS)		{ key_up_is_down = 1; } 
			if (action == GLFW_RELEASE) { key_up_is_down = 0; } 
		}break;
		case GLFW_KEY_DOWN:
		{
			if (action == GLFW_PRESS)		{ key_down_is_down = 1; } 
			if (action == GLFW_RELEASE) { key_down_is_down = 0; } 
		}break;
	}
}

internal void
w2_init()
{
	Arena *arena_0 = arena_alloc();
	Arena *arena_1 = arena_alloc();
	W2_State *state = push_struct(arena_0, W2_State);

	state->arena = arena_0;
	state->paths_arena = arena_1;

	state->shader_dir_path = str8f(state->paths_arena, "%S/data/shaders", get_process_info()->binary_path);

	state->debug_camera = make_camera_perspective(FOV, ASPECT, NEAR_Z, FAR_Z);
	state->debug_camera_ortho = make_camera_orthographic(-2, 2, -2, 2, NEAR_Z, FAR_Z);
	state->debug_camera_pos = V3F32(0.0f, 2.0f, -5.0f);
	state->debug_camera_target = V3F32(0.0f, 0.0f, 0.0f);

	g_w2_state = state;
}



// stub function for now because the ordering of glfw and glew 
internal void
r_init(CmdLine *cmdline)
{
}

internal void
ogl_draw_pyramid(Vec3F32 position, Vec3F32 rotation_axis, Vec3F32 scale, F32 rotation_turns)
{

	Mat4x4F32 identity = MakeMat4x4F32(1.0f);

	Mat4x4F32 trs = {0};

	Mat4x4F32 t = MakeTranslate4x4F32(position);
	Mat4x4F32 r = MakeRotate4x4F32(rotation_axis, rotation_turns);
	Mat4x4F32 s = MakeScale4x4F32(scale);

	trs = Mul4x4F32(t, r);
	trs = Mul4x4F32(trs, s);

	Mat4x4F32 projection = get_debug_camera_projection();
	Mat4x4F32 view			 = get_debug_camera_view();

	OGL_Mesh3D *m = g_w2_state->pyramid_mesh;

	glUseProgram(g_w2_state->shaders[0]);
	glBindVertexArray(m->vao);
	glUniformMatrix4fv(glGetUniformLocation(g_w2_state->shaders[0], "u_transform"), 1, GL_FALSE, &trs.elements[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(g_w2_state->shaders[0], "u_projection"), 1, GL_FALSE, &projection.elements[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(g_w2_state->shaders[0], "u_view"), 1, GL_FALSE, &view.elements[0][0]);


	glBindBuffer(GL_ARRAY_BUFFER, m->buffers[0].handle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->element_buffer.handle);

	glDrawElements(GL_TRIANGLES, m->vtx_idx_pair.indices.count, GL_UNSIGNED_INT, (void*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0); 
	glBindVertexArray(0);
}

internal void
ogl_draw_cube(Vec3F32 position, Vec3F32 rotation_axis, Vec3F32 scale, F32 rotation_turns)
{
	Mat4x4F32 identity = MakeMat4x4F32(1.0f);

	Mat4x4F32 trs = {0};


	Mat4x4F32 t = MakeTranslate4x4F32(position);
	Mat4x4F32 r = MakeRotate4x4F32(rotation_axis, rotation_turns);
	Mat4x4F32 s = MakeScale4x4F32(scale);

	trs = Mul4x4F32(t, r);
	trs = Mul4x4F32(trs, s);

	Mat4x4F32 projection = get_debug_camera_projection();
	Mat4x4F32 view			 = get_debug_camera_view();


	OGL_Mesh3D *m = g_w2_state->cube_mesh;

	glBindVertexArray(m->vao);
	glUseProgram(g_w2_state->shaders[0]);


	glUniformMatrix4fv(glGetUniformLocation(g_w2_state->shaders[0], "u_transform"), 1, GL_FALSE, &trs.elements[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(g_w2_state->shaders[0], "u_projection"), 1, GL_FALSE, &projection.elements[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(g_w2_state->shaders[0], "u_view"), 1, GL_FALSE, &view.elements[0][0]);

	glBindBuffer(GL_ARRAY_BUFFER, m->buffers[0].handle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->element_buffer.handle);

	glDrawElements(GL_TRIANGLES, m->vtx_idx_pair.indices.count, GL_UNSIGNED_INT, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

internal void
ogl_draw_icosphere(Vec3F32 position, Vec3F32 rotation_axis, Vec3F32 scale, F32 rotation_turns)
{
	Mat4x4F32 identity = MakeMat4x4F32(1.0f);

	Mat4x4F32 trs = {0};

	Mat4x4F32 t = MakeTranslate4x4F32(position);
	Mat4x4F32 r = MakeRotate4x4F32(rotation_axis, rotation_turns);
	Mat4x4F32 s = MakeScale4x4F32(scale);

	trs = Mul4x4F32(t, r);
	trs = Mul4x4F32(trs, s);

	Mat4x4F32 projection = get_debug_camera_projection();
	Mat4x4F32 view			 = get_debug_camera_view();


	OGL_Mesh3D *m = g_w2_state->icosphere_mesh;

	glBindVertexArray(m->vao);
	glUseProgram(g_w2_state->shaders[0]);


	glUniformMatrix4fv(glGetUniformLocation(g_w2_state->shaders[0], "u_transform"), 1, GL_FALSE, &trs.elements[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(g_w2_state->shaders[0], "u_projection"), 1, GL_FALSE, &projection.elements[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(g_w2_state->shaders[0], "u_view"), 1, GL_FALSE, &view.elements[0][0]);

	glBindBuffer(GL_ARRAY_BUFFER, m->buffers[0].handle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->element_buffer.handle);

	glDrawElements(GL_TRIANGLES, m->vtx_idx_pair.indices.count, GL_UNSIGNED_INT, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

internal void
ogl_draw_uvsphere(Vec3F32 position, Vec3F32 rotation_axis, Vec3F32 scale, F32 rotation_turns)
{
	Mat4x4F32 identity = MakeMat4x4F32(1.0f);

	Mat4x4F32 trs = {0};

	Mat4x4F32 t = MakeTranslate4x4F32(position);
	Mat4x4F32 r = MakeRotate4x4F32(rotation_axis, rotation_turns);
	Mat4x4F32 s = MakeScale4x4F32(scale);

	trs = Mul4x4F32(t, r);
	trs = Mul4x4F32(trs, s);

	Mat4x4F32 projection = get_debug_camera_projection();
	Mat4x4F32 view			 = get_debug_camera_view();


	OGL_Mesh3D *m = g_w2_state->uvsphere_mesh;

	glBindVertexArray(m->vao);
	glUseProgram(g_w2_state->shaders[0]);


	glUniformMatrix4fv(glGetUniformLocation(g_w2_state->shaders[0], "u_transform"), 1, GL_FALSE, &trs.elements[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(g_w2_state->shaders[0], "u_projection"), 1, GL_FALSE, &projection.elements[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(g_w2_state->shaders[0], "u_view"), 1, GL_FALSE, &view.elements[0][0]);

	glBindBuffer(GL_ARRAY_BUFFER, m->buffers[0].handle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->element_buffer.handle);

	glDrawElements(GL_TRIANGLES, m->vtx_idx_pair.indices.count, GL_UNSIGNED_INT, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

internal Camera
make_camera_perspective(F32 fov, F32 aspect, F32 near_z, F32 far_z)
{
	Camera result = {0};
	result.projection = MakePerspective4x4F32(fov, aspect, near_z, far_z);
	return result;
}

internal Camera
make_camera_orthographic(F32 left, F32 right, F32 bottom, F32 top, F32 near_z, F32 far_z)
{
	Camera result = {0};
	result.projection = MakeOrthographic4x4F32(left, right, bottom, top, near_z, far_z);
	return result;
}

internal Mat4x4F32 get_debug_camera_view(void)
{
	Mat4x4F32 result = MakeLookAt4x4F32(g_w2_state->debug_camera_pos, g_w2_state->debug_camera_target, V3F32(0.0f, 1.0f, 0.0f));
	return result;
}


internal Mat4x4F32 get_debug_camera_projection(void)
{
	if (ortho_camera) { return g_w2_state->debug_camera_ortho.projection; } 
	return g_w2_state->debug_camera.projection;
}


internal void
ogl_toggle_wireframe(void)
{
	local_persist B32 wire_mode = 1;

	wire_mode = !wire_mode;

	if (wire_mode)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
}



internal OGL_Buffer ogl_buffer_alloc(GLenum target, U64 size, void *data, GLenum usage)
{
	OGL_Buffer buffer = {0};
	buffer.target = target;
	buffer.usage = usage;
	buffer.size = size;
	buffer.data = data;

	glGenBuffers(1, &buffer.handle);
	glBindBuffer(buffer.target, buffer.handle);
	glBufferData(buffer.target, buffer.size, buffer.data, buffer.usage);

	return buffer;
}

internal OGL_Mesh3D *ogl_mesh3d_alloc(Arena *arena, R_OGL_AttributeArray attribs)
{
	OGL_Mesh3D *mesh = push_struct(arena, OGL_Mesh3D);

	glGenVertexArrays(1, &mesh->vao);
	glBindVertexArray(mesh->vao);

	mesh->attribs = attribs;
	mesh->buffers = push_array(arena, OGL_Buffer, mesh->attribs.count);

	glBindVertexArray(0);
	return mesh;
}

internal VertexIndexArrayPair
ogl_make_uvsphere(Arena *arena, U64 rings, U64 slices)
{
	VertexIndexArrayPair result = {0};
	Temp scratch = scratch_begin(&arena, 1);

	F32 radius = 1.0f;

	U64 phi_count = Clamp(3, rings, 64);
	U64 theta_count = Clamp(2, slices, 64);
	F32 theta_step = PiF32 / theta_count;
	F32 phi_step = (PiF32*2) / phi_count;

	U64 vertex_count = 2 + phi_count * (theta_count-1);
	U64 index_count = 2*3*phi_count + 2*3*phi_count * (theta_count - 2);

	result.vertices.v = push_array_no_zero(arena, Vec3F32, vertex_count);
	result.vertices.count = vertex_count;

	result.indices.v = push_array_no_zero(arena, S32, index_count);
	result.indices.count = index_count;

	Vec3F32 *vtx = result.vertices.v;
	S32 *idx = result.indices.v;
	U64 off = 0;

	// top vertex
	vtx[off] = V3F32(0.0f, radius, 0.0f);
	off += 1; 

	for (U32 j = 1; j < theta_count; j += 1)
	{
		F32 theta = j * theta_step;
		for (U32 i = 0; i < phi_count; i += 1)
		{
			F32 phi = i * phi_step;

			F32 x = radius*SinF32(theta)*CosF32(phi);
			F32 y = radius*CosF32(theta);
			F32 z = -radius*SinF32(theta)*SinF32(phi);

			vtx[off] = V3F32(x, y, z);
			off += 1;
		}
	}
	vtx[off] = V3F32(0.0f, -radius, 0.0f);

	off = 0;
	for (U32 i = 0; i < phi_count - 1; i += 1)
	{
		idx[off+0] = 0;
		idx[off+1] = i + 1;
		idx[off+2] = i + 2;
		idx += 3;
	}

	idx[off+0] = 0;
	idx[off+1] = phi_count;
	idx[off+2] = 1;
	off += 3;

	for (U32 j = 0; j < (theta_count - 2); j += 1)
	{
		for (U32 i = 0; i < (phi_count - 1); i += 1)
		{
			U32 index[4] =
			{
				1 + (i + 0) + (j + 0) * phi_count,
				1 + (i + 0) + (j + 1) * phi_count,
				1 + (i + 1) + (j + 1) * phi_count,
				1 + (i + 1) + (j + 0) * phi_count
			};

			idx[off+0] = index[0];
			idx[off+1] = index[1];
			idx[off+2] = index[2];
			off += 3;

			idx[off+0] = index[0];
			idx[off+1] = index[2];
			idx[off+2] = index[3];
			off += 3;
		}
		U32 index[4] =
		{
			phi_count + j * phi_count,
			phi_count + (j + 1) * phi_count,
			1 + (j + 1) * phi_count,
			1 + j * phi_count
		};

			idx[off+0] = index[0];
			idx[off+1] = index[1];
			idx[off+2] = index[2];
			off += 3;

			idx[off+0] = index[0];
			idx[off+1] = index[2];
			idx[off+2] = index[3];
			off += 3;
	}

	U32 last_idx = vertex_count - 1;
	for (U32 i = 0; i  < (phi_count - 1); i += 1)
	{
		idx[off+0] = last_idx;
		idx[off+1] = last_idx - phi_count + i + 1;
		idx[off+2] = last_idx - phi_count + i;
		idx += 3;
	}

		idx[off+0] = last_idx;
		idx[off+1] = last_idx - phi_count;
		idx[off+2] = last_idx - 1;
		idx += 3;

	return result;
	scratch_end(scratch);
}

internal VertexIndexArrayPair
ogl_make_icosphere(Arena *arena, U64 num_subdivide_passes)
{
 VertexIndexArrayPair result = {0};
  local_persist read_only Vec3F32 icosahedron_vertices[] =
  {
#define face_size 1.618034f // (1 + sqrt(5)) / 2
   {-1.f, +face_size, +0.f},
   {+1.f, +face_size, +0.f},
   {-1.f, -face_size, +0.f},
   {+1.f, -face_size, +0.f},
   {+0.f, -1.f, +face_size},
   {+0.f, +1.f, +face_size},
   {+0.f, -1.f, -face_size},
   {+0.f, +1.f, -face_size},
   {+face_size, +0.f, -1.f},
   {+face_size, +0.f, +1.f},
   {-face_size, +0.f, -1.f},
   {-face_size, +0.f, +1.f},
#undef face_size
  };
  local_persist read_only S32 icosahedron_indices[] =
  {
   0, 11, 5, 0, 5, 1, 0, 1, 7,
   0, 7, 10, 0, 10, 11, 11, 10, 2,
   5, 11, 4, 1, 5, 9, 7, 1, 8,
   10, 7, 6, 3, 9, 4, 3, 4, 2,
   3, 2, 6, 3, 6, 8, 3, 8, 9,
   9, 8, 1, 4, 9, 5, 2, 4, 11,
   6, 2, 10, 8, 6, 7
  };
  
  U64 vertex_count = ArrayCount(icosahedron_vertices);
  U64 index_count  = ArrayCount(icosahedron_indices);
  
  result.vertices.v = push_array_no_zero(arena, Vec3F32, vertex_count);
  result.vertices.count = vertex_count;

  result.indices.v = push_array_no_zero(arena, S32, index_count);
  result.indices.count = index_count;

  Vec3F32 *vtx = result.vertices.v;
  S32 *idx = result.indices.v;
  
  MemoryCopy(vtx, icosahedron_vertices, sizeof(icosahedron_vertices));
  MemoryCopy(idx, icosahedron_indices, sizeof(icosahedron_indices));

	return result;
}

internal VertexIndexArrayPair
ogl_make_cube(Arena *arena)
{
	VertexIndexArrayPair result = {0};
	// Vertices
	local_persist read_only Vec3F32 cube_vertices[] =
	{
		{ -1.0f, -1.0f, -1.0f  },
		{  1.0f, -1.0f, -1.0f  },
		{  1.0f,  1.0f, -1.0f	 },
		{ -1.0f,  1.0f, -1.0f  },
		{  1.0f, -1.0f,  1.0f  },
		{ -1.0f, -1.0f,  1.0f  },
		{ -1.0f,  1.0f,  1.0f  },
		{  1.0f,  1.0f,  1.0f  },
	};	
		// Indices
	local_persist read_only S32 cube_indices[] =
	{
		 0, 1, 2,
		 2, 3, 0,
		 0, 3, 5,
		 5, 3, 6,
		 6, 7, 5,
		 5, 7, 4,
		 4, 7, 2,
		 2, 1, 4,
		 4, 1, 5,
		 5, 1, 0,
		 3, 2, 7,
		 7, 6, 3
	};


	
	U64 vertex_count = ArrayCount(cube_vertices);
	U64 index_count  = ArrayCount(cube_indices);

	result.vertices.v = push_array_no_zero(arena, Vec3F32, vertex_count);
	result.vertices.count = vertex_count;

	result.indices.v = push_array_no_zero(arena, S32, index_count);
	result.indices.count = index_count;

	Vec3F32 *vtx = result.vertices.v;
	S32 *idx = result.indices.v;

	MemoryCopy(vtx, cube_vertices, sizeof(cube_vertices));
	MemoryCopy(idx, cube_indices, sizeof(cube_indices));

	return result;
}


internal VertexIndexArrayPair
ogl_make_pyramid(Arena* arena)
{
	VertexIndexArrayPair result = {0};
	// Vertices
	local_persist read_only Vec3F32 pyramid_vertices[] =
	{
		{ -1.0f, -1.0f, -1.0f },
	 	{ +1.0f, -1.0f, -1.0f },
		{ +0.0f, +1.0f, +0.0f },
		{ +1.0f, -1.0f, +1.0f },
		{ -1.0f, -1.0f, +1.0f },
	};	
		// Indices
	local_persist read_only S32 pyramid_indices[] =
	{
		 2, 0, 1,
		 2, 1, 3,
		 2, 3, 4,
		 2, 4, 0,
		 4, 3, 0,
		 0, 3, 1,
	};


	
	U64 vertex_count = ArrayCount(pyramid_vertices);
	U64 index_count  = ArrayCount(pyramid_indices);

	result.vertices.v = push_array_no_zero(arena, Vec3F32, vertex_count);
	result.vertices.count = vertex_count;

	result.indices.v = push_array_no_zero(arena, S32, index_count);
	result.indices.count = index_count;

	Vec3F32 *vtx = result.vertices.v;
	S32 *idx = result.indices.v;

	MemoryCopy(vtx, pyramid_vertices, sizeof(pyramid_vertices));
	MemoryCopy(idx, pyramid_indices, sizeof(pyramid_indices));

	return result;
}

