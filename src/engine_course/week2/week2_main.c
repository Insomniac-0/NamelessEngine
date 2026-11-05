// External Includes
#include <GL/glew.h>
#include <GLFW/glfw3.h>


// Layer Includes [H]
#include "base/base_inc.h"
#include "render/render_inc.h"
#include "week2.h"

// Layer Includes [C]
#include "base/base_inc.c"
#include "render/render_inc.c"
#include "week2.c"


// None generated right now
/* #include "generated/engine_course_week2.meta.h" */
/* #include "generated/engine_course_week2.meta.c" */


// defines & globals
#define W_WIDTH 1280
#define W_HEIGHT 960


// shader sketch
// in vec3 i2v_pos
// in vec3 i2v_normal
// in vec2 i2v_texcoord

// uniform mat4 transform
// uniform mat4 view 
// uniform mat4 projection 



global R_OGL_Attribute g_ogl_pyramid_input_attributes[] =
{
	{0,				str8_lit_comp("i2v_pos"),				GL_FLOAT, 3},
	{1,				str8_lit_comp("i2v_color"),			GL_FLOAT, 3},
};

global R_OGL_Attribute g_ogl_mesh3d_input_attributes[] =
{
	{0,	str8_lit_comp("i2v_pos"),				GL_FLOAT, 3},
	{1, str8_lit_comp("i2v_normal"),		GL_FLOAT, 3},
	{2, str8_lit_comp("i2v_texcoord"),	GL_FLOAT, 2},
};

internal void
entry_point(CmdLine *cmdline)
{
	Temp scratch = scratch_begin(0, 0);
	// This should eventually be replaced with abstraction layer for window handling
	// and renderer will have a way to "attach" to the window
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);  
	glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
	glfwWindowHint(GLFW_MAXIMIZED, GLFW_FALSE);

	GLFWwindow* window = 0;
	window = glfwCreateWindow(W_WIDTH, W_HEIGHT, "Week2", NULL, NULL);

	if (window == NULL)
	{
		fprintf(stderr, "Failed to create GLFW window\n");
		glfwTerminate();
		return;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);

	r_ogl_load_functions();
	r_ogl_init(cmdline);

	w2_init();


	String8 vshad_src_path = str8f(g_w2_state->paths_arena, "%S/week2_vshad.glsl", g_w2_state->shader_dir_path);
	String8 fshad_src_path = str8f(g_w2_state->paths_arena, "%S/week2_fshad.glsl", g_w2_state->shader_dir_path);

	String8 vshad_src = shader_source_from_file_path(g_w2_state->paths_arena, vshad_src_path);
	String8 fshad_src = shader_source_from_file_path(g_w2_state->paths_arena, fshad_src_path);

	// Build Shaders

	struct {GLenum type; String8 *src; GLuint out; String8 errors;} stages[] =
	{
		{ GL_VERTEX_SHADER,			&vshad_src },
		{ GL_FRAGMENT_SHADER, 	&fshad_src },
	};

	//- Compile shaders
	for EachElement(idx, stages)
	{
		stages[idx].out = glCreateShader(stages[idx].type);
		GLint src_size = stages[idx].src->size;

		glShaderSource(stages[idx].out, 1, (char **)&stages[idx].src->str, &src_size);
		glCompileShader(stages[idx].out);

		GLint info_log_length = 0;
		GLint status = 0;
		glGetShaderiv(stages[idx].out, GL_COMPILE_STATUS, &status);
		glGetShaderiv(stages[idx].out, GL_INFO_LOG_LENGTH, &info_log_length);

		if (info_log_length != 0)
		{
			stages[idx].errors.str = push_array(scratch.arena, U8, info_log_length+1);
			stages[idx].errors.size = info_log_length;

			glGetShaderInfoLog(stages[idx].out, info_log_length, 0, (char *)stages[idx].errors.str);
			fprintf(stderr, "asd%s\n", stages[idx].errors.str);
		}
	}

	//Attach compilations
	GLuint program = glCreateProgram();
	for EachElement(idx, stages)
	{
		glAttachShader(program, stages[idx].out);
	}

	// Bind Attributes
	R_OGL_AttributeArray attribs = { g_ogl_pyramid_input_attributes, ArrayCount(g_ogl_pyramid_input_attributes) };
	for EachIndex(idx, attribs.count)
	{
		glBindAttribLocation(program, attribs.v[idx].index, (char *)attribs.v[idx].name.str);
	}

	// link -> validate -> store
	glLinkProgram(program);
	glValidateProgram(program);
	g_w2_state->shaders[0] = program;


	// PYRAMID MESH
	{
		VertexIndexArrayPair pyramid = ogl_make_pyramid(g_w2_state->arena);
		OGL_Mesh3D *m = ogl_mesh3d_alloc(g_w2_state->arena, attribs);
		m->vtx_idx_pair = pyramid;
		glBindVertexArray(m->vao);

		m->buffers[0] = ogl_buffer_alloc(GL_ARRAY_BUFFER, sizeof(Vec3F32)*pyramid.vertices.count, pyramid.vertices.v, GL_STATIC_DRAW);
		m->buffers[1] = ogl_buffer_alloc(GL_ARRAY_BUFFER, sizeof(g_pyramid_colors), g_pyramid_colors, GL_STATIC_DRAW);
		m->element_buffer = ogl_buffer_alloc(GL_ELEMENT_ARRAY_BUFFER, sizeof(S32)*pyramid.indices.count, pyramid.indices.v, GL_STATIC_DRAW); 


		glBindBuffer(GL_ARRAY_BUFFER, m->buffers[0].handle);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

		glBindBuffer(GL_ARRAY_BUFFER, m->buffers[1].handle);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);



		g_w2_state->pyramid_mesh = m;
		glBindVertexArray(0);
	}

	// CUBE MESH
	{
		VertexIndexArrayPair cube	= ogl_make_cube(g_w2_state->arena);
		OGL_Mesh3D *m = ogl_mesh3d_alloc(g_w2_state->arena, attribs);
		m->vtx_idx_pair = cube;
		glBindVertexArray(m->vao);

		m->buffers[0] = ogl_buffer_alloc(GL_ARRAY_BUFFER, sizeof(Vec3F32)*cube.vertices.count, cube.vertices.v, GL_STATIC_DRAW);
		m->buffers[1] = ogl_buffer_alloc(GL_ARRAY_BUFFER, sizeof(g_cube_colors), g_cube_colors, GL_STATIC_DRAW);
		m->element_buffer = ogl_buffer_alloc(GL_ELEMENT_ARRAY_BUFFER, sizeof(S32)*cube.indices.count, cube.indices.v, GL_STATIC_DRAW); 

		glBindBuffer(GL_ARRAY_BUFFER, m->buffers[0].handle);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

		glBindBuffer(GL_ARRAY_BUFFER, m->buffers[1].handle);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

		g_w2_state->cube_mesh = m;
		glBindVertexArray(0);
	}

	// ICOSPHERE MESH
	{
		VertexIndexArrayPair sphere	= ogl_make_icosphere(g_w2_state->arena, 0);
		OGL_Mesh3D *m = ogl_mesh3d_alloc(g_w2_state->arena, attribs);
		m->vtx_idx_pair = sphere;
		glBindVertexArray(m->vao);

		m->buffers[0] = ogl_buffer_alloc(GL_ARRAY_BUFFER, sizeof(Vec3F32)*sphere.vertices.count, sphere.vertices.v, GL_STATIC_DRAW);
		m->buffers[1] = ogl_buffer_alloc(GL_ARRAY_BUFFER, sizeof(Vec3F32)*sphere.vertices.count, sphere.vertices.v, GL_STATIC_DRAW);
		m->element_buffer = ogl_buffer_alloc(GL_ELEMENT_ARRAY_BUFFER, sizeof(S32)*sphere.indices.count, sphere.indices.v, GL_STATIC_DRAW); 

		glBindBuffer(GL_ARRAY_BUFFER, m->buffers[0].handle);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

		glBindBuffer(GL_ARRAY_BUFFER, m->buffers[1].handle);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

		g_w2_state->icosphere_mesh = m;
		glBindVertexArray(0);
	}

	// UVSPHERE MESH
	{
		VertexIndexArrayPair sphere = ogl_make_uvsphere(g_w2_state->arena, 16, 8);
		OGL_Mesh3D *m = ogl_mesh3d_alloc(g_w2_state->arena, attribs);
		m->vtx_idx_pair = sphere;
		glBindVertexArray(m->vao);

		m->buffers[0] = ogl_buffer_alloc(GL_ARRAY_BUFFER, sizeof(Vec3F32)*sphere.vertices.count, sphere.vertices.v, GL_STATIC_DRAW);
		m->buffers[1] = ogl_buffer_alloc(GL_ARRAY_BUFFER, sizeof(Vec3F32)*sphere.vertices.count, sphere.vertices.v, GL_STATIC_DRAW);
		m->element_buffer = ogl_buffer_alloc(GL_ELEMENT_ARRAY_BUFFER, sizeof(S32)*sphere.indices.count, sphere.indices.v, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, m->buffers[0].handle);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

		glBindBuffer(GL_ARRAY_BUFFER, m->buffers[1].handle);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

		g_w2_state->uvsphere_mesh = m;
		glBindVertexArray(0);
	}




	Entity cubes[2];
	Entity pyramids[2];
	Entity spheres[2];

	cubes[0].position = V3F32(-1.5f, 0.0f, 0.0f);
	cubes[0].scale		= V3F32(0.25f, 0.25f, 0.25f);
	cubes[0].rotation = 0.0f;

	cubes[1].position = V3F32(+1.5f, 0.0f, 0.0f);
	cubes[1].scale		= V3F32(0.25f, 0.25f, 0.25f);
	cubes[1].rotation = 0.0f;

	spheres[0].position = V3F32(-0.5f, 0.0f, 0.0f);
	spheres[0].scale 	  = V3F32(0.25f, 0.25f, 0.25f);
	spheres[0].rotation = 0.0f;

	spheres[1].position = V3F32(+0.5f, 0.0f, 0.0f);
	spheres[1].scale 	  = V3F32(0.25f, 0.25f, 0.25f);
	spheres[1].rotation = 0.0f;

	pyramids[0].position = V3F32(-3.0f, 0.0f, 0.0f);
	pyramids[0].scale 	 = V3F32(0.25f, 0.25f, 0.25f);
	pyramids[0].rotation = 0.0f;

	pyramids[1].position = V3F32(+3.0f, 0.0f, 0.0f);
	pyramids[1].scale 	 = V3F32(0.25f, 0.25f, 0.25f);
	pyramids[1].rotation = 0.0f;


	F32 turns = 0.0f; 					// for cubes
	F32 pos_offset = 0.0f;			// for sphere
	F32 scale_value = 0.5f; 		// for pyramid

	F32 rotation_speed = 0.1f;

	Vec3F32 position			= V3F32(0.0f, 0.0f, 0.0f);
	Vec3F32 rotation_axis = V3F32(0.0f, 1.0f, 0.0f);
	Vec3F32 scale					= V3F32(0.5f, 0.5f, 0.5f);


	F32 yaw = 0.0f;
	F32 pitch = 0.0f;
	F32 model_rotation_speed = 0.5f;


	F32 cube_move_speed = 1.0f;
	F32 camera_move_speed = 1.0f;

	U64 last_time = now_time_us();
	U64 now_time = now_time_us();

	for (;!quit;)
	{
		now_time = now_time_us();
		F64 dt = (F64)(now_time - last_time) / Million(1);
		last_time = now_time;

		// camera
		if (key_a_is_down) 			{ g_w2_state->debug_camera_pos.x -= camera_move_speed * dt; g_w2_state->debug_camera_target.x -= camera_move_speed * dt; }
		if (key_d_is_down) 			{ g_w2_state->debug_camera_pos.x += camera_move_speed * dt; g_w2_state->debug_camera_target.x += camera_move_speed * dt; }

		if (key_w_is_down) 			{ g_w2_state->debug_camera_pos.z += camera_move_speed * dt; g_w2_state->debug_camera_target.z += camera_move_speed * dt; }
		if (key_s_is_down) 			{ g_w2_state->debug_camera_pos.z -= camera_move_speed * dt; g_w2_state->debug_camera_target.z -= camera_move_speed * dt; }

		if (space_is_down) 			{ g_w2_state->debug_camera_pos.y += camera_move_speed * dt; g_w2_state->debug_camera_target.y += camera_move_speed * dt; }
		if (key_shift_is_down)	{ g_w2_state->debug_camera_pos.y -= camera_move_speed * dt; g_w2_state->debug_camera_target.y -= camera_move_speed * dt; }

		if (key_left_is_down)		{ yaw += model_rotation_speed * dt; }
		if (key_right_is_down)	{ yaw -= model_rotation_speed * dt; }
		if (key_up_is_down)			{ pitch -= model_rotation_speed * dt; }
		if (key_down_is_down)		{ pitch += model_rotation_speed * dt; }



		glClearColor(0.05f, 0.05f, 0.05f, 0.5f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// Draw
		switch(demo_index)
		{
			case 0: { ogl_draw_pyramid(position, rotation_axis, scale, yaw); 		} break;
			case 1: { ogl_draw_cube(position, rotation_axis, scale, yaw); 	 		} break;
			case 2: { ogl_draw_icosphere(position, rotation_axis, scale, yaw); 	} break;
			case 3: { ogl_draw_uvsphere(position, rotation_axis, scale, yaw); 	} break;
			case 4:
			{
				ogl_draw_cube(cubes[0].position, rotation_axis, cubes[0].scale, cubes[0].rotation);
				ogl_draw_cube(cubes[1].position, rotation_axis, cubes[1].scale, cubes[1].rotation);


				ogl_draw_uvsphere(spheres[0].position, rotation_axis, spheres[0].scale, spheres[0].rotation);
				ogl_draw_uvsphere(spheres[1].position, rotation_axis, spheres[1].scale, spheres[1].rotation);


				ogl_draw_pyramid(pyramids[0].position, rotation_axis, pyramids[0].scale, pyramids[0].rotation);
				ogl_draw_pyramid(pyramids[1].position, rotation_axis, pyramids[1].scale, pyramids[1].rotation);
			} break;

			default:{}break;
		}


		turns += rotation_speed * dt;
		turns = ModF32(turns, 1.0f);

		pos_offset = CosF32(RadFromTurnsF32(turns));

		cubes[0].rotation = RadFromTurnsF32(turns);
		cubes[1].rotation = RadFromTurnsF32(turns);

		spheres[0].position.y = +pos_offset;
		spheres[1].position.y = -pos_offset;

		scale_value = CosF32(RadFromTurnsF32(turns)) * 0.125f + 0.5f;

		pyramids[0].scale = V3F32(scale_value, scale_value, scale_value);
		pyramids[1].scale = V3F32(scale_value, scale_value, scale_value);
		

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}
