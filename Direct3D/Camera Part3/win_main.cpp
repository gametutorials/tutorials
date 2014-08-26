// Done by TheTutor

/*	
	While a first person camera, is typically considered an easy camera system to implement, a
	third person camera (at least a good one) is typically considered difficult to implement.  This 
	is because anytime you have to deal with a 3rd person camera system, you have to worry about
	camera collisions with the geoemtry in your scene.
	
	This tutorial is gonna to simplify the creation of a 3rd person camera system by not worrying 
	about collisions, but rather focusing on how you implement the guts of a 3rd person camera system.
	So lets start by defining what a 3rd person camera system is.  It is a camera system in which 
	the camera's eye floats, typically a predefined distance, behind an object with camera rotations
	occurring around the center of the object.
	
	Any "3rd person action game" utilizes a 3rd person camera.  A classic game with a great 3rd person
	camera is Mario 64.  The N64 launch title, put the plumber in 3D action for the first time 
	with a slick and intuitive camera system.
	
	Within this tutorial we are going to construct a 3rd person camera system that is typical of
	a "3rd person action game".  When we move the object forward/back/left/right, the camera will
	follow the object.  When we pitch or swivel the camera, it will pitch/yaw around the center
	of our object.  All forward and backward movement will occur down our line of sight.  Need 
	a visual?  Well go run the app and come back to see how it all works.  Now it's time to jump
	to the code...	
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
#define XRGB(r, g, b) D3DCOLOR_XRGB(r, g, b) // We'll use a smaller macro name
											// for D3D's color macro

////////////////
// Constants //
//////////////

// Width and height of the window (specifically the client rect)
const int kWinWid = 800;
const int kWinHgt = 600;

const char kClassName[] = "GT_D3DCameraPart3"; // WNDCLASSEX's name

// Amount to move camera via keyboard input
// We're locking the frame rate to 60 frames a second, so every second we 
// we want to move no more than 10 units
const float kMoveAmt = 10.0f * (1.0f / 60.0f);

// Max number of verts in our global vertex pool
const int kMaxVerts = 512;

//////////////
// Globals //
//////////// 

SVertex gVertPool[kMaxVerts] = {0}; // Global pool of vertices to be used for rendering objects
CPos gCubePos(0,0,0); // Position of cube in the world

bool LockFrameRate(int frame_rate = 60); // Locks the frame rate
void CameraMouseInput(); // Handle the camera's mouse input

// Draws a half "color1", half "color2" cube to the screen, centered at "cen" with "size" dimensions
void DrawBox(const CPos &cen, const CVector &size, int color1, int color2);

// Draws a square grid in the XZ plane
// Grid is centered around "cen", contains 4 raised to "numGrids" squares, and uses "color" as it's color
void DrawGrid(const CPos &cen, int numGrids, int color);

// The WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

// WinMain
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
	
	DWORD winStyleEx = WS_EX_APPWINDOW;
	DWORD winStyle = WS_CAPTION | WS_SYSMENU; // Window style

	// Adjust window rect so it gives us our desired client rect when we 
	// create the window
	AdjustWindowRectEx(&rect, winStyle, false, winStyleEx);

	// Create the window
    hwnd = CreateWindowEx(winStyleEx, // Window extended style
					      kClassName,
						  "www.GameTutorials.com -- D3D 3rd Person Camera",
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

	// While the app is running..
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
		else if(LockFrameRate()) // If it is time to draw
		{
			CameraMouseInput();
			g3D->setViewMatrix(gCamera); // Set the view matrix
			
			g3D->begin(); // Begin drawing
			g3D->clear();
			
			// The dimensions of our four box towers in the scene
			const CVector kTowerSize(1.0f, 3.0f, 1.0f);
			
			// Draw the four towers
			DrawBox(CPos(5.0f,0.0f,0.0f), kTowerSize, XRGB(255, 0, 0), XRGB(0, 0, 255));
			DrawBox(CPos(-5.0f,0.0f,0.0f), kTowerSize, XRGB(255, 0, 0), XRGB(0, 0, 255));
			DrawBox(CPos(0.0f,0.0f,5.0f), kTowerSize, XRGB(255, 0, 0), XRGB(0, 0, 255));
			DrawBox(CPos(0.0f,0.0f,-5.0f), kTowerSize, XRGB(255, 0, 0), XRGB(0, 0, 255));
			
			// Draw the cube "player object"
			DrawBox(gCubePos, CVector(0.5f,0.5f,0.5f), XRGB(255, 0, 255), XRGB(0, 255, 0));
			DrawGrid(CPos(0,-2,0), 32, XRGB(0, 200, 0));
						
			g3D->end(); // Finish drawing
		}
		else
			Sleep(1); // Give OS a bit of time to process other things
	
	} // end of while(1)

	ShowCursor(TRUE); // Reshow cursor
	
	g3D->deinit();
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
		{
			CVector vec; // Used to hold camera's forward vector
			CPos eye; // Used to hold camera's eye
		
			switch(wparam)
			{
				case VK_ESCAPE: // If they push ESC, close the app
					SendMessage(hwnd, WM_CLOSE, 0, 0);
						break;

				case 'W':
				case VK_UP: // If they push up, move forward (Camera's +Z)
					vec = gCamera->getCamForward();
					eye = gCamera->getEye();
					
					// Move the camera's eye and cube in the direction of
					// the camera's forward vector
					eye += vec * kMoveAmt;
					gCubePos += vec * kMoveAmt;
					
					gCamera->setEye(eye);
					gCamera->setTarget(gCubePos); // Set the camera to look at the cube
						break;

				case 'S':
				case VK_DOWN: // If they push down, move backward (Camera's -Z)
					vec = gCamera->getCamForward();
					eye = gCamera->getEye();
					
					// Move the camera's eye and cube opposite the direction of
					// the camera's forward vector
					eye -= vec * kMoveAmt;
					gCubePos -= vec * kMoveAmt;
					
					gCamera->setEye(eye);
					gCamera->setTarget(gCubePos); // Set the camera to look at the cube
						break;

				case 'D':
				case VK_RIGHT: // If they push right, move right (Camera's +X)
					vec = gCamera->getCamRight();
					eye = gCamera->getEye();
					
					// Move the camera's eye and cube in the direction of
					// the camera's right vector (strafe right)
					eye += vec * kMoveAmt;
					gCubePos += vec * kMoveAmt;
					
					gCamera->setEye(eye);
					gCamera->setTarget(gCubePos); // Set the camera to look at the cube
						break;

				case 'A':
				case VK_LEFT: // If they push left, move left (Camera's -X)
					vec = gCamera->getCamRight();
					eye = gCamera->getEye();
					
					// Move the camera's eye and cube opposite the direction of
					// the camera's right vector (strafe left)
					eye -= vec * kMoveAmt;
					gCubePos -= vec * kMoveAmt;
					
					gCamera->setEye(eye);
					gCamera->setTarget(gCubePos); // Set the camera to look at the cube
						break;
			}
			
			return 0;
		}
		
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
	// If the desired frame rate amount of seconds has passed -- return true (ie Render())
	if((currentTime - lastTime) > (1.0f / frame_rate))
	{
		// Reset the last time
		lastTime = currentTime;	
			return true;
	}

	return false;
}

// Handles camera mouse input
void CameraMouseInput() 
{
	const float kMaxAngle = 89.0f;
	static float pitchAmt = 0.0f; // Amout we've looked up or down

	// Get the middle of the screen
	int midScrX = GetSystemMetrics(SM_CXSCREEN) >> 1;
	int midScrY = GetSystemMetrics(SM_CYSCREEN) >> 1;

	float amt;
	POINT pt = {0};

	GetCursorPos(&pt); // Get the current mouse position

	// Rotate left/right
	amt = float(pt.x - midScrX) * kMoveAmt;
	gCamera->rotateY(DEG2RAD(amt), gCubePos);

	// Rotate up/down
	amt = float(midScrY - pt.y) * kMoveAmt;
	
	// Cap pitch
	if(pitchAmt + amt < -kMaxAngle)
	{
		amt = -kMaxAngle - pitchAmt;
		pitchAmt = -kMaxAngle;
	}
	else if(pitchAmt + amt > kMaxAngle)
	{
		amt = kMaxAngle - pitchAmt;
		pitchAmt = kMaxAngle;
	}
	else
	{
		pitchAmt += amt;
	}
	
	// Pitch the camera up/down
	gCamera->pitch(DEG2RAD(amt), gCubePos);

	SetCursorPos(midScrX, midScrY); // Set our cursor back to the middle of the screen
}

// Draws a two colored box to the screen, centered at "cen" with "size" dimensions
void DrawBox(const CPos &cen, const CVector &size, int color1, int color2)
{
	// This handy dandy little algorithm will construct the
	// 8 verts we need for the box
	for(int i = 0; i < 8; ++i)
	{
		if(i & 1)
			gVertPool[i].x = cen.x - size.x;
		else
			gVertPool[i].x = cen.x + size.x;
			
		if(i & 2)
			gVertPool[i].y = cen.y - size.y;
		else
			gVertPool[i].y = cen.y + size.y;
			
		if(i & 4)
		{
			gVertPool[i].z = cen.z + size.z;
			gVertPool[i].color = color1; // Back side gets 1st color
		}	
		else
		{
			gVertPool[i].z = cen.z - size.z;
			gVertPool[i].color = color2; // Front side gets 2nd color
		}		
	}
	
	// Indexes for our box
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
	
	g3D->render(gVertPool, 8, indexList, 36); // Draw the box
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

// Sometimes it's better to watch from 3rd person 

/*
	You now should have a good first step towards creating a 3rd person camera system.  There
	are a couple things to be aware about that we'll cover here:
	
		1)  Gimbol lock -- If our camera's forward vector is equal to the world up's vector
						   this will cause some nasty stuff to happen.  This is why we
						   clamp how much we allow the camera to pitch.
		2)  Coordinate systems.  If you use a different coordinate system, the math probably
			won't work as is.
		3)  Matrix math.  We don't explictly use a matrix in the tutorial, but if you're using
			a preexisting matrix class, be sure to be careful of row vs column implementations.
			
	Questions and comments should be directed to the forums at:  www.GameTutorials.com
*/

/*-------------------------*\
|  Programmed by:  TheTutor |
|  ©2000-2006 GameTutorials |
\*-------------------------*/


