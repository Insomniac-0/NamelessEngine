internal VertexIndexArrayPair
geo_make_cube(Arena* arena)
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
geo_make_pyramid(Arena* arena)
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



internal VertexIndexArrayPair
geo_make_icosphere(Arena* arena, U64 num_subdivide_passes)
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
geo_make_uvsphere(Arena *arena, U64 rings, U64 slices)
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
