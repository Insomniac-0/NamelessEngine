//~ includes : [h]
#include "base/base_inc.h"

//~ includes : [c]
#include "base/base_inc.c"

//~ includes : x11
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>




int main(void)
{
	Display* display = XOpenDisplay(NULL);
	S32 screen = DefaultScreen(display);
	Window root = RootWindow(display, screen);

	// Config
	S32 x = 100;
	S32 y = 100;
	S32 w = 400;
	S32 h = 300;

	XSetWindowAttributes attr;
	attr.event_mask = ExposureMask | KeyPressMask;
	attr.border_pixel = BlackPixel(display, screen);
	attr.background_pixel = WhitePixel(display, screen);
	

	Window test_window = XCreateWindow(
		display,		// Display
		root,				// Window
		x,		// X : S32
		y,		// Y : S32
		w,		// Width : U32
		h,		// Height : U32
		1,		// Border-Width : S32
		CopyFromParent,		// Depth : S32
		CopyFromParent,		// Class : U32
		CopyFromParent,		// Visual : Visual
		CWBackPixel | CWBorderPixel | CWEventMask, 		// valuemask : U64
		&attr		// attributes : XSetWindowAttributes
	);

	XStoreName(display, test_window, "Test Window");

	// Atoms
	Atom wm = XInternAtom(display, "_NET_WM_WINDOW_TYPE", False);
	Atom utility = XInternAtom(display, "_NET_WM_WINDOW_TYPE_UTILITY", False);

	Atom wm_protocols = XInternAtom(display, "WM_PROTOCOLS", False);
	Atom delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(display, test_window, &delete_window, 1);

	XChangeProperty(
		display,
		test_window,
		wm,
		XA_ATOM,
		32,
		PropModeReplace,
		(U8*)&utility,
		1
	);

	XMapWindow(display, test_window);

	XEvent event;
	S32 done = 0;
	while (!done)
	{
		XNextEvent(display, &event);
		switch (event.type)
		{
		case Expose: { break; }
		case KeyPress:
		{
			KeySym key = XLookupKeysym(&event.xkey, 0);
			if (key == XK_Return || key == XK_Escape)
			{
				done = 1;
			}
			break;
		}
		case ClientMessage:
		{
			if (event.xclient.data.l[0] == delete_window)
			{
				done = 1;
				break;
			}
			
		}

		}

	}

	return 0;
}
