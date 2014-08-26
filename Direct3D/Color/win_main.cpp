// Done by TheTutor

/*	
	The tutorial at hand is all about adding color to objects in D3D.  Because 
	we are using a FVF (Flexible Vertex Format) this turns out to be a pretty simple
	task.  Before we jump into the code we'll cover most of the different ways
	you see color represented in a D3D app.

	A color in D3D is represented by 32-bits.  8 for alpha, 8 for red, 8 for green, and
	8 for blue (in that order).  So common data types for a color in D3D are:

		int (32-bit signed integer)
		DWORD (32-bit unsigned integer)
		D3DCOLOR (typedef to be a DWORD)

	D3D provides a few cool macros for making a "diffuse color".  They are:

		D3DCOLOR_XRGB(r,g,b)
		D3DCOLOR_RGBA(r,g,b,a)
		D3DCOLOR_ARGB(a,r,g,b)

		**NOTE** In the above macros, r = red, g = green, b = blue, and a = alpha all of which
				 are assumed to be in the range of 0 - 255.  Additionally it is important to
				 note that colors in D3D are stored in ARGB order.  These macros assure that 
				 the color gets set in this order.

	 Okay so hopefully you're looking at this and thinking to yourself, "Piece of cake".  There
	 really isn't much to it, but, incase you're curious to see all that color talk in action, 
	 this tutorial does just that.  What we are going to do is a draw a triangle to screen
	 and set it's color based on the mouse's (x,y) position in the window.  It's time 
	 to get in living color...

*/


#include <windows.h>
#include "d3d_obj.h"

#pragma comment(lib, "d3d9.lib")

////////////////
// Constants //
//////////////

// Width and height of the window
const int kWinWid = 640;
const int kWinHgt = 480;

const char kClassName[] = "GT_D3DColor"; // WNDCLASSEX's name

//////////////
// Globals //
////////////

// Here's where we create our triangle.  By using a little math
// we'll center the triangle based on the width of the window.
// Notice how we use the macro to initialize the color of "triangle".
// So if you were asking yourself, "Is it legal to use the macro when declaring a variable?"
// the answer is yes %)
SVertex gTriangle[] = 
{ 
	// x, y, z, w, color
	{ kWinWid / 2.0f,  100.0f, 1.0f, 1.0f, D3DCOLOR_XRGB(255,255,255), }, 
	{ kWinWid * 3.0f / 4.0f, 350.0f, 1.0f, 1.0f, D3DCOLOR_XRGB(255,255,255), },
	{ kWinWid / 4.0f, 350.0f, 1.0f, 1.0f, D3DCOLOR_XRGB(255,255,255), },
};

void DrawTriangle(); // This draws our triangle to the screen

// The WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

// Main window program
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprev, PSTR cmdline, int ishow)
{
    HWND hwnd = NULL;
	MSG msg = {0};
    WNDCLASSEX wndclassex = {0};

	// Init fields we care about
	wndclassex.cbSize = sizeof(WNDCLASSEX); // Always set to size of WNDCLASSEX
    wndclassex.style = CS_HREDRAW | CS_VREDRAW;
    wndclassex.lpfnWndProc = WinProc;
    wndclassex.hInstance = hinstance;
	wndclassex.lpszClassName = kClassName;
	wndclassex.hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(IDC_ARROW),
											IMAGE_CURSOR, 0, 0, LR_SHARED);

    RegisterClassEx(&wndclassex); // Register the WNDCLASSEX

	// Here's our desired window's client rect
	RECT rect = { 0, 0, kWinWid, kWinHgt };
	
	DWORD winStyleEx = WS_EX_CLIENTEDGE; // Extended window style
	DWORD winStyle = WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX; // Window style

	// Adjust our window rect so our client rect will be the dimensions we want
	AdjustWindowRectEx(&rect, winStyle, false, winStyleEx);

	// Create the window
    hwnd = CreateWindowEx(winStyleEx,
					      kClassName,
						  "www.GameTutorials.com -- D3D Color",
						  winStyle, // No resizing of the window
						  CW_USEDEFAULT,
						  CW_USEDEFAULT,
						  rect.right - rect.left, // Window width
						  rect.bottom - rect.top, // Window height
						  NULL,
						  NULL,
						  hinstance,
						  NULL);

	// Init our global 3D object
	if(g3D->init(hwnd) == false)
		return EXIT_FAILURE; // There's been an error, lets get out of this joint

	// Get the client rect and make sure our client is the size we want
	GetClientRect(hwnd, &rect);
	assert(rect.right == kWinWid && rect.bottom == kWinHgt);

	ShowWindow(hwnd, ishow);
	UpdateWindow(hwnd);

	// While the app is running.., 
    while(1)
	{
		// Handle any messages from the OS
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
				break;
			
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
			Sleep(1); // Give the OS some time back
	
	} // end of while(1)

	g3D->deinit(); // Free up CD3DObj
	UnregisterClass(kClassName,hinstance); // Free up WNDCLASSEX
	    return EXIT_SUCCESS; // Application was a success
}

// WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch(message)
    {
		case WM_PAINT: // Whenever the window gets repainted, redraw the triangle
			DrawTriangle();
				return 0;

		case WM_MOUSEMOVE:
		{
			// First we'll get the (x,y) position of the cursor
			short x = (short)LOWORD(lparam); // The x position of the cursor
			short y = (short)HIWORD(lparam); // The y position of the cursor

			// Then we'll get the RECT that makes up the client area of the window
			RECT rect = {0};
			GetClientRect(hwnd, &rect); // Get the client rect of the window

			// Clamp our x and y to our client area
			if(x < rect.left) x = (short)rect.left;
			if(x > rect.right) x = (short)rect.right;
			if(y < rect.top) y = (short)rect.top;
			if(y > rect.bottom) y = (short)rect.bottom;

			// Determine the percentage of red, green and blue to use for our 
			// triangle based on the position of the cursor.  Here's how we determine
			// the color:  The farther we move the cursor to the right, the more red 
			// color we'll use.  The farther we move the cursor to the left, the more
			// blue color we'll use.  And last but not least, the farther towards the
			// bottom of the window we move our cursor, the more green we'll use.
			// So quickly to recap that looks like this:
			//		RED == 0% (left) to 100% (right)
			//		GREEN == 0% (top) to 100% (bottom)
			//		BLUE == 0% (right) to 100% (left)
			float redPercent = x / (float)rect.right;
			float bluePercent = 1.0f - x / (float)rect.right;
			float greenPercent = y / (float)rect.bottom;

			// Get the R, G, B in the value of 0 - 255
			int red = int(255.0f * redPercent);
			int green = int(255.0f * greenPercent);
			int blue = int(255.0f * bluePercent);

			// Fill in the colors using the D3D "create a color macro"
			gTriangle[0].color = D3DCOLOR_XRGB(red, green, blue);
			gTriangle[1].color = D3DCOLOR_XRGB(red, green, blue);
			gTriangle[2].color = D3DCOLOR_XRGB(red, green, blue);

			DrawTriangle(); // Draw our triangle to the screen with it's updated color
				return 0;
		}

		case WM_KEYDOWN:

			// If ESC key is pressed, quit the app
			if(wparam == VK_ESCAPE)
				SendMessage(hwnd, WM_CLOSE, 0, 0);

			return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
				return 0;
    }

    return DefWindowProc(hwnd, message, wparam, lparam);
}

// This draws our triangle to the screen
void DrawTriangle()
{
	g3D->begin(); // Begin the scene
	g3D->clearColor(D3DCOLOR_XRGB(0,0,0)); // Clear our viewport to black

	g3D->render(gTriangle, 3); // Draw our triangle

	g3D->end(); // End the scene (Quit rendering and blit to the screen)
}

// Extra DirectXion

/*
	That's about it for diffuse colors in D3D.  If you are looking for further 
	information about color in D3D head on over to the message boards at:
	http://www.GameTutorials.com
*/
	

/*-------------------------*\
|  Programmed by:  TheTutor |
|  ©2000-2006 GameTutorials |
\*-------------------------*/



