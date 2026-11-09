#define BUILD_TARGET "Week3: OBJ Loading"

// External Includes
#include <GL/glew.h>
#include <SDL3/SDL.h>


// Layer Includes [H]
#include "base/base_inc.h"
#include "obj_loader/obj_loader.h"
#include "geo3d/geo3d_inc.h"
#include "render/render_inc.h"
#include "week3.h"

// Layer Includes [C]
#include "base/base_inc.c"
#include "obj_loader/obj_loader.c"
#include "geo3d/geo3d_inc.c"
#include "render/render_inc.c"
#include "week3.c"

// Generated



internal void
entry_point(CmdLine *cmdline)
{
	Temp scratch = scratch_begin(0, 0);

	SDL_InitFlags sdl_flags = SDL_INIT_VIDEO | SDL_INIT_EVENTS;
	if(!SDL_Init(sdl_flags))
	{
		fprintf(stderr, "SDL_Init Failed...\n");
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);


#if !defined(BUILD_TARGET)
  #define BUILD_TARGET "week3"
#endif

	SDL_WindowFlags win_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
	SDL_Window *window = SDL_CreateWindow(BUILD_TARGET, W_WIDTH, W_HEIGHT, win_flags);
	SDL_GLContext gl_ctx = SDL_GL_CreateContext(window);

	AppInit(W_WIDTH, W_HEIGHT);

	r_ogl_load_functions();
	r_ogl_init(cmdline);


	// OBJ LOADING TEST





	String8 meshdata_path = str8f(scratch.arena, "%S/data/meshdata", get_process_info()->binary_path);
	String8 obj_src_path = str8f(scratch.arena, "%S/triangulated_cube.obj", meshdata_path);

	String8 obj_data = data_from_file_path(scratch.arena, obj_src_path);
	//fprintf(stderr, "%s\n", obj_str.str);

	OBJ_TokenizeResult tokenize = obj_tokenize_from_string(scratch.arena, obj_data);

	U64 pos_count = 0;
	U64 normal_count = 0;
	U64 texcoord_count = 0;
	U64 face_count = 0;

	for EachIndex(idx, tokenize.tokens.count)
	{

		OBJ_TokenFlags flags = tokenize.tokens.v[idx].flags;

		String8 raw_str = {0};
		String8 fmt_str = {0};

		raw_str = str8_substr(obj_data, tokenize.tokens.v[idx].range);

		if (flags & OBJ_TokenFlag_Identifier)
		{
			if (str8_match(raw_str, str8_lit("v"), 0))
			{
				fmt_str = str8_lit("Vertex Position: ");
				pos_count += 1;
			}
			else if (str8_match(raw_str, str8_lit("vn"), 0))
			{
				fmt_str = str8_lit("Vertex Normal: ");
				normal_count += 1;
			}
			else if (str8_match(raw_str, str8_lit("vt"), 0))
			{
				fmt_str = str8_lit("TexCoord: ");
				texcoord_count += 1;
			}
			else if (str8_match(raw_str, str8_lit("f"), 0))
			{
				fmt_str = str8_lit("Face Indices: ");
				face_count += 1;
			}
			else
			{
				fmt_str = str8_lit("");
			}
		}
		else if (flags & OBJ_TokenFlag_Numeric)
		{
			fmt_str = str8f(scratch.arena, "%S ", raw_str);
		}
		else if (flags & OBJ_TokenFlag_Newline)
		{
			fmt_str = str8_lit("\n");
		}
		else if (flags & OBJ_TokenFlag_Newline)
		{
			fmt_str = str8_lit("\n");
		}
		else
		{
			fmt_str = str8_lit("");
		}

		fprintf(stderr, "%s", fmt_str.str);
	}
	fprintf(stderr,
					"\nPosition Count: %lu\nNormal Count: %lu\nTexCoord Count: %lu\nFace Count:%lu\n",
					pos_count,
					normal_count,
					texcoord_count,
					face_count);
					
					


	U64 last_time = now_time_us();
	U64 now_time = now_time_us();
	for (;!quit;)
	{
		now_time = now_time_us();
		F64 dt = (F64)(now_time - last_time) / Million(1);
		last_time = now_time;


		glClearColor(0.05f, 0.05f, 0.05f, 0.5f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		SDL_GL_SwapWindow(window);
		HandleInput();
	}

	scratch_end(scratch);
}
