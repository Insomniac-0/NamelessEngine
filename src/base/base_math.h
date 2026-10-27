#ifndef BASE_MATH_H
#define BASE_MATH_H


//~ Vector Types


typedef union Vec2F32 Vec2F32;
union Vec2F32
{
	struct
	{
		F32 x;
		F32 y;
	};
	F32 v[2];

	/* #if LANG_CPP */
	/* 	inline F32 &operator[](S32 index) { return v[index]; } */
	/* 	inline const F32 &operator[](S32 index) const { return v[index]; } */

	/* 	inline Vec2F32 &operator*=(F32 a); */
	/* 	inline Vec2F32 &operator+=(Vec2F32 a); */
	/* #endif */

};

typedef union Vec2S64 Vec2S64;
union Vec2S64
{
	struct
	{
		S64 x;
		S64 y;
	};
	S64 v[2];
};


typedef union Vec2S32 Vec2S32;
union Vec2S32
{
	struct
	{
		S32 x;
		S32 y;
	};
	S32 v[2];
};

typedef union Vec2S16 Vec2S16;
union Vec2S16
{
	struct
	{
		S16 x;
		S16 y;
	};
	S16 v[2];
};


// 3-Vectors

typedef union Vec3F32 Vec3F32;
union Vec3F32
{
	struct
	{
		F32 x;
		F32 y;
		F32 z;
	};
	struct
	{
		F32 r;
		F32 g;
		F32 b;
	};
	struct
	{
		Vec2F32 xy;
		F32 _z0;
	};
	struct
	{
		F32 _x0;
		Vec2F32 yz;

	};
	F32 v[3];

	/* #if LANG_CPP */
	/* 	inline F32 &operator[](S32 index) { return v[index]; } */
	/* 	inline const F32 &operator[](S32 index) const { return v[index]; } */
	/* #endif */
};

typedef union Vec3S32 Vec3S32;
union Vec3S32
{
	struct
	{
		S32 x;
		S32 y;
		S32 z;
	};
	struct
	{
		Vec2S32 xy;
		S32 _z0;
	};
	S32 v[3];
};



// 4-Vectors

typedef union Vec4F32 Vec4F32;
union Vec4F32
{
	struct
	{
		F32 x;
		F32 y;
		F32 z;
		F32 w;
	};
	struct
	{
		Vec2F32 xy;
		Vec2F32 zw;
	};
	struct
	{
		Vec3F32 xyz;
		F32 _z0;
	};
	struct
	{
		F32 _x0;
		Vec3F32 yzw;
	};

	F32 v[4];
};

typedef union Vec4S32 Vec4S32;
union Vec4S32
{
	struct
	{
		S32 x;
		S32 y;
		S32 z;
		S32 w;
	};
	struct
	{
		Vec2S32 xy;
		Vec2S32 zw;
	};
	struct
	{
		Vec3S32 xyz;
		S32 _w0;
	};
	struct
	{
		S32 _x0;
		Vec3S32 yzw;
	};
	S32 v[4];
};



//~ Matrix Types

typedef struct Mat3x3F32 Mat3x3F32;
struct Mat3x3F32
{
	F32 v[3][3];
};

typedef struct Mat4x4F32 Mat4x4F32;
struct Mat4x4F32
{
	F32 v[4][4];
};


//~ Matrix Constructors
//- 3x3
internal Mat3x3F32 MakeMat3x3F32(F32 d);

//- 4x4
internal Mat4x4F32 MakeMat4x4F32(F32 d);


//~ Range Types

// Range1 Types
typedef union Range1U8 Range1U8;
union Range1U8
{
	struct
	{
		U8 min;
		U8 max;
	};
	U8 v[2];
};


typedef union Range1U16 Range1U16;
union Range1U16
{
	struct
	{
		U16 min;
		U16 max;
	};
	U16 v[2];
};


typedef union Range1U32 Range1U32;
union Range1U32
{
	struct
	{
		U32 min;
		U32 max;
	};
	U32 v[2];
};


typedef union Range1U64 Range1U64;
union Range1U64
{
	struct
	{
		U64 min;
		U64 max;
	};
	U64 v[2];
};

typedef union Range1S32 Range1S32;
union Range1S32
{
	struct
	{
		S32 min;
		S32 max;
	};
	S32 v[2];
};


typedef union Range1S64 Range1S64;
union Range1S64
{
	struct
	{
		S64 min;
		S64 max;
	};
	S64 v[2];
};



typedef union Range1F32 Range1F32;
union Range1F32
{
	struct
	{
		F32 min;
		F32 max;
	};
	F32 v[2];
};


typedef union Range1F64 Range1F64;
union Range1F64
{
	struct
	{
		F64 min;
		F64 max;
	};
	F64 v[2];
};

// Range2 Types

typedef union Range2S16 Range2S16;
union Range2S16
{
	struct
	{
		S16 x0;
		S16 y0;
		S16 x1;
		S16 y1;
	};
	struct
	{
		Vec2S16 min;
		Vec2S16 max;
	};
};


typedef union Range2S32 Range2S32;
union Range2S32
{
	struct
	{
		S32 x0;
		S32 y0;
		S32 x1;
		S32 y1;
	};
	struct
	{
		Vec2S32 min;
		Vec2S32 max;
	};
};

typedef union Range2F32 Range2F32;
union Range2F32
{
	struct
	{
		Vec2F32 min;
		Vec2F32 max;
	};
	struct
	{
		Vec2F32 p0;
		Vec2F32 p1;
	};
	struct
	{
		F32 x0;
		F32 y0;
		F32 x1;
		F32 y1;
	};
	Vec2F32 v[2];
};


//~ Base Math Ops

#define SqrtF32(v)		sqrtf(v)
#define CbrtF32(v)		cbrtf(v)
#define PowF32(b, e)	powf((b), (e))
#define CeilF32(v)		ceilf(v)
#define FloorF32(v)		floorf(v)
#define RoundF32(v)		roundf(v)
#define SqrtF64(v)		sqrt(v)
#define CbrtF64(v)		cbrt(v)
#define PowF64(b, e)	pow((b), (e))
#define CeilF64(v)		ceil(v)
#define FloorF64(v)		floor(v)
#define RoundF64(v)		round(v)

#define ABS(a) ((a) > 0 ? (a) : -(a))
#define MOD(a, m) (((a) % (m)) >= 0 ? ((a) % (m)) : (((a) % (m)) + (m)))
#define SQUARE(x) ((x) * (x))



internal inline F32 InvSqrtF32(F32 value);

internal F32 Mix1F32(F32 a, F32 b, F32 t);

//~ Vector Ops


//~ VECTOR 2

//- F32
internal Vec2F32 	V2F32(F32 x, F32 y);
internal Vec2F32 	Add2F32(Vec2F32 a, Vec2F32 b);
internal Vec2F32 	Sub2F32(Vec2F32 a, Vec2F32 b);
internal Vec2F32 	Mul2F32(Vec2F32 a, Vec2F32 b);
internal Vec2F32 	Div2F32(Vec2F32 a, Vec2F32 b);
internal Vec2F32 	Scale2F32(Vec2F32 a, F32 b);
internal F32			Dot2F32(Vec2F32 a, Vec2F32 b);
internal F32 			LengthSquared2F32(Vec2F32 v);
internal F32 			Length2F32(Vec2F32 v);
internal Vec2F32	Normalized2F32(Vec2F32 v);
internal Vec2F32	Mix2F32(Vec2F32 a, Vec2F32 b, F32 t);

internal Vec2F32 Div2F32_F32(Vec2F32 a, F32 b);

//- F64 TODO:
//- S16 TODO:
//- S32 TODO:
//- S64 TODO:

//~ VECTOR 3

//- F32
internal Vec3F32 	V3F32(F32 x, F32 y, F32 z);
internal Vec3F32 	Add3F32(Vec3F32 a, Vec3F32 b);
internal Vec3F32 	Sub3F32(Vec3F32 a, Vec3F32 b);
internal Vec3F32	Mul3F32(Vec3F32 a, Vec3F32 b);
internal Vec3F32	Div3F32(Vec3F32 a, Vec3F32 b);
internal Vec3F32 	Scale3F32(Vec3F32 a, F32 b);
internal F32 			Dot3F32(Vec3F32 a, Vec3F32 b);
internal F32 			LengthSquared3F32(Vec3F32 v);
internal F32 			Length3F32(Vec3F32 v);
internal Vec3F32 	Normalized3F32(Vec3F32 v);
internal Vec3F32 Cross3F32(Vec3F32 a, Vec3F32 b);

internal Vec3F32 Div3F32_F32(Vec3F32 a, F32 b);

//- F64 TODO:
//- S16 TODO:
//- S32 TODO:
//- S64 TODO:

//~ VECTOR 4

//- F32
internal Vec4F32 	V4F32(F32 x, F32 y, F32 z, F32 w);
internal Vec4F32 	Add4F32(Vec4F32 a, Vec4F32 b);
internal Vec4F32 	Sub4F32(Vec4F32 a, Vec4F32 b);
internal Vec4F32 	Mul4F32(Vec4F32 a, Vec4F32 b);
internal Vec4F32 	Div4F32(Vec4F32 a, Vec4F32 b);
internal Vec4F32 	Scale4F32(Vec4F32 a, F32 b);
internal F32 			Dot4F32(Vec4F32 a, Vec4F32 b);
internal F32 			LengthSquared4F32(Vec4F32 v);
internal F32 			Length4F32(Vec4F32 v);
internal Vec4F32 	Normalized4F32(Vec4F32 v);


internal Vec4F32 Div4F32_F32(Vec4F32 a, F32 b);

//- F64 TODO:
//- S16 TODO:
//- S32 TODO:
//- S64 TODO:


internal B8 EqualV2(Vec2F32 a, Vec2F32 b);
internal B8 EqualV3(Vec3F32 a, Vec3F32 b);
internal B8 EqualV4(Vec4F32 a, Vec4F32 b);




// Range Ops

// Range1 U8
internal Range1U8 range_1u8(U8 min, U8 max);
internal U8 dim_1u8(Range1U8 r);

// Range1 U16
internal Range1U16 range_1u16(U16 min, U16 max);
internal U16 dim_1u16(Range1U16 r);

// Range1 U32
internal Range1U32 range_1u32(U32 min, U32 max);
internal U32 dim_1u32(Range1U32 r);

// Range1 U64
internal Range1U64 range_1u64(U64 min, U64 max);
internal U64 dim_1u64(Range1U64 r);


#define r1u8(min, max)	range_1u8((min), (max))
#define r1u16(min, max) range_1u16((min), (max))
#define r1u32(min, max) range_1u32((min), (max))
#define r1u64(min, max) range_1u64((min), (max))


// Color Operations TODO(seppo) Impl:

// hsv <-> rgb
internal Vec3F32 hsv_from_rgb(Vec3F32 rgb);
internal Vec3F32 rbg_from_hsv(Vec3F32 hsv);
internal Vec4F32 hsva_from_rgba(Vec4F32 rgba);
internal Vec4F32 rbga_from_hsva(Vec3F32 hsva);

// srgb <-> linear
internal Vec3F32 linear_from_srgb(Vec3F32 srgb);
internal Vec3F32 srgb_from_linear(Vec3F32 linear);
internal Vec4F32 linear_from_srgba(Vec4F32 srgba);
internal Vec4F32 srgba_from_linear(Vec4F32 linear);

// oklab <-> linear

internal Vec3F32 oklab_from_linear(Vec3F32 linear);
internal Vec3F32 linear_from_oklab(Vec3F32 oklab);
internal Vec4F32 oklab_from_lineara(Vec4F32 lineara);
internal Vec4F32 lineara_from_oklab(Vec4F32 oklab);

// rgba <-> u32
internal U32			u32_from_rgba(Vec4F32 rgba);
internal Vec4F32	rgba_from_u32(U32 hex);

#define rgba_from_u32_lit_comp(h) { (((h)&0xff000000)>>24)/255.f, (((h)&0x00ff0000)>>16)/255.f, (((h)&0x0000ff00)>> 8)/255.f, (((h)&0x000000ff)>> 0)/255.f }

#if !BASE_DISABLE_OMITTED_BASE_TYPES


// types
typedef Vec2F32 Vec2;
typedef Vec3F32 Vec3;
typedef Vec4F32 Vec4;

typedef Mat3x3F32 Mat3x3;
typedef Mat4x4F32 Mat4x4;


// constructors 

#define V2(x, y)					V2F32((x), (y))
#define V3(x, y, z)  			V3F32((x), (y), (z))
#define V4(x, y, z, w)  	V4F32((x), (y), (z), (w))


// 2d vector ops
#define Add2(a, b) 					Add2F32((a), (b))
#define Sub2(a, b) 					Sub2F32((a), (b))
#define Mul2(a, b) 					Mul2F32((a), (b))
#define Div2(a, b) 					Div2F32((a), (b))
#define Scale2(a, s) 				Scale2F32((a), (s))
#define Dot2(a, b)					Dot2F32((a), (b))
#define LengthSquared2(v)		LengthSquared2F32(v)
#define Length2(v)					Length2F32(v)
#define Normalized2(v)			Normalized2F32(v)
#define Mix2(a, b, t)				Mix2F32((a), (b), (t))

// 3d vector ops

// 4d vector ops


#endif // BASE_DISABLE_OMMITED_BASE_TYPES

#endif // BASE_MATH_H
