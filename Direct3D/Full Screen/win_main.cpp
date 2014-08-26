// Done by TheTutor

/*	
	When creating an decent 3D app there are a few common controls that are more or less expected.
	One of these is the ability to toggle between windowed and full screen mode by pressing
	ALT+ENTER.  This tutorial shows you how to do just that in a D3D app.
	
	Besides just being able to toggle between full screen and windowed mode, this
	tutorial adds a back buffer and does not lock to V-sync.  You find the details
	in the setPresentationParams() method
	
	On to the code...	
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

const char kClassName[] = "GT_D3DFullScreen"; // WNDCLASSEX's name

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

void DrawAndRotateCube(); // Guess what this does %)
bool LockFrameRate(int frame_rate = 60); // Locks the frame rate

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
    wndclassex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
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
						  "www.GameTutorials.com -- Full Screen",
						  winStyle, // Window style
						  CW_USEDEFAULT,
						  CW_USEDEFAULT,
						  rect.right - rect.left,
						  rect.bottom - rect.top,
						  NULL,
						  NULL,
						  hinstance,
						  NULL);
						  
	// Get the client rect and make sure our client is the size we want
	GetClientRect(hwnd, &rect);
	assert(rect.right == kWinWid && rect.bottom == kWinHgt);

	// Init our global 3D object
	if(g3D->init(hwnd) == false)
		return EXIT_FAILURE; // There's been an error, lets get out of this joint
		
	// Set the perspective matrix
	g3D->setProjMatrix(DEG2RAD(60), (float)rect.right / (float)rect.bottom, 1.0f, 8192.0f);

	ShowWindow(hwnd, ishow);
	UpdateWindow(hwnd);

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
		else if(LockFrameRate()) // If it's time to render
		{
			g3D->setViewMatrix(CPos(0.0f, 0.0f, -5.0f), CPos(0.0f, 0.0f, 0.0f));
			DrawAndRotateCube();
		}
		else
			Sleep(1); // Give the OS some time back to process other things
	
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

// Draws and rotates our cube
void DrawAndRotateCube()
{
	D3DXMATRIXA16 matrix;
	static int angle = 0; // The angle of rotation

	g3D->begin(); // Begin the scene
	g3D->clear(); // Clear the viewport
	
	// We add 1° each time we draw and convert to radians
	float angInRad = DEG2RAD(++angle);
	
	// By parameter:
	// &matrix -- D3DXMATRIX to be filled with the resulting rotation matrix
	// angInRad -- Angle in radians to rotate around the Y-axis
	// angInRad -- Angle in radians to rotate around the X-axis
	// angInRad -- Angle in radians to rotate around the Z-axis	
	D3DXMatrixRotationYawPitchRoll(&matrix, angInRad, angInRad, angInRad);
	
	// Set the world matrix
	// By doing this we apply our rotated matrix to anything we render until
	// we set the world matrix to something different 
	g3D->setWorldMatrix(&matrix);

	// Render the cube
	g3D->render(gCubeVerts, kCubeVertCount, gCubeIndices, kCubeIndiceCount);

	g3D->end(); // End the scene
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

// DirectXion

/*
	Toggling from windowed to full screen isn't extremely difficult.  What can make it a
	pain is if you forget to put back all of your render states.  This includes anything
	set by calling SetRenderState() and SetTransform() as well any texture memory surfaces.
	Be sure to make sure you reset all of these states to have your application look as
	you expect once you've toggled screen modes.
		
	If you have questions post 'em up on the forums at www.GameTutorials.com		
*/	

/*-------------------------*\
|  Programmed by:  TheTutor |
|  ©2000-2006 GameTutorials |
\*-------------------------*/

