internal Arena *
arena_alloc_(ArenaParams* params)
{
	// TODO(Seppo): Call to generic PROFILE_BEGIN_FUNC(); [ ]
	U64 reserve_size = params->reserve_size;
	U64 commit_size = params->commit_size;

	void* base = params->optional_backing_buffer;
	if (base == 0)
	{
		if (params->flags & ARENA_FLAG_LARGE_PAGES)
		{
			reserve_size = AlignPow2(reserve_size, get_system_info()->large_page_size);
			commit_size = AlignPow2(commit_size, get_system_info()->large_page_size);
		}
		else
		{
			reserve_size = AlignPow2(reserve_size, get_system_info()->page_size);
			commit_size = AlignPow2(commit_size, get_system_info()->page_size);
		}

		if (params->flags & ARENA_FLAG_LARGE_PAGES)
		{
			base = reserve_memory_large(reserve_size);
			commit_memory_large(base, commit_size);
		}
		else
		{
			base = reserve_memory(reserve_size);
			commit_memory(base, commit_size);
		}

		AsanPoisonMemoryRegion(base, commit_size);
	}
	else
	{
		AsanPoisonMemoryRegion(base, params->reserve_size);
	}

	// TODO(Seppo): crash and log arena creation failure [ ]
	//if (Unlikely(base == 0))
	//{
	//abort
	//}

	AsanUnpoisonMemoryRegion(base, ARENA_HEADER_SIZE);

	Arena* arena								= base;
	arena->current							= arena;
	arena->flags								= params->flags;
	arena->commit_size					= params->commit_size;
	arena->reserve_size					= params->reserve_size;
	arena->base_pos							= 0;
	arena->pos									= ARENA_HEADER_SIZE;
	arena->cmt									= commit_size;
	arena->res									= reserve_size;
	arena->allocation_site_file = params->allocation_site_file;
	arena->allocation_site_line = params->allocation_site_line;
	arena->name									= params->name;
#if ARENA_FREE_LIST
	arena->free_last = 0;
#endif

	// TODO(Seppo): Call to generic PROFILE_END(); [ ]

	return arena;

}


internal void
arena_release(Arena* arena)
{
	// PROFILE BEGIN

	release_memory(arena, arena->res);
	// PROFILE END
}


internal void *
arena_push(Arena* arena, U64 size_bytes, U64 align, B32 zero)
{
	Arena *current = arena->current;
	U64 pos_pre = AlignPow2(current->pos, align);
	U64 pos_pst = pos_pre + size_bytes;

	U64 size_to_zero = 0;
	if (zero)
	{
		size_to_zero = Min(current->cmt, pos_pst) - pos_pre;
	}

	// commit new pages if needed
	if (arena->cmt < pos_pst)
	{
		U64 commit_pst_aligned = pos_pst + current->commit_size-1;

		commit_pst_aligned -= commit_pst_aligned % arena->commit_size;

		U64 commit_pst_clamped = ClampTop(commit_pst_aligned, arena->res);
		U64 commit_size = commit_pst_clamped - arena->cmt;
		U8* commit_ptr = (U8*)arena + arena->cmt;
		if (arena->flags & ARENA_FLAG_LARGE_PAGES)
		{
			commit_memory_large(commit_ptr, commit_size);
		}
		else
		{
			commit_memory(commit_ptr, commit_size);
		}
		AsanPoisonMemoryRegion(commit_ptr, commit_size);
		arena->cmt = commit_pst_clamped;
	}

	void* result = 0;
	if (arena->cmt >= pos_pst)
	{
		result = (U8*)arena+pos_pre;
		arena->pos = pos_pst;
		AsanUnpoisonMemoryRegion(result, size_bytes);
		MemoryZero(result, size_to_zero);
	}
	// TODO(Seppo): Panic on failure
	//if (Unlikely(result == 0))
	//{
	//#if definded(SHELL_H)
	//sh_msg(x);
	//#endif
	//abort_self(1)
			

	return result;
}



internal U64
arena_pos(Arena* arena)
{
	U64 pos = arena->base_pos + arena->pos;
	return pos;
}

internal void
arena_pop_to(Arena* arena, U64 pos)
{
	U64 big_pos = ClampBot(ARENA_HEADER_SIZE, pos);

	U64 new_pos = big_pos - arena->base_pos;
	AssertAlways(new_pos <= arena->pos);
	AsanPoisonMemoryRegion((U8*)arena + new_pos, (arena->pos - new_pos));
	arena->pos = new_pos;
}


internal void
arena_clear(Arena* arena)
{
	arena_pop_to(arena, 0);
}

internal void
arena_pop(Arena* arena, U64 amount)
{
	U64 pos_old = arena_pos(arena);
	U64 pos_new = pos_old;
	if (amount < pos_old)
	{
		pos_new = pos_old - amount;
	}
	arena_pop_to(arena, pos_new);
}


internal Temp
temp_begin(Arena* arena)
{
	U64 pos = arena_pos(arena);
	Temp temp = {arena, pos};
	return temp;
}


internal void
temp_end(Temp temp)
{
	arena_pop_to(temp.arena, temp.pos);
}
