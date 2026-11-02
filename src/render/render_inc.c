#include "render_core.c"


#if R_BACKEND == R_BACKEND_OPENGL
# include "opengl/render_opengl.c"
#else
# error No renderer backend defined
#endif

