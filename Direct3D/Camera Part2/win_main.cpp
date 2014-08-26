// Done by TheTutor

/*	
	This tutorial introduces "camera swivel" to our camera system.  What we are going to do 
	is hook up mouse movement to our camera system so that it functions like your typically FPS.
	
	We'll mimic the controls of Quake3 so that when you move the mouse up/down you swivel (pitch)
	up/down relative to your camera coordinates.  But when you move the mouse left/right you 
	swivel (yaw) relative to the world coordinate system. 
	
	The world coordinate system we'll be using is:
	
	+Y   +Z
	 |   /
	 |  /
	 | /
	 + ------ +X

	Now, let the coding begin.
*/


#include <windows.h>
#include <stdio.h>
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

const char kClassName[] = "GT_D3DCameraPart2"; // WNDCLASSEX's name

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
void CameraMouseInput(); // Handles camera input

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

	DWORD winStyleEx = WS_EX_CLIENTEDGE;
	DWORD winStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME |
					 WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

	// Adjust window rect so it gives us our desired client rect when we 
	// create the window
	AdjustWindowRectEx(&rect, winStyle, false, winStyleEx);

	// Create the window
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

	ShowCursor(FALSE); // Hide cursor
	ShowWindow(hwnd, ishow);
	UpdateWindow(hwnd);
	
	// Start our cursor in the middle of the screen
	SetCursorPos(GetSystemMetrics(SM_CXSCREEN) >>  1, GetSystemMetrics(SM_CYSCREEN) >> 1); 

	// While the app is running...
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
		else if(LockFrameRate()) // If it's time to draw
		{
			CameraMouseInput(); // Get mouse input for the camera
		
			// Using our camera will set up the view of our 3D world
			g3D->setViewMatrix(gCamera);
			
			g3D->begin(); // Begin drawing
			g3D->clear();
			
			DrawCube(CPos(0,0,0), 1);
			DrawGrid(CPos(0,-2,0), 32, D3DCOLOR_ARGB(255, 0, 200, 0));
						
			g3D->end(); // Finish drawing
		}
	
	} // end of while(1)

	ShowCursor(TRUE); // Reshow cursor
	
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

				case 'W':
				case VK_UP: // If they push up, move forward (Camera's +Z)
					gCamera->move(CCamera::eForward, kMoveAmt);
						break;

				case 'S':
				case VK_DOWN: // If they push down, move backward (Camera's -Z)
					gCamera->move(CCamera::eBack, kMoveAmt);
						break;

				case 'D':
				case VK_RIGHT: // If they push right, move right (Camera's +X)
					gCamera->move(CCamera::eRight, kMoveAmt);
						break;

				case 'A':
				case VK_LEFT: // If they push left, move left (Camera's -X)
					gCamera->move(CCamera::eLeft, kMoveAmt);
						break;
						
				case 'C':
					gCamera->reset(); // Recenter the camera
						break;
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

// Handles mouse input for the camera
void CameraMouseInput()
{
	const float kMaxAngle = 89.5f; // Max degree of rotation
	static float pitchAmt = 0.0f; // Amout we've looked up or down
	
	// Get the middle of the screen
	int midScrX = GetSystemMetrics(SM_CXSCREEN) >> 1;
	int midScrY = GetSystemMetrics(SM_CYSCREEN) >> 1;
	
	float amt;
	POINT pt = {0};
	
	GetCursorPos(&pt); // Get the current mouse position
		
	// Rotate left/right
	amt = float(pt.x - midScrX) * kMoveAmt;
	gCamera->rotateY(DEG2RAD(amt));
	
	// Calculate amount to rotate up/down
	amt = float(midScrY - pt.y) * kMoveAmt;

	// Clamp pitch amount
	if(pitchAmt + amt <= -kMaxAngle)
	{
		amt = -kMaxAngle - pitchAmt;
		pitchAmt = -kMaxAngle;
	}
	else if(pitchAmt + amt >= kMaxAngle)
	{
		amt = kMaxAngle - pitchAmt;
		pitchAmt = kMaxAngle;
	}
	else
	{
		pitchAmt += amt;
	}
	
	// Pitch camera
	gCamera->pitch(DEG2RAD(amt)); 	

	SetCursorPos(midScrX, midScrY); // Set our cursor back to the middle of the screen
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
			gVertPool[i].color = D3DCOLOR_ARGB(255, 200, 0, 0); // Red is the back of the cube's color
		}	
		else
		{
			gVertPool[i].z = cen.z - size;
			gVertPool[i].color = D3DCOLOR_ARGB(255, 0, 0, 200); // Blue is the front of the cube's color
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
	Finally!  You can now mimic that Quake style camera.  Just be very careful that
	you know what coordinate system you are using and if you are using matrices,
	if the matrices are row based or column based.

	As always, questions, comments, and suggestions can be posted on the forums at:
	http://www.GameTutorials.com	
*/
	

/*-------------------------*\
|  Programmed by:  TheTutor |
|  ©2000-2006 GameTutorials |
\*-------------------------*/
