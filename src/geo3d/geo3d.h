#ifndef GEO_3D_H
#define GEO_3D_H


typedef struct VertexIndexArrayPair VertexIndexArrayPair;
struct VertexIndexArrayPair
{
	Vec3F32Array vertices;
	S32Array indices;
};



// Builtin Geometry
internal VertexIndexArrayPair geo_make_cube(Arena* arena);
internal VertexIndexArrayPair geo_make_pyramid(Arena* arena);
internal VertexIndexArrayPair geo_make_icosphere(Arena* arena, U64 num_subdivide_passes);
internal VertexIndexArrayPair geo_make_uvsphere(Arena *arena, U64 rings, U64 slices);


#endif // GEO_3D_H
