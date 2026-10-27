#ifndef BASE_ARENA_H
#define BASE_ARENA_H


#define ARENA_HEADER_SIZE 128

typedef U64 ArenaFlags;

enum
{
	ARENA_FLAG_NO_CHAIN			= (1<<0),
	ARENA_FLAG_LARGE_PAGES	= (1<<1),
};

typedef struct ArenaParams ArenaParams;
struct ArenaParams
{
	ArenaFlags flags;
	U64 reserve_size;
	U64 commit_size;
	void *optional_backing_buffer;
	char *allocation_site_file;
	int allocation_site_line;
	char *name;
};

typedef struct Arena Arena;
struct Arena
{
	Arena *prev;
	Arena *current;
	ArenaFlags flags;
	U64 commit_size;
	U64 reserve_size;
	U64 base_pos;
	U64 pos;
	U64 cmt;
	U64 res;
	char *allocation_site_file;
	int allocation_site_line;
	char *name;
#if ARENA_FREE_LIST
	Arena *free_last;
#endif
};

StaticAssert(sizeof(Arena) <= ARENA_HEADER_SIZE, arena_header_size_check);

typedef struct Temp Temp;
struct Temp
{
	Arena *arena;
	U64 pos;
};


global U64 arena_default_reserve_size = MB(64);
global U64 arena_default_commit_size = KB(64);
global U64 arena_default_flags = 0;



// Arena Base Functions //


internal Arena *arena_alloc_(ArenaParams* params);

#define arena_alloc(...) arena_alloc_(&(ArenaParams){.reserve_size = arena_default_reserve_size, .commit_size = arena_default_commit_size, .flags = arena_default_flags, .allocation_site_file = __FILE__, .allocation_site_line = __LINE__, __VA_ARGS__})

internal void arena_release(Arena* arena);
internal void *arena_push(Arena *arena, U64 size_bytes, U64 align, B32 zero);
internal U64 arena_pos(Arena* arena);
internal void arena_pop_to(Arena* arena, U64 pos);
internal void arena_clear(Arena* arena);
internal void arena_pop(Arena* arena, U64 amount);
internal Temp temp_begin(Arena* arena);
internal void temp_end(Temp temp);



// Arena Helper Macros //

#define push_array_no_zero_aligned(a, T, c, align) (T*)arena_push((a), sizeof(T)*(c), (align), (0))
#define push_array_aligned(a, T, c, align) (T*)arena_push((a), sizeof(T)*(c), (align), (1))
#define push_array_no_zero(a, T, c) push_array_no_zero_aligned(a, T, c, Max(8, AlignOf(T)))
#define push_array(a, T, c) push_array_aligned(a, T, c, Max(8, AlignOf(T)))

#define push_struct(a, T) push_array(a, T, 1) 
#define push_struct_no_zero(a, T) push_array_no_zero(a, T, 1)


#endif // BASE_ARENA_H
