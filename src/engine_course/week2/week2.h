#ifndef WEEK_2_H
#define WEEK_2_H

#define W_WIDTH 1280
#define W_HEIGHT 960


#define DEMO_COUNT 5
#define SHADER_COUNT 1

#define FOV 45.0f
#define ASPECT W_WIDTH / W_HEIGHT
#define NEAR_Z 0.1f
#define FAR_Z 100.0f

#define MAX_BUFFER_COUNT 10



// Temporary 'Entity' Struct
typedef struct Entity Entity;
struct Entity
{
	// transform data
	Vec3F32 position;
	Vec3F32 scale;
	F32 rotation; // just rotating around y axis
};

// scratch / api sketching

// maybe have default simple mesh3d attribs like:
// in vec3 position
// in	vec3 normal 
// in vec2 texcoord 
// in vec3 vertex_color

typedef struct VertexIndexArrayPair VertexIndexArrayPair;
struct VertexIndexArrayPair
{
	Vec3F32Array vertices;
	S32Array indices;
};

typedef struct OGL_Buffer OGL_Buffer;
struct OGL_Buffer
{
	GLenum target;
	GLenum usage;
	GLuint handle;
	U64 size;
	void *data;
};


// Temporary solution to just easily create and draw stuff
typedef struct OGL_Mesh3D OGL_Mesh3D;
struct OGL_Mesh3D
{
	VertexIndexArrayPair vtx_idx_pair;
	R_OGL_AttributeArray attribs;
	OGL_Buffer *buffers;
	OGL_Buffer element_buffer;
	GLuint vao;
};
// mesh struct might look something like
/*

	struct Mesh3D
	{
		Vec3F32 position[];
		Vec3F32 normal[];
		Vec2F32 texcoords[];

		S32 indices[];
	}

 */



typedef struct OGL_BufferEntry OGL_BufferEntry;
struct OGL_BufferEntry
{
	U64 base_pos;
	U64 offet;

	GLenum type;
	GLuint handle;
	U64 shared_buffer_offset;
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


typedef struct Camera Camera;
struct Camera
{
	Mat4x4F32 projection;
};

typedef struct Shader Shader;
struct Shader
{
	String8 name;
};

typedef struct W2_State W2_State;
struct W2_State
{
	Arena *arena;

	GLuint general_purpose_vao;

	OGL_Mesh3D *pyramid_mesh;
	OGL_Mesh3D *cube_mesh;
	OGL_Mesh3D *icosphere_mesh;
	OGL_Mesh3D *uvsphere_mesh;


	GLuint shaders[SHADER_COUNT];
	// Paths
	Arena *paths_arena;
	String8 shader_dir_path;

	// Camera
	Camera debug_camera;
	Camera debug_camera_ortho;
	Vec3F32 debug_camera_pos;
	Vec3F32 debug_camera_target;
};


// globals
global W2_State *g_w2_state = 0;
global OGL_State *g_ogl_state = 0;

global B32 key_a_is_down = 0;
global B32 quit = 0;
global B32 space_is_down = 0;
global B32 key_shift_is_down = 0;

global B32 key_d_is_down = 0;
global B32 key_w_is_down = 0;
global B32 key_s_is_down = 0;

global B32 key_left_is_down = 0;
global B32 key_right_is_down = 0;
global B32 key_up_is_down = 0;
global B32 key_down_is_down = 0;

global U64 demo_index = 4;
global B32 ortho_camera = 0;


global F32 g_pyramid_colors[] =
{
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 0.0f,
};
global F32 g_pyramid_vertices[] =
{
	// position							 // color
	-1.0f, -1.0f, -1.0f,    1.0f, 0.0f, 0.0f,
	 1.0f, -1.0f, -1.0f,    0.0f, 1.0f, 0.0f,
	 0.0f, 	1.0f,  0.0f,    1.0f, 1.0f, 1.0f,	 // APEX
	 1.0f, -1.0f,  1.0f,    0.0f, 0.0f, 1.0f,
	-1.0f, -1.0f,  1.0f,    0.0f, 0.0f, 0.0f,
};
global U32 g_pyramid_indices[] =
{
	0, 1, 2,
	1, 3, 2,
	3, 4, 2,
	4, 0, 2,
	0, 4, 1,
	1, 4, 3
};

global F32 g_cube_vertices[] =
{
	// F
	-1.0f, -1.0f, -1.0f,    1.0f, 0.0f, 0.0f, // bot-left
	 1.0f, -1.0f, -1.0f,    1.0f, 0.0f, 0.0f, // bot-right
	 1.0f,  1.0f, -1.0f,		1.0f, 0.0f, 0.0f, // top-right
	 -1.0f,  1.0f, -1.0f,		1.0f, 0.0f, 0.0f, // top-left

	// B
	 1.0f, -1.0f,  1.0f,    1.0f, 1.0f, 1.0f, // bot-right
	-1.0f, -1.0f,  1.0f,    1.0f, 1.0f, 1.0f, // bot-left
	-1.0f,  1.0f,  1.0f,    1.0f, 1.0f, 1.0f, // top-left
	 1.0f,  1.0f,  1.0f,    1.0f, 1.0f, 1.0f  // top-right
};
global F32 g_cube_colors[] =
{
  1.0f, 0.0f, 0.0f, // bot-left
  1.0f, 0.0f, 0.0f, // bot-right
	1.0f, 0.0f, 0.0f, // top-right
	1.0f, 0.0f, 0.0f, // top-left
  1.0f, 1.0f, 1.0f, // bot-right
  1.0f, 1.0f, 1.0f, // bot-left
  1.0f, 1.0f, 1.0f, // top-left
  1.0f, 1.0f, 1.0f  // top-right
};
global F32 g_icosphere_colors[] =
{
	1.0f, 1.0f, 1.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 0.0f, 0.0f,
};

global U32 g_cube_indices[] =
{
	// Front Face
	0, 1, 2,
	2, 3, 0,
	// Left Face
	0, 3, 5,
	5, 3, 6,
	// Back Face
	6, 7, 5,
	5, 7, 4,
	// Right Face
	4, 7, 2,
	2, 1, 4,
	// Bot Face
	4, 1, 5,
	5, 1, 0,
	// Top Face
	3, 2, 7,
	7, 6, 3
};



internal void w2_init();

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);




// Ideas:
// Attribute |  index  |  name  |  type  |  count (0..4)  |
// VertexData  | data | size |
// AttributeDataPair |  Attribute  | VertexData |

// create table of attributes with table gen and just iterate over pair to setup buffers
// 

// Mesh3D
internal OGL_Mesh3D *ogl_mesh3d_alloc(Arena *arena, R_OGL_AttributeArray attribs);

//- Built in mesh ------------------------------------------------------------------------------
// TODO(seppo): move to some other layer, does not belong with opengl stuff
internal VertexIndexArrayPair ogl_make_icosphere(Arena* arena, U64 num_subdivide_passes);
internal VertexIndexArrayPair ogl_make_cube(Arena* arena);
internal VertexIndexArrayPair ogl_make_pyramid(Arena* arena);
internal VertexIndexArrayPair ogl_make_uvsphere(Arena *arena, U64 rings, U64 slices);
// Creating meshes should not be done like this this is only for built in meshes
// Idealy I have mesh builder system where i can just push vertices push normals push indices etc..
// need some scratch buffer to push vertices on and copy them to a "permanent" arena when done building
// just push large array and update count when pushing then copy only the portion neeeded,
// if exceeding the large array just push another and create a linked list of them
// and stitch them together when done
//--------------------------------------------------------------------------------------------

// Buffers 
internal OGL_Buffer ogl_buffer_alloc(GLenum type, U64 size, void *data, GLenum usage);
internal void ogl_bind_buffer(OGL_Buffer buffer);


// simple draw
// TODO(seppo): theese could be 1 function and you pass the mesh so something like:
// ogl_draw_mesh(Mesh mesh, Vec3F32 position, ?? rotation, Vec3F32 scale) 
// this is obviously not ideal for actual rendering the game that will be done with instanced rendering
// and render groups and batches
internal void ogl_draw_pyramid(Vec3F32 position, Vec3F32 rotation_axis, Vec3F32 scale, F32 rotation_turns);
internal void ogl_draw_cube(Vec3F32 position, Vec3F32 rotation_axis, Vec3F32 scale, F32 rotation_turns);
internal void ogl_draw_icosphere(Vec3F32 position, Vec3F32 rotation_axis, Vec3F32 scale, F32 rotation_turns);
internal void ogl_draw_uvsphere(Vec3F32 position, Vec3F32 rotation_axis, Vec3F32 scale, F32 rotation_turns);


// camera
internal Camera make_camera_perspective(F32 fov, F32 aspect, F32 near_z, F32 far_z);
internal Camera make_camera_orthographic(F32 left, F32 right, F32 bottom, F32 top, F32 near_z, F32 far_z);
internal Mat4x4F32 get_debug_camera_view(void);
internal Mat4x4F32 get_debug_camera_projection(void);


internal void ogl_toggle_wireframe(void);




#endif // WEEK_2_H
