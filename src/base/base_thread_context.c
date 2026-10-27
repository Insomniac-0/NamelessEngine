thread_static ThreadContext *tctx_thread_local;


internal ThreadContext *
thread_context_alloc(void)
{
	Arena *arena_0 = arena_alloc();
	Arena *arena_1 = arena_alloc();

	ThreadContext *tctx = push_array(arena_0, ThreadContext, 1);
	tctx->arenas[0] = arena_0;
	tctx->arenas[1] = arena_1;
	tctx->lane_ctx.lane_count = 1;
	return tctx;
}

internal void
thread_context_release(ThreadContext* tctx)
{
	arena_release(tctx->arenas[1]);
	arena_release(tctx->arenas[0]);
}

internal void
thread_context_select(ThreadContext* tctx)
{
	tctx_thread_local = tctx;
}

internal ThreadContext*
thread_context_selected(void)
{
	return tctx_thread_local;
}


internal Arena*
thread_context_get_scratch(Arena **conflicts, U64 count)
{
	ThreadContext *tctx = thread_context_selected();
	Arena *result = 0;
	Arena **arena_ptr = tctx->arenas;
	for (U64 i = 0; i < ArrayCount(tctx->arenas); i += 1, arena_ptr += 1)
	{
		Arena **conflict_ptr = conflicts;
		B32 has_conflict = 0;
		for (U64 j = 0; j < count; j += 1, conflict_ptr += 1)
		{
			if (*arena_ptr == *conflict_ptr)
			{
				has_conflict = 1;
			 	break;
			}
		}
		if(!has_conflict)
		{
			result = *arena_ptr;
			break;
		}
	}
	return result;
}



// lane metadata

internal LaneContext
thread_context_set_lane_ctx(LaneContext lane_ctx)
{
	ThreadContext *tctx = thread_context_selected();
	LaneContext restore = tctx->lane_ctx;
	tctx->lane_ctx = lane_ctx;
	return restore;
}

internal void
thread_context_lane_barrier_wait(void *broadcast_ptr, U64 broadcast_size, U64 broadcast_src_lane_idx)
{
	ProfBeginFunction();
	ProfColor(0x0000ff);
	ThreadContext *tctx = thread_context_selected();

	// doing broadcast -> copy to broadcast memory on source lane
	U64 broadcast_size_clamped = ClampTop(broadcast_size, sizeof(tctx->lane_ctx.broadcast_memory[0]));
	if (broadcast_ptr != 0 && lane_idx() == broadcast_src_lane_idx)
	{
		MemoryCopy(tctx->lane_ctx.broadcast_memory, broadcast_ptr, broadcast_size_clamped);
	}

	barrier_wait(tctx->lane_ctx.barrier);


	if (broadcast_ptr != 0 && lane_idx() != broadcast_src_lane_idx)
	{
		MemoryCopy(broadcast_ptr, tctx->lane_ctx.broadcast_memory, broadcast_size_clamped);
	}

	if (broadcast_ptr != 0)
	{
		barrier_wait(tctx->lane_ctx.barrier);
	}

	ProfEnd();
}

