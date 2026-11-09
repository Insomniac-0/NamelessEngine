
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
