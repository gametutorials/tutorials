// Done by TheTutor

/*	
	This tutorial is an intro to the wacky world of 3D programming using D3D (Direct 3D).
	D3D is Microsoft's API that allows programmers to make sweet looking 3D graphics.

	Before we jump into the tutorial, if you have problems compiling ANY of the Direct3D
	tutorials, make sure you have downloaded and installed the Direct3D SDK.  You
	can read about how to do just that in the FAQ section of the GameTutorials CD or 
	at http://www.gametutorials.com/beginnerfaq.htm
	
	For an introductory tutorial the sheer number of files may seem a bit intimidating,
	but after reading through it, most should agree that breaking things up this may
	makes everything a bit easier to swallow.  So where do we begin?  3D is a HUGE 
	topic where performance is still being enhanced through newer hardware and 
	slicker/faster algorithms.  I mean just think about the games you played 5 years ago
	and look at what you are playing today.  It's simply amazing the amount of growth
	that has happened in 3D programming.  So in the utmost attempt to try and not bite off
	more than we can chew and start with the most basic concepts dealing with 3D 
	programming using D3D, this tutorial will do one simple task:  Draw a white triangle to
	the screen.
	
	To accomplish the task at hand there's a few things we will have to do:

		  1)  Go over and define some common 3D lingo
		  2)  Initialize the needed interfaces to have a D3D app
		  3)  Draw the sweet white triangle to the screen

	Okay still with me?  Good %)  So without any further ado we jump into our "Vocab" section
	and define some terms you will undoubtedly see when dealing with the 3rd dimension.

*/

/*	VOCAB 1.01
	
	Vertex -- A vertex is a special point of a mathematical object, and is usually a 
			  location where two or more lines or edges meet.  At least that's roughly the 
			  mathematical definition.  In 3D programming this definition holds up pretty well
			  expect that quite often additional data (such as a color, texture coordinates, 
			  bone weights, etc) are added to each vertex.

	Primitive -- A base geometric shape to render.  Examples are a point, a line, or a triangle

	Vertex Buffer --  This is basically the same concept as a "character buffer".  So just
					  like char buff[128]; is a buffer of 128 char's to fill with a string,
					  a vertex buffer is an array of vertices for filling with primitives
					  (a.k.a polygons)

	Scene -- Ever look through a gaming magazine and see a still picture of a game? (ahhh, of
			 course you have :)  That still picture could be thought of as one "scene" in
			 a game.  A scene occurs every time you begin rendering to a target (typically 
			 your screen) and then conclude rendering to that target.  In a game you
			 might create a "scene" like this:  You begin the scene.  You then
			 render the sky/backdrop.  Next you render the surrounding environment).  Then
			 it's off to render the characters in the set and perhaps some objects as well.
			 Last but not least you render the visible particle effects. You then end the scene.
			 The picture (the scene) that just got created by rendering several different
			 things is then blasted to your TV/monitor and that is what the player sees.

	Viewport -- The viewport is the RECT (rectangular area) of where we are projecting our
				3D scene onto to.  Normally this is the window that makes up the application.
				
	FVF (Flexible Vertex Format) -- D3D allows us to specify what we want in our vertex(s).
									This permits us to be flexible in what data we store
									with certain vertices.  We'll talk more about this in
									vertex_types.h %)

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

const char kClassName[] = "GT_D3DIntro"; // WNDCLASSEX's name

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

	// Just like a guy wants his girl to be certain dimensions, the same is true
	// with our window.  When setting up our window, we want to MAKE SURE that the
	// client rect is exactly kWinWid x kWinHgt.  However, if we just simply 
	// specify our window width/height to be that, our client rect will be
	// smaller than that.  Thus we adjust our window rect (so it's bigger) allowing
	// for the client rect to be the exact size we specify.  Coolio?
	RECT rect = { 0, 0, kWinWid, kWinHgt }; // Desired window client rect
	
	DWORD winStyleEx = WS_EX_CLIENTEDGE; // This is our extended window style
	DWORD winStyle = WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX; // Our window style

	// This function modifies "rect" so it contains the exact dimensions we 
	// need to create a window that has a client rect of kWinWid x kWinHgt
	AdjustWindowRectEx(&rect, winStyle, false, winStyleEx);

	// Create the window
    hwnd = CreateWindowEx(winStyleEx,
					      kClassName,
						  "www.GameTutorials.com -- D3D Intro",
						  winStyle,
						  CW_USEDEFAULT,
						  CW_USEDEFAULT,
						  rect.right - rect.left, // Window width
						  rect.bottom - rect.top, // Window height
						  NULL,
						  NULL,
						  hinstance,
						  NULL);

	// Init the global CD3DObj
	if(g3D->init(hwnd) == false)
		return EXIT_FAILURE; // There's been an error

	// DOUBLE CHECK and make sure our client rect is what we want it to be
	GetClientRect(hwnd, &rect);
	assert(rect.right == kWinWid && rect.bottom == rect.bottom);

	// Here's where we create our triangle.  By using a little math
	// we'll center the triangle based on the width of the window.
	SVertex triangle[] = 
	{ 
		// x, y, z, w, color
		{ kWinWid / 2.0f,  100.0f, 1.0f, 1.0f, D3DCOLOR_ARGB(255, 255, 255, 255), }, 
		{ kWinWid * 3.0f / 4.0f, 350.0f, 1.0f, 1.0f, D3DCOLOR_ARGB(255, 255, 255, 255), },
		{ kWinWid / 4.0f, 350.0f, 1.0f, 1.0f, D3DCOLOR_ARGB(255, 255, 255, 255), },
	};

	ShowWindow(hwnd, ishow); // Set the window to the "ishow" flag passed in WinMain
	UpdateWindow(hwnd); // Make sure the window gets initially painted

	// While the app is running...
    while(1)
	{
		// Handle any OS messages 
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
				break;
			
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			g3D->begin(); // Begin the scene
			g3D->clearColor(D3DCOLOR_ARGB(255, 0, 0, 0)); // Clear our viewport to black

			g3D->render(triangle, 3); // Draw our triangle

			g3D->end(); // End the scene (Quit rendering and blit to the screen)
			
			Sleep(1); // Give the OS a bit of time to process other things
		}
	
	} // end of while(1)

	g3D->deinit(); // Free up the CD3DObj
	UnregisterClass(kClassName,hinstance); // Free up WNDCLASSEX
	    return EXIT_SUCCESS; // Application was a success
}

// WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch(message)
    {
		case WM_KEYDOWN:

			// If ESC key is pressed, close the app
			if(wparam == VK_ESCAPE)
				SendMessage(hwnd, WM_CLOSE, 0, 0);

			return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
				return 0;
    }

    return DefWindowProc(hwnd, message, wparam, lparam);
}

// Extra DirectXion

/*
	Did you think there'd be this much stuff you'd have to do just to get 
	a triangle to the screen?  Well probably if you've done 3D before, but if
	you haven't big shocker eh?  D3D is a very cool, very powerful API.  It's also
	pretty complicated.  You should discontinue learning this stuff if you don't
	already have a good grasp of C/C++ and Win32 programming.  I say this not to 
	discourage you, but to prevent you from hours and hours of frustration and hair pulling.
	But do come back when you've brushed up on those topics %)

	Okay if you are still reading this you must be ready for the full 3D experience.  So
	I wanted to just touch on a few things before I close this tutorial out.  First,
	this tutorial IS NOT trying to be optimized in any sense.  This tutorial's 
	sole purpose is to explain the API's behind D3D and in doing so, draw a white triangle
	to the screen.  That's it.  Second you probably saw a lot of checking the "HRESULT" from
	the D3D API calls.  What is that all about?

	Each D3D interface call (example IDirect3D9::CreateDevice()) returns an HRESULT 
	which tells the success/failure of the call.  If the function returns D3D_OK that
	means the function succeed.  Any other return value means there was an error of some
	kind.  In addition to checking the error code like this tutorial does, there is
	another popular way for checking the error code.  The other error checking method
	uses the FAILED() and/or SUCCEEDED() macro.  So an example code snippet might look like:

		if(FAILED(mDevice->CreateDevice( ***all the parameters*** )))
			return false;

				OR

		if(SUCCEEDED(mDevice->CreateDevice( ***all the parameters*** )))
			return true;
		else
			return false;

	As with all GT tutorials, if you need more direction head to www.gametutorials.com 
	and jump on the forums OR you can head to MSDN (http://msdn.microsoft.com/) and check out
	their wealth of knowledge on this beast know as D3D with DX9.0

*/
	
/*-------------------------*\
|  Programmed by:  TheTutor |
|  ©2000-2006 GameTutorials |
\*-------------------------*/




