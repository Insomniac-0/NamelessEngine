// stub function for now because the ordering of glfw and glew 
internal void
r_init(CmdLine *cmdline)
{
}



internal void
AppInit(S32 width, S32 height)
{
	Arena* arena = arena_alloc();

	AppState *state = push_struct(arena, AppState);

	state->arena = arena;
	state->window_x = 0;
	state->window_y = 0;
	state->window_w = width;
	state->window_h = height;

	app_state = state;
}


internal void HandleInput()
{
	SDL_Event e;
	for(;SDL_PollEvent(&e) != 0;)
	{
		switch(e.type)
		{
			case SDL_EVENT_QUIT:{quit = 1;}break;
			case SDL_EVENT_KEY_DOWN:
			case SDL_EVENT_KEY_UP:
			{
				KeyEvent(e.key.key, e.key.mod, e.key.down);
			}
			case SDL_EVENT_WINDOW_RESIZED:
			{

				WindowResizeEvent(e.window.data1, e.window.data2);
			}break;
		}
	}
}

internal void KeyEvent(SDL_Keycode key, SDL_Keymod mod, bool down)
{
	switch(key)
	{
		case SDLK_ESCAPE:
		{
			quit = 1;
		}break;
	}
}


internal void
WindowResizeEvent(S32 w, S32 h)
{
	app_state->window_w = w;
	app_state->window_h = w;

	glViewport(0, 0, w, h);
}
