#ifndef RENDER_INC_H
#define RENDER_INC_H


// This is just so I can swap out the backend in the future if I need

#define R_BACKEND_OPENGL 0

#define R_BACKEND R_BACKEND_OPENGL

#include "render_core.h"


#if R_BACKEND == R_BACKEND_OPENGL
# include "opengl/render_opengl.h"
#else
# error No renderer backend defined
#endif



#endif // RENDER_INC_H
