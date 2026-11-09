#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

typedef U32 OBJ_TokenFlags;
enum
{
	OBJ_TokenFlag_Identifier		= (1 << 0),
	OBJ_TokenFlag_Numeric				= (1 << 1),
	OBJ_TokenFlag_Comment				= (1 << 2),
	OBJ_TokenFlag_Whitespace		= (1 << 3),
	OBJ_TokenFlag_Newline				= (1 << 4),
	OBJ_TokenFlag_BadCharacter	= (1 << 5),
};

typedef struct OBJ_Token OBJ_Token;
struct OBJ_Token
{
	Range1U64 range;
	OBJ_TokenFlags flags;
};

typedef struct OBJ_TokenChunkNode OBJ_TokenChunkNode;
struct OBJ_TokenChunkNode
{
	OBJ_TokenChunkNode *next;
	OBJ_Token *v;
	U64 count;
	U64 cap;
};

typedef struct OBJ_TokenChunkList OBJ_TokenChunkList;
struct OBJ_TokenChunkList
{
	OBJ_TokenChunkNode *first;
	OBJ_TokenChunkNode *last;
	U64 chunk_count;
	U64 total_token_count;
};

typedef struct OBJ_TokenArray OBJ_TokenArray;
struct OBJ_TokenArray
{
	OBJ_Token *v;
	U64 count;
};

typedef struct OBJ_TokenizeResult OBJ_TokenizeResult;
struct OBJ_TokenizeResult
{
	OBJ_TokenArray tokens;
};


internal void obj_token_chunk_list_push(Arena *arena, OBJ_TokenChunkList *list, U64 cap, OBJ_Token token);
internal OBJ_TokenArray obj_token_array_from_chunk_list(Arena *arena, OBJ_TokenChunkList *chunks);
internal OBJ_TokenizeResult obj_tokenize_from_string(Arena *arena, String8 string);


#endif //OBJ_LOADER_H
