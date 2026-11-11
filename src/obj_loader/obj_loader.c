internal void
obj_token_chunk_list_push(Arena *arena, OBJ_TokenChunkList *list, U64 cap, OBJ_Token token)
{
	OBJ_TokenChunkNode *node = list->last;
	if (node == 0 || node->count >= node->cap)
	{
		node = push_struct(arena, OBJ_TokenChunkNode);
		node->cap = cap;
		node->v = push_array_no_zero(arena, OBJ_Token, cap);
		SLLQueuePush(list->first, list->last, node);
		list->chunk_count += 1;
	}
	MemoryCopyStruct(&node->v[node->count], &token);
	node->count += 1;
	list->total_token_count += 1;
}

internal OBJ_TokenArray
obj_token_array_from_chunk_list(Arena *arena, OBJ_TokenChunkList *chunks)
{
	OBJ_TokenArray result = {0};
	result.count = chunks->total_token_count;
	result.v = push_array_no_zero(arena, OBJ_Token, result.count);
	U64 write_idx = 0;

	for (OBJ_TokenChunkNode *n = chunks->first; n != 0; n = n->next)
	{
		MemoryCopy(result.v+write_idx, n->v, sizeof(OBJ_Token)*n->count);
		write_idx += n->count;
	}
	return result;
}

internal OBJ_TokenizeResult
obj_tokenize_from_string(Arena *arena, String8 string)
{
	Temp scratch = scratch_begin(&arena, 1);
	OBJ_TokenChunkList tokens = {0};

	U8 *byte_first = string.str;
	U8 *byte_opl = byte_first + string.size;
	U8 *byte = byte_first;


	// scan string and produce tokens
	for (;byte < byte_opl;)
	{
		OBJ_TokenFlags token_flags = {0};
		U8 *token_start = 0;
		U8 *token_opl = 0;


		// whitespace
		if (token_flags == 0 && (*byte == ' ' || *byte == '\t' || *byte == '\v' || *byte == '\r'))
		{
			token_flags = OBJ_TokenFlag_Whitespace;
			token_start = byte;
			token_opl = byte;
			byte += 1;
			for (;byte <= byte_opl; byte += 1)
			{
				token_opl += 1;
				if (byte == byte_opl || (*byte != ' ' || *byte != '\t' || *byte != '\v' || *byte != '\r'))
				{
					break;
				}
			}
		}

		// newline
		if (token_flags == 0 && *byte == '\n')
		{
			token_flags = OBJ_TokenFlag_Newline;
			token_start = byte;
			token_opl = byte+1;
			byte += 1;
		}

		// comment
		if (token_flags == 0 && (byte+1 < byte_opl && *byte == '#'))
		{
			token_flags = OBJ_TokenFlag_Comment;
			token_start = byte;
			token_opl = byte+1;
			byte += 1;
			for (;byte <= byte_opl; byte += 1)
			{
				token_opl += 1;
				if (byte == byte_opl)
				{
					break;
				}
				if (*byte == '\n')
				{
					break;
				}
			}
		}

		// identifiers
		if (token_flags == 0 && (('A' <= *byte && *byte <= 'Z') ||
														 ('a' <= *byte && *byte <= 'z') ||
														 *byte == '_'))
		{
			token_flags = OBJ_TokenFlag_Identifier;
			token_start = byte;
			token_opl = byte;
			byte += 1;
			for (;byte <= byte_opl; byte += 1)
			{
				token_opl += 1;
				if (byte == byte_opl ||
						(!('A' <= *byte && *byte <= 'Z') &&
						 !('a' <= *byte && *byte <= 'z') &&
						 !('0' <= *byte && *byte <= '9') &&
						 *byte != '_'))

				{
					break;
				}
			}
		}

		// numerics
		if (token_flags == 0 && (('0' <= *byte && *byte <= '9') ||
														 (*byte == '.' && byte+1 < byte_opl && '0' <= byte[1] && byte[1] <= '9') ||
														 (*byte == '-' && byte+1 < byte_opl && '0' <= byte[1] && byte[1] <= '9')))
		{
			token_flags = OBJ_TokenFlag_Numeric;
			token_start = byte;
			token_opl = byte;
			byte += 1;
			for (;byte <= byte_opl; byte += 1)
			{
				token_opl += 1;
				if (byte == byte_opl ||
						(!('0' <= *byte && *byte <= '9') &&
						 *byte != '_' &&
						 *byte != '.'))

				{
					break;
				}
			}
		}

		// reserved symbols
		if (token_flags == 0 && (*byte == '/'))
		{
			token_flags = OBJ_TokenFlag_Separator;
			token_start = byte;
			token_opl = byte+1;
			byte += 1;
		}

		if (token_flags == 0)
		{
			token_flags = OBJ_TokenFlag_BadCharacter;
			token_start = byte;
			token_opl = byte+1;
			byte += 1;
		}

		if (token_flags != 0 && token_start != 0 && token_opl > token_start)
		{
			OBJ_Token token = { {(U64)(token_start - byte_first), (U64)(token_opl - byte_first)}, token_flags };
			obj_token_chunk_list_push(scratch.arena, &tokens, 4096, token);
		}
	}

	OBJ_TokenizeResult result = {0};
	result.tokens = obj_token_array_from_chunk_list(arena, &tokens);

	scratch_end(scratch);
	return result;
}


internal OBJ_ParseResult
obj_parse_from_text_tokens(Arena *arena, String8 text, OBJ_TokenArray tokens)
{
	Temp scratch = scratch_begin(&arena, 1);
	Vec3F32ChunkList positions = {0};
	Vec3F32ChunkList normals = {0};
	Vec2F32ChunkList uvs = {0};

	OBJ_PolygonFaceList faces = {0};

	OBJ_Token *tokens_first = tokens.v;
	OBJ_Token *tokens_opl = tokens_first + tokens.count;
	OBJ_Token *token = tokens_first;
	OBJ_IdentifierFlags identifier_flags = 0;

	String8 obj_name;
	B32 has_name = 0;

	for (;token < tokens_opl;)
	{
		identifier_flags = 0;
		String8 token_string = str8_substr(text, token[0].range);
		if (token->flags & OBJ_TokenFlag_Whitespace ||
				token->flags & OBJ_TokenFlag_Comment ||
				token->flags & OBJ_TokenFlag_Newline ||
				token->flags & OBJ_TokenFlag_BadCharacter)
		{
			token += 1;
			goto end_consume;
		}

		if (identifier_flags == 0 && token->flags & OBJ_TokenFlag_Identifier)
		{
			if (str8_match(token_string, str8_lit("o"), 0))
			{
				identifier_flags = OBJ_IdentifierFlag_ObjectName;
			}
			else if (str8_match(token_string, str8_lit("v"), 0))
			{
				identifier_flags = OBJ_IdentifierFlag_VertexPosition;
			}
			else if(str8_match(token_string, str8_lit("vn"), 0))
			{
				identifier_flags = OBJ_IdentifierFlag_VertexNormal;
			}
			else if(str8_match(token_string, str8_lit("vt"), 0))
			{
				identifier_flags = OBJ_IdentifierFlag_VertexUV;
			}
			else if(str8_match(token_string, str8_lit("f"), 0))
			{
				identifier_flags = OBJ_IdentifierFlag_PolygonFace;
			}
			else
			{
				identifier_flags = OBJ_IdentifierFlag_Null;
			}
			token += 1;
		}

		if (!has_name && identifier_flags & OBJ_IdentifierFlag_ObjectName)
		{
			for (;token <= tokens_opl; token += 1)
			{
				if (token == tokens_opl)
				{
					break;
				}
				if (token->flags & OBJ_TokenFlag_Newline)
				{
					break;
				}
				if (token->flags & OBJ_TokenFlag_Identifier)
				{
					obj_name = str8_copy(arena, token_string);
					has_name = 1;
					token += 1;
					break;
				}
			}
		}
		if (identifier_flags & OBJ_IdentifierFlag_VertexPosition)
		{
			Vec3F32 vertex = {0};
			U64 write_idx = 0;

			for (;token <= tokens_opl && write_idx < 3; token += 1)
			{
				if (token == tokens_opl)
				{
					break;
				}
				if (token->flags & OBJ_TokenFlag_Newline)
				{
					break;
				}
				if (token->flags & OBJ_TokenFlag_Numeric)
				{
					String8 num_raw_string = str8_substr(text, token[0].range);
					F32 f32 = (F32)f64_from_str8(num_raw_string);
					vertex.v[write_idx] = f32;
					write_idx += 1;
				}
			}
			v3f32_chunk_list_push(scratch.arena, &positions, 4096, vertex);
		}
		if (identifier_flags & OBJ_IdentifierFlag_VertexNormal)
		{
			Vec3F32 vertex = {0};
			U64 write_idx = 0;
			for (;token <= tokens_opl && write_idx < 3; token += 1)
			{
				if (token == tokens_opl)
				{
					break;
				}
				if (token->flags & OBJ_TokenFlag_Newline)
				{
					break;
				}
				if (token->flags & OBJ_TokenFlag_Numeric)
				{
					String8 num_raw_string = str8_substr(text, token[0].range);
					F32 f32 = (F32)f64_from_str8(num_raw_string);
					vertex.v[write_idx] = f32;
					write_idx += 1;
				}
			}
			v3f32_chunk_list_push(scratch.arena, &normals, 4096, vertex);
		}
		if (identifier_flags & OBJ_IdentifierFlag_VertexUV)
		{
			Vec2F32 vertex = {0};
			U64 write_idx = 0;
			for (;token <= tokens_opl && write_idx < 2; token += 1)
			{
				if (token == tokens_opl)
				{
					break;
				}
				if (token->flags & OBJ_TokenFlag_Newline)
				{
					break;
				}
				if (token->flags & OBJ_TokenFlag_Numeric)
				{
					String8 num_raw_string = str8_substr(text, token[0].range);
					F32 f32 = (F32)f64_from_str8(num_raw_string);
					vertex.v[write_idx] = f32;
					write_idx += 1;
				}
			}
			v2f32_chunk_list_push(scratch.arena, &uvs, 4096, vertex);
		}
		if (identifier_flags & OBJ_IdentifierFlag_PolygonFace)
		{

			OBJ_PolygonIndex polygon_indices[6] = {0};
			U32 vert[3];

			U64 vert_idx = 0;
			U64 write_idx = 0;

			B32 past_first_numeric = 0;

			for (;token <= tokens_opl && write_idx < 6; token += 1)
			{
				if (token == tokens_opl)
				{
					break;
				}
				if (token->flags & OBJ_TokenFlag_Newline)
				{
					if (past_first_numeric)
					{
						polygon_indices[write_idx].p = vert[0];
						polygon_indices[write_idx].t = vert[1];
						polygon_indices[write_idx].n = vert[2];
						write_idx += 1;
					}
					break;
				}
				if (token->flags & OBJ_TokenFlag_Whitespace)
				{
					if (past_first_numeric)
					{
						polygon_indices[write_idx].p = vert[0];
						polygon_indices[write_idx].t = vert[1];
						polygon_indices[write_idx].n = vert[2];
						write_idx += 1;
					}
					vert_idx = 0;
				}
				if (token->flags & OBJ_TokenFlag_Separator)
				{
					vert_idx += 1;
				}
				if (token->flags & OBJ_TokenFlag_Numeric)
				{
					if (!past_first_numeric)
					{
						past_first_numeric = 1;
						// Not needed but doing it for clarity
						write_idx = 0;
						vert_idx = 0;
					}
					String8 num_raw_string = str8_substr(text, token[0].range);
					U32 u32 = u32_from_str8(num_raw_string, 10);
					vert[vert_idx] = u32;
				}
			}

			// just for clarity
			U64 index_count = write_idx;
			obj_polygon_face_list_push(scratch.arena, &faces, polygon_indices, index_count);
		}

	end_consume:;
	}

	fprintf(stderr, ".obj parse procedure done.\n");

	OBJ_ParseResult result = {0};
	result.positions = v3f32_array_from_chunk_list(arena, &positions);
	result.normals = v3f32_array_from_chunk_list(arena, &normals);
	result.uvs = v2f32_array_from_chunk_list(arena, &uvs);

	scratch_end(scratch);

	return result;
}

internal void
obj_polygon_face_list_push(Arena *arena, OBJ_PolygonFaceList *list, OBJ_PolygonIndex *indices, U64 count)
{
	B32 need_triangulate = count > 3 ? 1 : 0;

	OBJ_PolygonFaceNode *node = push_struct(arena, OBJ_PolygonFaceNode);
	node->count = count;
	node->v = push_array_no_zero(arena, OBJ_PolygonIndex, node->count);
	node->need_triangulate = need_triangulate;
	SLLQueuePush(list->first, list->last, node);
	MemoryCopy(&node->v[0], indices, sizeof(OBJ_PolygonIndex)*node->count);
	list->total_index_count += count;
	if (need_triangulate)
	{
		list->num_faces_to_triangulate += 1;
	}
}
