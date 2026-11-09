#ifndef WEEK_3_H
#define WEEK_3_H

#define W_WIDTH 1280
#define W_HEIGHT 960

#define FOV 45.0f
#define ASPECT W_WIDTH / W_HEIGHT
#define NEAR_Z 0.1f
#define FAR_Z 100.0f


typedef struct AppState AppState;
struct AppState
{
	Arena *arena;
	S32 window_x;
	S32 window_y;
	S32 window_w;
	S32 window_h;
};


// asset struct sketching
typedef enum AssetKind
{
	AssetKind_Texture,
	AssetKind_Model3D,
	AssetKind_Audio,

	AssetKind_COUNT
}
AssetKind;

// asset path_prefix;
// asset file_ext;


struct Asset
{
	AssetKind kind;
	String8 asset_name;
	String8 asset_file_name;
};

global AppState *app_state = 0;

// globals

global B32 quit = 0;

internal void AppInit(S32 width, S32 height);

internal void HandleInput();
internal void KeyEvent(SDL_Keycode key, SDL_Keymod mod, bool down);
internal void WindowResizeEvent(S32 w, S32 h);

/* void framebuffer_size_callback(GLFWwindow* window, int width, int height); */
/* void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods); */


#endif // WEEK_3_H
