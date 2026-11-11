#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

typedef U32 OBJ_TokenFlags;
enum
{
	OBJ_TokenFlag_Identifier		= (1 << 0),
	OBJ_TokenFlag_Numeric				= (1 << 1),
	OBJ_TokenFlag_Comment				= (1 << 2),
	OBJ_TokenFlag_Separator			= (1 << 3),
	OBJ_TokenFlag_Whitespace		= (1 << 4),
	OBJ_TokenFlag_Newline				= (1 << 5),
	OBJ_TokenFlag_BadCharacter	= (1 << 6),
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

typedef U32 OBJ_IdentifierFlags;
enum
{
	OBJ_IdentifierFlag_Null						= (0 << 0),
	OBJ_IdentifierFlag_ObjectName			= (1 << 0),
	OBJ_IdentifierFlag_VertexPosition = (1 << 1),
	OBJ_IdentifierFlag_VertexNormal		= (1 << 2),
	OBJ_IdentifierFlag_VertexUV				= (1 << 3),
	OBJ_IdentifierFlag_PolygonFace		= (1 << 4),
	OBJ_IdentifierFlag_PointElement		= (1 << 5),
	OBJ_IdentifierFlag_LineElement		= (1 << 6)
};

typedef enum OBJ_PolygonFaceKind
{
	OBJ_PolygonFaceKind_Triangle,
	OBJ_PolygonFaceKind_Quad,
	OBJ_PolygonFaceKind_Ngon,
	OBJ_PolygonFaceKind_NotSupported,
	OBJ_PolygonFaceKind_COUNT
} OBJ_PolygonFaceKind;

typedef union OBJ_PolygonIndex OBJ_PolygonIndex;
union OBJ_PolygonIndex
{
	struct
	{
		U32 p;
		U32 t;
		U32 n;
	};
	U32 v[3];
};

typedef struct OBJ_PolygonFaceNode OBJ_PolygonFaceNode;
struct OBJ_PolygonFaceNode
{
	OBJ_PolygonFaceNode *next;
	OBJ_PolygonIndex *v;
	B32 need_triangulate;
	U64 count;
};

typedef struct OBJ_PolygonFaceList OBJ_PolygonFaceList;
struct OBJ_PolygonFaceList
{
	OBJ_PolygonFaceNode *first;
	OBJ_PolygonFaceNode *last;
	U64 face_count;
	U64 total_index_count;
	U64 num_faces_to_triangulate;
};

typedef struct OBJ_ParseResult OBJ_ParseResult;
struct OBJ_ParseResult
{
	String8 object_name;
	Vec3F32Array positions;
	Vec3F32Array normals;
	Vec2F32Array uvs;

	OBJ_PolygonIndex *indices;
	U64 index_count;
};

internal void obj_token_chunk_list_push(Arena *arena, OBJ_TokenChunkList *list, U64 cap, OBJ_Token token);
internal OBJ_TokenArray obj_token_array_from_chunk_list(Arena *arena, OBJ_TokenChunkList *chunks);

// Text -> Tokens
internal OBJ_TokenizeResult obj_tokenize_from_string(Arena *arena, String8 string);

// Tokens -> ParseResult
internal OBJ_ParseResult obj_parse_from_text_tokens(Arena *arena, String8 text, OBJ_TokenArray tokens);


// PolygonFace
internal void obj_polygon_face_list_push(Arena *arena, OBJ_PolygonFaceList *list, OBJ_PolygonIndex *indices, U64 count);


// ParseResult -> GPUBuffer



#endif //OBJ_LOADER_H
