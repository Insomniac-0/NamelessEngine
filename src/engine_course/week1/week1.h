#ifndef WEEK_1_H
#define WEEK_1_H

#define W_WIDTH 800
#define W_HEIGHT 800

#define shader_source_from_file_path(arena, path)		data_from_file_path((arena), (path))

#define glUseProgramScope(...)              DeferLoop(glUseProgram(__VA_ARGS__), glUseProgram(0))
#define glBindVertexArrayScope(...)         DeferLoop(glBindVertexArray(__VA_ARGS__), glBindVertexArray(0))
#define glBindFramebufferScope(target, ...) DeferLoop(glBindFramebuffer((target), __VA_ARGS__), glBindFramebuffer((target), 0))
#define glBindTextureScope(target, ...)     DeferLoop(glBindTexture((target), __VA_ARGS__), glBindTexture((target), 0))
#define glEnableScope(...)                  DeferLoop(glEnable(__VA_ARGS__), glDisable(__VA_ARGS__))




typedef struct OGL_Attribute OGL_Attribute;
struct OGL_Attribute
{
	U64 index;
	String8 name;
	GLenum type;
	U64 count;
};

typedef struct OGL_AttributeArray OGL_AttributeArray;
struct OGL_AttributeArray
{
	OGL_Attribute *v;
	U64 count;
};


typedef struct OGL_State OGL_State;
struct OGL_State
{
	Arena *arena;
	GLuint default_vao;
	GLuint default_vbo;
	GLuint default_program;
};


// globals
global OGL_State *ogl_state = 0;

global F32 triangle_vbuffer[] =
	{
//    X			 Y			Z			 	  R      G     B   
		-1.0f,  1.0f,  0.0f,    1.0f,  0.0f,  0.0f,
		-1.0f, -1.0f,  0.0f,    0.0f,  1.0f,	0.0f,
		 1.0f, -1.0f,  0.0f,    0.0f,  0.0f,	1.0f,
	};

String8 week1_vshad_src = {0};
String8 week1_fshad_src = {0};

global B32 quit = 0;
global B32 space_is_down = 0;



// 

// callbacks
internal void ogl_debug_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);




// MeshBuilder Type

struct OGL_VertexBuffer
{
	F32 *vertices;
	F32 *indices;

	GLuint vao;
	GLuint vbo;
	GLuint ebo;

	U64 vertex_count;
	U64 index_count;
};


internal void ogl_load_functions();
internal void ogl_init();


internal void ogl_draw_triangle(Vec2F32 position, F32 turns, Vec2F32 scale, Vec3F32 color);
internal void ogl_draw_rectangle(Vec2F32 position, F32 turns, Vec2F32 scale, Vec3F32 color);


#endif // WEEK_1_H
