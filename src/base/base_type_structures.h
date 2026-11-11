#ifndef BASE_TYPE_STRUCTURES_H
#define BASE_TYPE_STRUCTURES_H


typedef struct F32Array F32Array;
struct F32Array
{
	F32 *v;
	U64 count;
};

//~ Vector Arrays



typedef struct Vec4F32Array Vec4F32Array;
struct Vec4F32Array
{
	Vec4F32 *v;
	U64 count;
};




// Vector3 Types 
typedef struct Vec3F32ChunkNode Vec3F32ChunkNode;
struct Vec3F32ChunkNode
{
	Vec3F32ChunkNode *next;
	Vec3F32 *v;
	U64 count;
	U64 cap;
};

typedef struct Vec3F32ChunkList Vec3F32ChunkList;
struct Vec3F32ChunkList
{
	Vec3F32ChunkNode *first;
	Vec3F32ChunkNode *last;
	U64 chunk_count;
	U64 total_v3f32_count;
};

typedef struct Vec3F32Array Vec3F32Array;
struct Vec3F32Array
{
	Vec3F32 *v;
	U64 count;
};


// Vector2 Types
typedef struct Vec2F32ChunkNode Vec2F32ChunkNode;
struct Vec2F32ChunkNode
{
	Vec2F32ChunkNode *next;
	Vec2F32 *v;
	U64 count;
	U64 cap;
};

typedef struct Vec2F32ChunkList Vec2F32ChunkList;
struct Vec2F32ChunkList
{
	Vec2F32ChunkNode *first;
	Vec2F32ChunkNode *last;
	U64 chunk_count;
	U64 total_v2f32_count;
};

typedef struct Vec2F32Array Vec2F32Array;
struct Vec2F32Array
{
	Vec2F32 *v;
	U64 count;
};


// List Helpers

internal void v3f32_chunk_list_push(Arena *arena, Vec3F32ChunkList *list, U64 cap, Vec3F32 v);
internal void v2f32_chunk_list_push(Arena *arena, Vec2F32ChunkList *list, U64 cap, Vec2F32 v);


// List -> Array

internal Vec3F32Array v3f32_array_from_chunk_list(Arena *arena, Vec3F32ChunkList *chunks);
internal Vec2F32Array v2f32_array_from_chunk_list(Arena *arena, Vec2F32ChunkList *chunks);




#endif // BASE_TYPE_STRUCTURES_H
