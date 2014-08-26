// Done by TheTutor

/*	
	This tutorial is all about doing transparency in D3D.  Before we get too far 
	ahead of ourselves lets do a quick recap of what transparency is all about.
	
	Transparency utilizes the alpha component (the 'A' in an RGBA color) to 
	determine the opaqueness (solidness) of an object.  An alpha value of 
	255 means fully opaque (solid color).  An alpha value of 0 means fully transparent
	(invisible or completely see through) and any value in-between is going to give you 
	a translucent (semi see through) look.
				
	Okay so we've unfrozen our brains and are beginning to remember what transparency is all 
	about.  What this application is going to do is render a spinning cube with transparency
	and have it fade in and out to a red background.
	
	There are two main areas in the code where we handle the transparency stuff.  In 
	the CD3DObj::init() function, we set the needed D3D flags.  In the UpdateAlpha() function,
	we increment our alpha value and update all of vertices colors to this alpha value 
	
	And with that... it's code time!
*/

#include <windows.h>
#include "d3d_obj.h"

// Add the following libraries
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

/////////////
// Macros //
///////////

#define DEG2RAD(x) (x * (D3DX_PI / 180.0f)) // Converts degrees to radians
#define RAD2DEG(x) (x * (180.0f / D3DX_PI)) // Converts radians to degrees

////////////////
// Constants //
//////////////

// Width and height of the window (specifically the client rect)
const int kWinWid = 800;
const int kWinHgt = 600;

const char kClassName[] = "GT_D3DTransparency"; // WNDCLASSEX's name

const int kCubeVertCount = 8; // Number of vertices in the cube
const int kCubeIndiceCount = 36; // Number of indices to define the cube as triangles

//////////////
// Globals //
////////////

// Our 8 cube vertices, half blue and half yellow
SVertex gCubeVerts[kCubeVertCount] =
{
	{ -1.0f, -1.0f, -1.0f, D3DCOLOR_RGBA(0, 0, 200, 255), }, 
	{ -1.0f, 1.0f, -1.0f, D3DCOLOR_RGBA(0, 0, 200, 255), }, 
	{ 1.0f, -1.0f, -1.0f, D3DCOLOR_RGBA(0, 0, 200, 255), }, 
	{ 1.0f, 1.0f, -1.0f, D3DCOLOR_RGBA(0, 0, 200, 255), }, 
	{ 1.0f, -1.0f, 1.0f, D3DCOLOR_RGBA(200, 200, 0, 255), }, 
	{ 1.0f, 1.0f, 1.0f, D3DCOLOR_RGBA(200, 200, 0, 255), },
	{ -1.0f, -1.0f, 1.0f, D3DCOLOR_RGBA(200, 200, 0, 255), }, 
	{ -1.0f, 1.0f, 1.0f, D3DCOLOR_RGBA(200, 200, 0, 255), }
};

// Our cube indices
WORD gCubeIndices[kCubeIndiceCount] = { 0, 1, 2, // Triangle 1
										2, 1, 3, // Triangle 2
										2, 3, 4, // Triangle 3
										4, 3, 5, // Triangle 4
										4, 5, 6, // Triangle 5
										6, 5, 7, // Triangle 6
										6, 7, 0, // Triangle 7
										0, 7, 1, // Triangle 8
										1, 7, 3, // Triangle 9
										3, 7, 5, // Triangle 10
										0, 2, 6, // Triangle 11
										2, 4, 6 }; // Triangle 12

bool LockFrameRate(int frame_rate = 60); // Locks the frame rate at "frame_rate"
void DrawAndRotateCube(); // Three guesses what this does and the first two don't count :)
void UpdateAlpha(); // Updates the alpha component of the cube's vertices

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

	RECT rect = { 0, 0, kWinWid, kWinHgt }; // Desired window client rect
	
	DWORD winStyleEx = WS_EX_CLIENTEDGE;
	DWORD winStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME |
					 WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

	// Adjust window rect so it gives us our desired client rect when we 
	// create the window
	AdjustWindowRectEx(&rect, winStyle, false, winStyleEx);

	// Create the window
    hwnd = CreateWindowEx(winStyleEx, // Window extended style
					      kClassName,
						  "www.GameTutorials.com -- Transparency",
						  winStyle, // Window style
						  CW_USEDEFAULT,
						  CW_USEDEFAULT,
						  rect.right - rect.left,
						  rect.bottom - rect.top,
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

	// Set the perspective matrix once since it will never change
	g3D->setProjMatrix(DEG2RAD(60), (float)rect.right / (float)rect.bottom, 1.0f, 8192.0f);
	
	// Set our view matrix once since it will never change
	g3D->setViewMatrix(CPos(0.0f, 0.0f, -5.0f), CPos(0.0f, 0.0f, 0.0f));

	ShowWindow(hwnd, ishow);
	UpdateWindow(hwnd);

	// While the app is running...
    while(1)
	{
		// Handle messages from the OS
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
				break;
			
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else if(LockFrameRate()) // If it's time to render, do so
		{
			DrawAndRotateCube();
			UpdateAlpha();
		}
		else
			Sleep(1); // Otherwise, give the OS a bit of time to do other things
	
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
		case WM_SYSKEYDOWN:

			// Toggle on ALT + ENTER				
			if(wparam == VK_RETURN && (lparam & (1 << 29)))
			{
				g3D->toggleFullScreen();
				
				// We need to reset our view
				g3D->setViewMatrix(CPos(0.0f, 0.0f, -5.0f), CPos(0.0f, 0.0f, 0.0f));
			}
				
			break; // Allow other system keys to be handled by DefWindowProc()
    
		case WM_KEYDOWN:
			
			// If the user presses ESC, close the app
			if(wparam == VK_ESCAPE)
				SendMessage(hwnd, WM_CLOSE, 0, 0);
				
			return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
				return 0;
    }

    return DefWindowProc(hwnd, message, wparam, lparam);
}

// Locks the frame rate at "frame_rate"
// Returns true when it's okay to draw, false otherwise
bool LockFrameRate(int frame_rate)
{
	static float lastTime = 0.0f;

	// Get current time in seconds (milliseconds * .001 = seconds)
	float currentTime = GetTickCount() * 0.001f; 

	// Get the elapsed time by subtracting the current time from the last time
	// If the desired frame rate amount of seconds has passed -- return true (ie Blit())
	if((currentTime - lastTime) > (1.0f / frame_rate))
	{
		// Reset the last time
		lastTime = currentTime;	
		return true;
	}

	return false;
}

// Draws and rotates our cube
void DrawAndRotateCube()
{
	const float kAngleInc = DEG2RAD(2); // Add 2° each update
	static float angle = 0; // The angle of rotation
	D3DXMATRIXA16 matrix;

	g3D->begin(); // Begin the scene
	g3D->clear(D3DCOLOR_XRGB(200, 0, 0)); // Clear the viewport
	
	// Rotate the cube
	D3DXMatrixRotationYawPitchRoll(&matrix, angle, angle, angle);
	g3D->setWorldMatrix(&matrix);

	// Render the cube
	g3D->render(gCubeVerts, kCubeVertCount, gCubeIndices, kCubeIndiceCount);

	g3D->end(); // End the scene
	
	angle += kAngleInc; // Update the angle for next render
}

// Updates the alpha component of global vertices
void UpdateAlpha()
{
	static int alpha = 255; // The current alpha value of each vertex in our cube
	static int alphaInc = 1; // What we currently increment our alpha value by
		
	// Loop through all of the vertices and set their alpha value to 
	// the value of "alpha"
	for(int i = 0; i < kCubeVertCount; ++i)
	{
		// Now because D3D stores it's colors in a ARGB format and they
		// store them in a DWORD which is 32-bits, we know the first 8-bits
		// contain the alpha value.  So first we binary AND with 0x00FFFFFF 
		// which will zero out the first 8-bits of our color.  Then we
		// shift our alpha over 24-bits and binary OR it with the color which 
		// will set the alpha bits in our color to our alpha value.	
		gCubeVerts[i].color &= 0x00FFFFFF;
		gCubeVerts[i].color |= (alpha << 24);
	}
		
	// Update the alpha component
	alpha += alphaInc;
	
	// We have to make sure our alpha doesn't go out of the range of 0 = 255
	if(alpha > 255)
	{
		// If it's greater than 255, cap it and then start counting back
		// to zero
		alpha = 255;
		alphaInc = -alphaInc;
	}
	else if(alpha < 0)
	{
		// If it's less than zero, cap it at zero and then start counting up
		// to 255
		alpha = 0;
		alphaInc = -alphaInc;
	}
}

// DirectXion

/*
	This tutorial was a mere introduction to blending in D3D.  There are several
	other blend mode you can set for both source blending and dest blending.
	
	If you have a transparent look on your face after reading this, do not fret!  Source/Dest
	blending is one of those concepts that for whatever reason, takes a few times doing
	on your own to really sink in.  Which brings up the challenge!
	
	*** CHALLENGE ***
	Try to change the source blend flag, dest blend flag, or both and still achieve
	the exact same fading in/out effect.
	
	As always, if you have any questions be sure to post 'em at www.GameTutorials.com
	
*/
	
/*-------------------------*\
|  Programmed by:  TheTutor |
|  ©2000-2006 GameTutorials |
\*-------------------------*/


