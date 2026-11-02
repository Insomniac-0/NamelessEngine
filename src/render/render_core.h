#ifndef RENDER_CORE_H
#define RENDER_CORE_H



typedef struct R_Rect2DInst R_Rect2DInst;
struct R_Rect2DInst
{
	Mat3x3F32 transform;
};

typedef struct R_Mesh3DInstance R_Mesh3DInstance;
struct R_Mesh3dInstance
{
	Mat4x4F32 transform;
};



//- Render Hooks

internal void r_init(CmdLine *cmdline);


#endif // RENDER_CORE_H
