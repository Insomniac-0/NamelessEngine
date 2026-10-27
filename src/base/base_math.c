internal inline F32
InvSqrtF32(F32 value)
{
	F32 result;

	result = 1.0f/SqrtF32(value);

	return result;
}

internal F32
Mix1F32(F32 a, F32 b, F32 t)
{
	return a*(1-t) + b*t;
}


//~ VECTOR 2

//- F32
internal Vec2F32
V2F32(F32 x, F32 y)
{
	Vec2F32 result;

	result.x = x;
	result.y = y;

	return result;
}

internal Vec2F32
Add2F32(Vec2F32 a, Vec2F32 b)
{
	Vec2F32 result;

	result.x = a.x + b.x;
	result.y = a.y + b.y;

	return result;
}


internal Vec2F32
Sub2F32(Vec2F32 a, Vec2F32 b)
{
	Vec2F32 result;

	result.x = a.x - b.x;
	result.y = a.y - b.y;

	return result;
}



internal Vec2F32
Mul2F32(Vec2F32 a, Vec2F32 b)
{
	Vec2F32 result;

	result.x = a.x * b.x;
	result.y = a.y * b.y;

	return result;
}


internal Vec2F32
Div2F32(Vec2F32 a, Vec2F32 b)
{
	Vec2F32 result;

	result.x = a.x / b.x;
	result.y = a.y / b.y;

	return result;
}


internal Vec2F32
Scale2F32 (Vec2F32 a, F32 b)
{
	Vec2F32 result;

	result.x = a.x * b;
	result.y = a.y * b;

	return result;
}


internal F32
Dot2F32(Vec2F32 a, Vec2F32 b)
{
	return (a.x*b.x + a.y*b.y);
}

internal F32
LengthSquared2F32(Vec2F32 v)
{
	return Dot2F32(v, v);
}



internal F32
Length2F32(Vec2F32 v)
{
	return SqrtF32(LengthSquared2F32(v));
}


internal Vec2F32
Normalized2F32(Vec2F32 v)
{
	return Scale2F32(v, InvSqrtF32(Dot2F32(v, v)));
}


internal Vec2F32
Mix2F32(Vec2F32 a, Vec2F32 b, F32 t)
{
	Vec2F32 result;
	result.x = a.x*(1-t) + b.x*t;
	result.y = a.y*(1-t) + b.y*t;
	return result;
}

internal Vec2F32
Div2F32_F32(Vec2F32 a, F32 b)
{
	Vec2F32 result;

	result.x = a.x / b;
	result.y = a.y / b;

	return result;
}


//- F64 TODO:
//- S16 TODO:
//- S32 TODO:
//- S64 TODO:


//~ VECTOR 3

//- F32

internal Vec3F32
V3F32(F32 x, F32 y, F32 z)
{
	Vec3F32 result;

	result.x = x;
	result.y = y;
	result.z = z;

	return result;
}




internal Vec3F32
Add3F32(Vec3F32 a, Vec3F32 b)
{
	Vec3F32 result;

	result.x = a.x + b.x;
	result.y = a.y + b.y;
	result.z = a.z + b.z;

	return result;
}

internal Vec3F32
Sub3F32(Vec3F32 a, Vec3F32 b)
{
	Vec3F32 result;

	result.x = a.x - b.x;
	result.y = a.y - b.y;
	result.z = a.z - b.z;

	return result;
}

internal Vec3F32
Mul3F32 (Vec3F32 a, Vec3F32 b)
{
	Vec3F32 result;

	result.x = a.x * b.x;
	result.y = a.y * b.y;
	result.z = a.z * b.z;

	return result;
}

internal Vec3F32
Div3F32(Vec3F32 a, Vec3F32 b)
{
	Vec3F32 result;

	result.x = a.x / b.x;
	result.y = a.y / b.y;
	result.z = a.z / b.z;

	return result;
}


internal Vec3F32
Scale3F32(Vec3F32 a, F32 b)
{
	Vec3F32 result;

	result.x = a.x * b;
	result.y = a.y * b;
	result.z = a.z * b;

	return result;
}



internal F32
Dot3F32(Vec3F32 a, Vec3F32 b)
{
	return((a.x * b.x) +
			  (a.y * b.y)  +
			  (a.z * b.z));
}



internal F32
LengthSquared3F32(Vec3F32 v)
{
	return Dot3F32(v, v);
}


internal F32
Length3F32(Vec3F32 v)
{
	return SqrtF32(LengthSquared3F32(v));
}


internal Vec3F32
Normalized3F32(Vec3F32 v)
{
	return Scale3F32(v, InvSqrtF32(Dot3F32(v, v)));
}

internal Vec3F32
Cross3F32(Vec3F32 a, Vec3F32 b)
{
	Vec3F32 result;

	result.x = (a.y * b.z) - (a.z * b.y);
	result.y = (a.z * b.x) - (a.x * b.z);
	result.z = (a.x * b.y) - (a.y * b.x);

	return result;
}



internal Vec3F32
Div3F32_F32(Vec3F32 a, F32 b)
{
	Vec3F32 result;

	result.x = a.x / b;
	result.y = a.y / b;
	result.z = a.z / b;

	return result;
}

//- F64 TODO:
//- S16 TODO:
//- S32 TODO:
//- S64 TODO:


//~ VECTOR 4

internal Vec4F32
V4F32(F32 x, F32 y, F32 z, F32 w)
{
	Vec4F32 result;

	result.x = x;
	result.y = y;
	result.z = z;
	result.w = w;

	return result;
}

internal Vec4F32
Add4F32(Vec4F32 a, Vec4F32 b)
{
	Vec4F32 result;

	result.x = a.x + b.x;
	result.y = a.y + b.y;
	result.z = a.z + b.z;
	result.w = a.w + b.w;

	return result;
}

internal Vec4F32
Sub4F32(Vec4F32 a, Vec4F32 b)
{
	Vec4F32 result;

	result.x = a.x - b.x;
	result.y = a.y - b.y;
	result.z = a.z - b.z;
	result.w = a.w - b.w;

	return result;
}


internal Vec4F32
Mul4F32(Vec4F32 a, Vec4F32 b)
{
	Vec4F32 result;

	result.x = a.x * b.x;
	result.y = a.y * b.y;
	result.z = a.z * b.z;
	result.w = a.w * b.w;

	return result;
}

internal Vec4F32
Div4F32(Vec4F32 a, Vec4F32 b)
{
	Vec4F32 result;

	result.x = a.x / b.x;
	result.y = a.y / b.y;
	result.z = a.z / b.z;
	result.w = a.w / b.w;

	return result;
}


internal Vec4F32
Scale4F32(Vec4F32 a, F32 b)
{
	Vec4F32 result;

	result.x = a.x * b;
	result.y = a.y * b;
	result.z = a.z * b;
	result.w = a.w * b;

	return result;
}

internal F32
Dot4F32(Vec4F32 a, Vec4F32 b)
{
	F32 result;
	result = ((a.x * b.x) +
						(a.y * b.y) +
						(a.z * b.z) +
						(a.w * b.z));

	return result;
}


internal F32
LengthSquared4F32(Vec4F32 v)
{
	return Dot4F32(v, v);
}

internal F32
Length4F32(Vec4F32 v)
{
	return SqrtF32(LengthSquared4F32(v));
}

internal Vec4F32
Div4F32_F32(Vec4F32 a, F32 b)
{
	Vec4F32 result;

	result.x = a.x / b;
	result.y = a.y / b;
	result.z = a.z / b;
	result.w = a.w / b;

	return result;
}

//- F64 TODO:
//- S16 TODO:
//- S32 TODO:
//- S64 TODO:



//~ Matrix Constructors TODO:

//- 3x3
internal Mat3x3F32
MakeMat3x3F32(F32 d)
{
	Mat3x3F32 result =
	{
		{
			{d, 0, 0},
			{0, d, 0},
			{0, 0, d},
		},
	};
	return result;
}


//- 4x4
internal Mat4x4F32
MakeMat4x4F32(F32 d)
{
	Mat4x4F32 result =
	{
		{
			{d, 0, 0, 0},
			{0, d, 0, 0},
			{0, 0, d, 0},
			{0, 0, 0, 1},
		},
	};
	return result;
}



internal B8
EqualV2(Vec2F32 a, Vec2F32 b)
{
	return ((a.x == b.x) &&
			(a.y == b.y));
}

internal B8
EqualV3(Vec3F32 a, Vec3F32 b)
{
	return ((a.x == b.x) &&
			(a.y == b.y) &&
			(a.z == b.z));
}

internal B8
EqualV4(Vec4F32 a, Vec4F32 b)
{
	return ((a.x == b.x) &&
			(a.y == b.y) &&
			(a.z == b.z) &&
			(a.w == b.w));
}










/*----- Unary Vector Operations ----*/














internal Vec4F32
Normalized4F32(Vec4F32 v)
{
	return Scale4F32(v, InvSqrtF32(Dot4F32(v, v)));
}



// Matrix Ops

// Range Ops

// Range1 U8
internal Range1U8 range_1u8(U8 min, U8 max) 		{ Range1U8 r = {min, max}; if (r.min > r.max) { Swap(U8, r.min, r.max); } return r; }
internal U8 dim_1u8(Range1U8 r)									{ U8 c = ((r.max > r.min ) ? (r.max - r.min) : 0); return c; }

internal Range1U16 range_1u16(U16 min, U16 max) { Range1U16 r = {min, max}; if (r.min > r.max) { Swap(U16, r.min, r.max); } return r; }
internal U16 dim_1u16(Range1U16 r)							{ U16 c = ((r.max > r.min ) ? (r.max - r.min) : 0); return c; }


internal Range1U32 range_1u32(U32 min, U32 max) { Range1U32 r = {min, max}; if (r.min > r.max) { Swap(U32, r.min, r.max); } return r; }
internal U32 dim_1u32(Range1U32 r)							{ U32 c = ((r.max > r.min ) ? (r.max - r.min) : 0); return c; }

internal Range1U64 range_1u64(U64 min, U64 max) { Range1U64 r = {min, max}; if (r.min > r.max) { Swap(U64, r.min, r.max); } return r; }
internal U64 dim_1u64(Range1U64 r)							{ U64 c = ((r.max > r.min ) ? (r.max - r.min) : 0); return c; }


// CPP Operator Overloads

//#if LANG_CPP

/*----- OP OVERLOAD	----*/

/*		   OP +			*/

/* internal Vec2F32 */
/* operator+(Vec2F32 a, Vec2F32 b) */
/* { */
/* 	return Add2F32(a, b); */
/* } */

/* internal Vec3F32 */
/* operator+(Vec3F32 a, Vec3F32 b) */
/* { */
/* 	return Add3F32(a, b); */
/* } */

/* internal Vec4F32 */
/* operator+(Vec4F32 a, Vec4F32 b) */
/* { */
/* 	return Add4F32(a, b); */
/* } */
/* /\*		   OP -			*\/ */

/* internal Vec2F32 */
/* operator-(Vec2F32 a, Vec2F32 b) */
/* { */
/* 	return Sub2F32(a, b); */
/* } */

/* internal Vec3F32 */
/* operator-(Vec3F32 a, Vec3F32 b) */
/* { */
/* 	return Sub3F32(a, b); */
/* } */

/* internal Vec4F32 */
/* operator-(Vec4F32 a, Vec4F32 b) */
/* { */
/* 	return Sub4F32(a, b); */
/* } */

/* /\*		   OP *			*\/ */

/* internal Vec2F32 */
/* operator*(Vec2F32 a, Vec2F32 b) */
/* { */
/* 	return Mul2F32(a, b); */
/* } */

/* internal Vec3F32 */
/* operator*(Vec3F32 a, Vec3F32 b) */
/* { */
/* 	return (a, b); */
/* } */

/* internal Vec4F32 */
/* operator*(Vec4F32 a, Vec4F32 b) */
/* { */
/* 	return F32(a, b); */
/* } */

/* internal Vec2F32 */
/* operator*(Vec2F32 a, F32 b) */
/* { */
/* 	return Mul2F32F(a, b); */
/* } */

/* internal Vec3F32 */
/* operator*(Vec3F32 a, F32 b) */
/* { */
/* 	return Scale3F32(a, b); */
/* } */

/* internal Vec4F32 */
/* operator*(Vec4F32 a, F32 b) */
/* { */
/* 	return F32F(a, b); */
/* } */


/* /\*		   OP /			*\/ */

/* internal Vec2F32 */
/* operator/(Vec2F32 a, Vec2F32 b) */
/* { */
/* 	return Div2F32(a, b); */
/* } */

/* internal Vec3F32 */
/* operator/(Vec3F32 a, Vec3F32 b) */
/* { */
/* 	return Div3F32(a, b); */
/* } */

/* internal Vec4F32 */
/* operator/(Vec4F32 a, Vec4F32 b) */
/* { */
/* 	return Div4F32(a, b); */
/* } */

/* internal Vec2F32 */
/* operator/(Vec2F32 a, F32 b) */
/* { */
/* 	return Div2F32F(a, b); */
/* } */

/* internal Vec3F32 */
/* operator/(Vec3F32 a, F32 b) */
/* { */
/* 	return Div3F32F(a, b); */
/* } */

/* internal Vec4F32 */
/* operator/(Vec4F32 a, F32 b) */
/* { */
/* 	return Div4F32F(a, b); */
/* } */


/* /\*		   OP +=	   	*\/ */

/* internal Vec2F32 */
/* &operator+=(Vec2F32 &a, Vec2F32 b) */
/* { */
/* 	return a = a + b; */
/* } */

/* internal Vec3F32 */
/* &operator+=(Vec3F32 &a, Vec3F32 b) */
/* { */
/* 	return a = a + b; */
/* } */

/* internal Vec4F32 */
/* &operator+=(Vec4F32 &a, Vec4F32 b) */
/* { */
/* 	return a = a + b; */
/* } */

/* /\*		   OP -=	   	*\/ */


/* /\*		   OP *=	   	*\/ */

/* internal Vec2F32 */
/* &operator*=(Vec2F32 &a, F32 b) */
/* { */
/* 	return a = a * b; */
/* } */

/* internal Vec3F32 */
/* &operator*=(Vec3F32 &a, F32 b) */
/* { */
/* 	return a = a * b; */
/* } */

/*		   OP /=	   	*/


/* #endif // LANG_CPP */
