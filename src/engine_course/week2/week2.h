#ifndef WEEK_2_H
#define WEEK_2_H

#define W_WIDTH 1280
#define W_HEIGHT 960


typedef struct VertexIndexArrayPair VertexIndexArrayPair;
struct VertexIndexArrayPair
{
	Vec3F32Array vertices;
	S32Array indices;
};

typedef struct OGL_State OGL_State;
struct OGL_State
{
	Arena *arena;
	GLuint default_vao;
	GLuint default_vbo;
	GLuint default_program;
};


typedef struct OGL_VertexBuffer OGL_VertexBuffer;
struct OGL_VertexBuffer
{
	R_OGL_AttributeArray attributes;
	VertexIndexArrayPair vrt_idx_pair;

	GLuint vao;
	GLuint vbo;
	GLuint ebo;
};


typedef struct W2_State W2_State;
struct W2_State
{
	Arena *arena;


	// Paths
	Arena *paths_arena;
	String8 shader_path;
};


// globals
global W2_State *w2_state = 0;
global OGL_State *ogl_state = 0;
global B32 quit = 0;
global B32 space_is_down = 0;


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);






#endif // WEEK_1_H
