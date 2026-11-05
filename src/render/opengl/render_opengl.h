#ifndef RENDER_OPENGL_H
#define RENDER_OPENGL_H


typedef struct R_OGL_Attribute R_OGL_Attribute;
struct R_OGL_Attribute
{
	U64 index;
	String8 name;
	GLenum type;
	U64 count;
};

typedef struct R_OGL_AttributeArray R_OGL_AttributeArray;
struct R_OGL_AttributeArray
{
	R_OGL_Attribute *v;
	U64 count;
};

typedef struct R_OGL_State R_OGL_State;
struct R_OGL_State
{
	Arena *arena;
	GLuint default_vao;
	GLuint default_vbo;
	GLuint default_program;
};


internal void r_ogl_load_functions();
internal void r_ogl_init(CmdLine *cmdline);

//~ Helpers

#define shader_source_from_file_path(arena, path)		data_from_file_path((arena), (path))

#define glUseProgramScope(...)              DeferLoop(glUseProgram(__VA_ARGS__), glUseProgram(0))
#define glBindBufferScope(target, ...)			DeferLoop(glBindBuffer((target), __VA_ARGS__), glBindBuffer((target), 0))
#define glBindVertexArrayScope(...)         DeferLoop(glBindVertexArray(__VA_ARGS__), glBindVertexArray(0))
#define glBindFramebufferScope(target, ...) DeferLoop(glBindFramebuffer((target), __VA_ARGS__), glBindFramebuffer((target), 0))
#define glBindTextureScope(target, ...)     DeferLoop(glBindTexture((target), __VA_ARGS__), glBindTexture((target), 0))
#define glEnableScope(...)                  DeferLoop(glEnable(__VA_ARGS__), glDisable(__VA_ARGS__))


internal void r_ogl_debug_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);


#endif // RENDER_OPENGL_H
