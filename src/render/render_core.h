#ifndef RENDER_CORE_H
#define RENDER_CORE_H

// Instance Types
typedef struct R_Rect2DInst R_Rect2DInst;
struct R_Rect2DInst
{
	Mat3x3F32 transform;
};

typedef struct R_Mesh3DInstance R_Mesh3DInstance;
struct R_Mesh3DInstance
{
	Mat4x4F32 transform;
};


// Vertex Format Types


// 256 bit VertexF256
typedef struct VertexF256 VertexF256;
struct VertexF256
{
	Vec3F32 position;
	Vec3F32 normal;
	Vec2F32 uv;
};


typedef U32 R_VertexAttribFlags;
enum
{
	R_VertexAttribFlag_XYZ			= (1 << 0),
	R_VertexAttribFlag_Normals	= (1 << 1),
	R_VertexAttribFlag_TexCoord = (1 << 2),
	R_VertexAttribFlag_RGB			= (1 << 3),
	R_VertexAttribFlag_RGBA			= (1 << 4),
	R_VertexAttribFlag_XYZW			= (1 << 5)
};


typedef U32 R_VertexAttribFormat;
enum
{
	R_VertexAttribFormat_VNT = (R_VertexAttribFlag_XYZ |
															R_VertexAttribFlag_Normals |
															R_VertexAttribFlag_TexCoord)
};

// Omitted Format Wrappers
typedef VertexF256 Vertex;


//- Render Hooks



internal void r_init(CmdLine *cmdline);





//


#endif // RENDER_CORE_H
