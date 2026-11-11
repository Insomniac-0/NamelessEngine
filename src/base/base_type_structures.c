internal void
v3f32_chunk_list_push(Arena *arena, Vec3F32ChunkList *list, U64 cap, Vec3F32 v)
{
	Vec3F32ChunkNode *node = list->last;
	if (node == 0 || node->count >= node->cap)
	{
		node = push_struct(arena, Vec3F32ChunkNode);
		node->cap = cap;
		node->v = push_array_no_zero(arena, Vec3F32, cap);
		SLLQueuePush(list->first, list->last, node);
	}
	MemoryCopyStruct(&node->v[node->count], &v);
	node->count += 1;
	list->total_v3f32_count += 1;
}


internal void
v2f32_chunk_list_push(Arena *arena, Vec2F32ChunkList *list, U64 cap, Vec2F32 v)
{
	Vec2F32ChunkNode *node = list->last;
	if (node == 0 || node->count >= node->cap)
	{
		node = push_struct(arena, Vec2F32ChunkNode);
		node->cap = cap;
		node->v = push_array_no_zero(arena, Vec2F32, cap);
		SLLQueuePush(list->first, list->last, node);
	}
	MemoryCopyStruct(&node->v[node->count], &v);
	node->count += 1;
	list->total_v2f32_count += 1;
}



internal Vec3F32Array
v3f32_array_from_chunk_list(Arena *arena, Vec3F32ChunkList *chunks)
{
	Vec3F32Array result = {0};
	result.count = chunks->total_v3f32_count;
	result.v = push_array_no_zero(arena, Vec3F32, result.count);
	U64 write_idx = 0;

	for (Vec3F32ChunkNode *n = chunks->first; n != 0; n = n->next)
	{
		MemoryCopy(result.v+write_idx, n->v, sizeof(Vec3F32)*n->count);
		write_idx += n->count;
	}
	return result;
}



internal Vec2F32Array
v2f32_array_from_chunk_list(Arena *arena, Vec2F32ChunkList *chunks)
{
	Vec2F32Array result = {0};
	result.count = chunks->total_v2f32_count;
	result.v = push_array_no_zero(arena, Vec2F32, result.count);
	U64 write_idx = 0;
	for (Vec2F32ChunkNode *n = chunks->first; n != 0; n = n->next)
	{
		MemoryCopy(result.v+write_idx, n->v, sizeof(Vec3F32)*n->count);
		write_idx += n->count;
	}
	return result;
}
