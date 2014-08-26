// Done by TheTutor

/*	
	This tutorial is all about loading 3DS Files.  The 3DS file format is the native file
	format for 3D Studio Max files.  It is owned and maintained by AutoDesk.  The file format
	is a "chunk based" file format.  This means the file is made of a series of chunks.  Each
	chunk has an identifier that defines the size and contents of a chunk.  "Chunk based" file
	formats are great, but they can easily be expanded by adding new chunks.  Additionally, only
	the chunk data desired needs to be parsed from the file, allowing more options when loading.

	This tutorial shows how to load the basic chunks you care about when loading a 3D object.
	The chunk defines are found in model_defines.h
	
	And with that, lets check out some models!
*/

#include <windows.h>
#include "d3d_texture.h"
#include "model.h"

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

const char kClassName[] = "GT_D3DModelLoader"; // WNDCLASSEX's name

//////////////
// Globals //
////////////

CModel gModel;

// Lock the frame rate
bool LockFrameRate(int frameRate = 60);

// Standard WinProc()
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
						  "www.GameTutorials.com -- D3D 3DS Model Loader",
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
		
	// Load the model		
	if(gModel.load3DS("spacefighter01.3ds") == false)
		return EXIT_FAILURE;
	
	// Set the perspective matrix once because it will never change
	g3D->setProjMatrix(DEG2RAD(60), (float)rect.right / (float)rect.bottom, 1.0f, 8192.0f);
	
	// We set up our view matrix once because it will never change
	g3D->setViewMatrix(CPos(0.0f, 100.0f, -500.0f), CPos(0.0f, 0.0f, 0.0f));

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
		else if(LockFrameRate()) // If it is time to render
		{
			static int angle = 0;
		
			g3D->begin(); // Begin the scene
			g3D->clear(); // Clear the Z-buffer
			
			D3DXMATRIXA16 wMat;
			
			D3DXMatrixRotationY(&wMat, DEG2RAD(angle++));
			g3D->setWorldMatrix(&wMat);
			
			gModel.render();
			
			g3D->end(); // End the scene
		}
		else
			Sleep(1); // Give the OS some time to process other things		
	
	} // end of while(1)

	g3D->deinit(); // Free up CD3DObj
	UnregisterClass(kClassName, hinstance); // Free up WNDCLASSEX
	    return EXIT_SUCCESS; // Application was a success
}

// Locks the frame rate at "frameRate"
// Returns true when it's okay to draw, false otherwise
bool LockFrameRate(int frameRate)
{
	static float lastTime = GetTickCount() * 0.001f;
	static float elapsedTime = 0.0f;

	float currentTime = GetTickCount() * 0.001f;	// Get the time (milliseconds = seconds * .001)
	float deltaTime = currentTime - lastTime;		// Get the slice of time
	float desiredFPS = 1.0f / frameRate;			// Store 1 / desiredFrameRate

	elapsedTime += deltaTime;						// Add to the elapsed time
	lastTime = currentTime;							// Update lastTime

	// Check if the time since we last checked is greater than our desiredFPS
	if(elapsedTime > desiredFPS)
	{
		elapsedTime -= desiredFPS; // Adjust our elapsed time
		return true; // Time to draw
	}

	// Still waiting...
	return false;
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
				g3D->setViewMatrix(CPos(0.0f, 100.0f, -500.0f), CPos(0.0f, 0.0f, 0.0f));
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


// DirectXion

/*
	If you're doing 3D you need models.  3D Studio Max is a widely used modeling package to
	create those models.  The chunk based data structure makes it fairly easy to load whatever
	you want from a .3DS file for your own application.
	
	Questions?  Check out the forums at www.GameTutorials.com	
*/

/*-------------------------*\
|  Programmed by:  TheTutor |
|  ©2000-2007 GameTutorials |
\*-------------------------*/



