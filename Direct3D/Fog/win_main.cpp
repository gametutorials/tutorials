// Done by TheTutor

/*	
	Fog is typically used for two reasons in a app.  The first is pretty obvious, the look of 
	the scene calls for fog %)  The second reason is that by using fog an application can speed
	up it's rendering time by fogging out geometry, thus not considering it for rendering.
	
	Within, we are going to take a beginners peek at fog in a D3D 9.0 application.  First, lets
	break down the parameters that control fog:
	
		Color -- The color of the fog
		Type -- Either pixel or vertex
		Start/Stop -- For fog with a linear falloff, this specifies the beginning of the fog
					  and the end of the fog
		Density -- For fog with an exponential falloff, this specifies how thick the fog is
		Falloff -- Either linear, exponential, or exponential squared
		Range -- Either using the distance from the camera to the object or the object's z-value
				 when figuring out the fog calculations
				 
	What we are going to do is write some wrapper functions that allow us to set all of these
	parameters except for range.  This is because distance based fog calculations are not supported
	on any current cards for pixel fog and in general they are more expensive than z-value based
	fog calculations.
	
	**NOTE** It is important when doing fog to remember not all forms of fog are supported on
			 every video card.  Be sure to check for failed fog function calls. 
	
	Lets get to the code...	
*/

#include <assert.h>
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

const char kClassName[] = "GT_D3DFog"; // WNDCLASSEX's name

// Light blue color of fog
const DWORD kFogColor = D3DCOLOR_ARGB(255, 32, 32, 128); 

// Amount to move camera via keyboard input
// We're locking the frame rate to 60 frames a second, so every second we 
// we want to move no more than 10 units
const float kMoveAmt = 10.0f * (1.0f / 60.0f);

// Max number of verts in our global vertex pool
const int kMaxVerts = 512;

//////////////
// Globals //
//////////// 

SVertex gVertPool[kMaxVerts] = {0}; // A giant array of vertices we can use for whatever we want

// Inits all fog parameters, returns true on success, false otherwise
bool InitFog();

bool LockFrameRate(int frame_rate = 60); // Locks the frame rate
void CameraMouseInput(); // Handles camera input

void DrawGrid(const CPos &cen, int numGrids, DWORD color); // Draws a square grid in the XZ plane
void DrawCube(const CPos &cen, float size, DWORD color); // Draws a cube centered at world pos "cen"

// WinProc
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
						  "www.GameTutorials.com -- D3D Fog",
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
	
	// If we can initialize the fog, exit the application
	if(!InitFog())
		return EXIT_FAILURE;
		
	ShowCursor(FALSE); // Hide cursor
	ShowWindow(hwnd, ishow);
	UpdateWindow(hwnd);
	
	// Slam the cursor to the middle of the screen
	SetCursorPos(GetSystemMetrics(SM_CXSCREEN) >> 1, GetSystemMetrics(SM_CYSCREEN) >> 1);
		
	// While the app is running...
    while(1)
	{
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE)) // Handle messages from the OS
		{
			if(msg.message == WM_QUIT)
				break;
			
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else if(LockFrameRate()) // Otherwise if it's time to draw
		{		
			static float angle = 0; // Angle of rotation
			D3DXMATRIX wMat; // World matrix
			
			CameraMouseInput(); // Move camera via the mouse
			
			// Set the world view to the current camera view
			g3D->setViewMatrix(gCamera);
			
			g3D->begin(); // Begin drawing
			g3D->clear(kFogColor); // Clear the screen to the fog color
			
			// Rotate around the Y-axis			
			g3D->setWorldMatrix(D3DXMatrixRotationY(&wMat, DEG2RAD(++angle)));
			
			// Draw 4 cubes
			DrawCube(CPos(2,0,0), 0.5f, D3DCOLOR_ARGB(255, 255, 0, 0));
			DrawCube(CPos(-2,0,0), 0.5f, D3DCOLOR_ARGB(255, 255, 0, 0));
			DrawCube(CPos(0,0,2), 0.5f, D3DCOLOR_ARGB(255, 255, 0, 0));
			DrawCube(CPos(0,0,-2), 0.5f, D3DCOLOR_ARGB(255, 255, 0, 0));
			
			// Draw the grid
			g3D->setWorldMatrix(D3DXMatrixIdentity(&wMat));
			DrawGrid(CPos(0,-2,0), 32, D3DCOLOR_ARGB(255, 0, 200, 0));
			
			g3D->end(); // Finish drawing
		}
		else
			Sleep(1); // Give the OS a tiny bit of time to process other things
	
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
			{
				g3D->toggleFullScreen();
				InitFog(); // Reenable fog
			}

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

// Set all fog parameters
// Returns true on success, false otherwise
bool InitFog()
{
	// Set all the parameters, returning false if any of them fail
	if(!g3D->setFogColor(kFogColor)) return false;
	if(!g3D->setPixelFogMode(D3DFOG_LINEAR)) return false;
	if(!g3D->setFogStart(1.0f)) return false;
	if(!g3D->setFogEnd(10.0f)) return false;
	
	return g3D->setFogActive(true); // Return the status of enabling the fog
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

void CameraMouseInput() // Handles camera input
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
void DrawCube(const CPos &cen, float size, DWORD color)
{
	// This handy dandy little algorithm will construct the
	// 8 verts we need for a cube.
	for(int i = 0; i < 8; ++i)
	{
		gVertPool[i].color = color;
		
		gVertPool[i].x = (i & 1) ? (cen.x - size) : (cen.x + size);
		gVertPool[i].y = (i & 2) ? (cen.y - size) : (cen.y + size);
		gVertPool[i].z = (i & 4) ? (cen.z + size) : (cen.z - size);	
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
void DrawGrid(const CPos &cen, int numGrids, DWORD color)
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

// This still foggy?

/*
	Adding fog to an application is not too difficult.  However, be aware that not all cards
	support all fog modes so if something isn't working, check the hardware capabilites (you can use
	IDirect3DDevice9::GetDeviceCaps() for this) first.
	
	If this is still foggy be sure to ask the forums found at:  www.GameTutorials.com	
*/

/*-------------------------*\
|  Programmed by:  TheTutor |
|  ©2000-2006 GameTutorials |
\*-------------------------*/



