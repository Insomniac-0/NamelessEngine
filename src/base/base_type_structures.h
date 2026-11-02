#ifndef BASE_TYPE_STRUCTURES_H
#define BASE_TYPE_STRUCTURES_H


typedef struct F32Array F32Array;
struct F32Array
{
	F32 *v;
	U64 count;
};

//~ Vector Arrays
typedef struct Vec3F32Array Vec3F32Array;
struct Vec3F32Array
{
	Vec3F32 *v;
	U64 count;
};

typedef struct Vec4F32Array Vec4F32Array;
struct Vec4F32Array
{
	Vec4F32 *v;
	U64 count;
};




#endif // BASE_TYPE_STRUCTURES_H
