// Done by TheTutor

/*	
	This tutorial is gonna introduce the concept of a "camera" in a 3D app.  So before we 
	get started it's important to define what a "3D camera" is.  Well a 3D camera is similar 
	to a home video camera.  Wherever you point the camera is what is going to be captured on 
	film to be viewed at a later date and time.  Well in a 3D application, the camera sets up
	what is viewable every frame.  A 3D camera typically has some general data associated with it,
	such as:
	
		Eye position -- Where the camera is located in "world space"
		Target position -- What the camera is looking at in "world space"
		Up vector -- Defines which direction is up for the world
		
	This is the basic, bare bones data you'll need to do a simple camera system.  Certainly 
	a zoom factor (for a zoom lens), or the field of view, could also be stored with your
	camera.
	
	What we're going to do in this tutorial is create a simple camera class that allows to
	"walk" forward, backward, right, and left.  Easy enough right?
	
	One last thing before we get started... It's important to spec out what coordinate 
	system we'll be using.  For this tutorial we'll be using a left handed coordinate system which
	looks like the following:   
			
	
	+Y   +Z
	 |   /
	 |  /
	 | /
	 + ------ +X

	Be sure to check out camera.h to see how all the magic happens.  And with that, lets
	get rolling...
*/


#include <windows.h>
#include "d3d_obj.h"
#include "vector.h"

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

const char kClassName[] = "GT_D3DCamera"; // WNDCLASSEX's name

// Amount to move camera via keyboard input
// We're locking the frame rate to 60 frames a second, so every second we 
// we want to move no more than 10 units
const float kMoveAmt = 10.0f * (1.0f / 60.0f);

// Max number of verts in our global vertex pool
const int kMaxVerts = 512;

//////////////
// Globals //
//////////// 

// Global pool of vertices to be used for rendering objects
SVertex gVertPool[kMaxVerts] = {0};

bool LockFrameRate(int frame_rate = 60); // Locks the frame rate

// Draws a half red, half blue cube to the screen, centered at "cen" with "size" dimensions
void DrawCube(const CPos &cen, int size);

// Draws a square grid in the XZ plane
// Grid is centered around "cen", contains 4 raised to "numGrids" squares, and uses "color" as it's color
void DrawGrid(const CPos &cen, int numGrids, int color);

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

    RegisterClassEx(&wndclassex); // Register WNDCLASSEX

	RECT rect = { 0, 0, kWinWid, kWinHgt }; // Desired window client rect
	
	DWORD winStyleEx = WS_EX_APPWINDOW;
	DWORD winStyle = WS_CAPTION | WS_SYSMENU; // Window style

	// Adjust window rect so it gives us our desired client rect when we 
	// create the window
	AdjustWindowRectEx(&rect, winStyle, false, winStyleEx);

	// Create window
    hwnd = CreateWindowEx(winStyleEx, // Window extended style
					      kClassName,
						  "www.GameTutorials.com -- D3D Camera",
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

	// We set up our projection matrix once because it will never change
	g3D->setProjMatrix(DEG2RAD(60), (float)rect.right / (float)rect.bottom, 1.0f, 8192.0f);

	ShowWindow(hwnd, ishow);
	UpdateWindow(hwnd);

	// While the app is running...
    while(1)
	{
		// Get any messages from the OS
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
				break;
			
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else if(LockFrameRate()) // If it's time to draw
		{
			// Using our camera will set up the view of our 3D world
			g3D->setViewMatrix(gCamera);
			
			g3D->begin(); // Begin drawing
			g3D->clear();
			
			DrawCube(CPos(0,0,0), 1);
			DrawGrid(CPos(0,-2,0), 32, D3DCOLOR_ARGB(255, 0, 200, 0));
						
			g3D->end(); // Finish drawing
		}
		else
			Sleep(1); // Give OS a little time
	
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
				g3D->toggleFullScreen();
				
			break; // Allow other system keys to be handled by DefWindowProc()
    
		case WM_KEYDOWN: // If we get a key down message, do stuff

			switch(wparam)
			{
				case VK_ESCAPE: // If they push ESC, close the app
					SendMessage(hwnd, WM_CLOSE, 0, 0);
						break;

				case VK_UP: // If they push up, move forward (Camera's +Z)
					gCamera->move(CCamera::eForward, kMoveAmt);
						break;

				case VK_DOWN: // If they push down, move backward (Camera's -Z)
					gCamera->move(CCamera::eBack, kMoveAmt);
						break;

				case VK_RIGHT: // If they push right, move right (Camera's +X)
					gCamera->move(CCamera::eRight, kMoveAmt);
						break;

				case VK_LEFT: // If they push left, move left (Camera's -X)
					gCamera->move(CCamera::eLeft, kMoveAmt);
						break;
						
				case 'C': case 'c':
					gCamera->reset(); // Recenter the camera
			}
			
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

// Draws a red/blue cube to the screen, centered at "cen" with "size" dimensions
void DrawCube(const CPos &cen, int size)
{
	// This handy dandy little algorithm will construct the
	// 8 verts we need for a cube.
	for(int i = 0; i < 8; ++i)
	{
		if(i & 1)
			gVertPool[i].x = cen.x - size;
		else
			gVertPool[i].x = cen.x + size;
			
		if(i & 2)
			gVertPool[i].y = cen.y - size;
		else
			gVertPool[i].y = cen.y + size;
			
		if(i & 4)
		{
			gVertPool[i].z = cen.z + size;
			gVertPool[i].color = D3DCOLOR_ARGB(255, 200, 0, 0); // Red is the cube's backside color
		}	
		else
		{
			gVertPool[i].z = cen.z - size;
			gVertPool[i].color = D3DCOLOR_ARGB(255, 0, 0, 200); // Red is the cube's front side color
		}		
	}
	
	// Indexes for our cube
	WORD indexList[36] = { 0, 2, 3,
						   3, 1, 0,
						   7, 6, 4,
						   4, 5, 7,
						   4, 6, 2, 
						   2, 0, 4,
						   1, 3, 7,
						   7, 5, 1,
						   4, 0, 1,
						   1, 5, 4,
						   2, 6, 7,
						   7, 3, 2 };
	
	g3D->render(gVertPool, 8, indexList, 36); // Draw the cube
}

// Draws a grid to the centered around "cen" to the screen
void DrawGrid(const CPos &cen, int numGrids, int color)
{
	assert(numGrids > 0);

	int i = 0, j = 0;
	
	// Loop through and draw 4 ^ numGrids, 1x1 grid squares 
	for(i = -numGrids; i <= numGrids; ++i)
	{
		gVertPool[j].color = color;
		gVertPool[j].x = cen.x - numGrids;
		gVertPool[j].y = cen.y; 
		gVertPool[j].z = cen.z + i;
		++j;
		
		gVertPool[j].color = color;
		gVertPool[j].x = cen.x + numGrids;
		gVertPool[j].y = cen.y; 
		gVertPool[j].z = cen.z + i;
		++j;
		
		gVertPool[j].color = color;
		gVertPool[j].x = cen.x + i;
		gVertPool[j].y = cen.y; 
		gVertPool[j].z = cen.z - numGrids;
		++j;
		
		gVertPool[j].color = color;
		gVertPool[j].x = cen.x + i;
		gVertPool[j].y = cen.y; 
		gVertPool[j].z = cen.z + numGrids;
		++j;
	}
	
	// Draw the entire grid using lines
	g3D->renderLine(gVertPool, j);
}

// Lights, Camera, Action...

/*
	Hopefully you've learned a little bit about cameras.  The implementation of 
	maintaining the three camera axes is a fairly common implementation of a 
	camera class.  However, the short comings of this tutorial should be pointed 
	out.  For starters, when you move in a direction, you can't specify how much you
	want to move AND it's doesn't use any sort of time based movement.  A great mini-exercise
	would be to modify this tutorial so you could specify how much you wanted to move
	and movement occurred in time sliced chunks.   

	As always, questions, comments, and suggestions can be posted on the forums at:
	http://www.GameTutorials.com	
*/
	

/*-------------------------*\
|  Programmed by:  TheTutor |
|  ©2000-2006 GameTutorials |
\*-------------------------*/
