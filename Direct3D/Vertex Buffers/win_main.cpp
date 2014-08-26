// Done by TheTutor

/*	
	This tutorial is all about D3D vertex buffers.  So before we get to far a fair question
	would be, "What is a vertex buffer?"  Well in it's most basic definition, a vertex buffer
	is a array of vertex data.  A more refined definition would be, "A vertex buffer is a object
	which contains vertex data and properties which govern the vertex data".  For instance
	a D3D vertex buffer allows you to set the memory management of the vertex buffer, which
	depending on the type you choose, can improve performance.
	
	So to effectively manage this D3D vertex buffer, we're going to create a simple 
	mesh class.  A mesh is usually thought of as a collection of primitive data (usually
	triangles) and any other properties which define the mesh, such as lighting material
	properties, alpha, it's bounding volume for collisions, etc, etc.  Our mesh class
	will be a simple one, but it will help to get you acquainted with vertex buffers.
	
	So lets get coding... 
*/

#include <windows.h>
#include "d3d_texture.h"
#include "d3d_mesh.h"

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

const char kClassName[] = "GT_D3DVertexBuffers"; // WNDCLASSEX's name

const int kMaxVerts = 6; // Number of verts in quad

//////////////
// Globals //
////////////

// The vertices that we'll use to draw a quad (square) to the screen
// Notice how each vertex has a pair of texture coordinates
SVertex gQuadVertexData[kMaxVerts] =
{
	// Triangle 1
	{ -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, },
	{ -1.0f,  1.0f, 0.0f, 0.0f, 0.0f, },
	{  1.0f,  1.0f, 0.0f, 1.0f, 0.0f, },
	
	// Triangle 2
	{  1.0f,  1.0f, 0.0f, 1.0f, 0.0f, },
	{  1.0f, -1.0f, 0.0f, 1.0f, 1.0f, },
	{ -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, },
};

// The texture that we'll use to map to our quad
CD3DTexture gTexture;

// This is our mesh object that contains a D3D vertex buffer which will
// be filled with our global vertex data
CD3DMesh gQuad;

// Lock the frame rate
bool LockFrameRate(int frame_rate = 60);

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
						  "www.GameTutorials.com -- D3D Vertex Buffers",
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
		
	if(gTexture.load("Texture.jpg") == false)
		return EXIT_FAILURE; // There's been an error, lets get out of this joint
		
	// Create and set the vertex buffer in our CD3DMesh object
	if(gQuad.setVertexData(gQuadVertexData, kMaxVerts) == false)
		return EXIT_FAILURE; // This is bad!  The tutorial is officially broken

	// Set the perspective matrix once because it will never change
	g3D->setProjMatrix(DEG2RAD(60), (float)rect.right / (float)rect.bottom, 1.0f, 8192.0f);
	
	// We set up our view matrix once because it will never change
	g3D->setViewMatrix(CPos(0.0f, 0.0f, -5.0f), CPos(0.0f, 0.0f, 0.0f));

	ShowWindow(hwnd, ishow);
	UpdateWindow(hwnd);

    while(1) // While the app is running
	{
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE)) // Handle messages from the OS
		{
			if(msg.message == WM_QUIT)
				break;
			
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else if(LockFrameRate()) // If it is time to draw, do so
		{
			g3D->begin(); // Begin the scene
			g3D->clear(); // Clear the Z-buffer
			
			gTexture.select(); // Select the texture to be used
			
			// Render the textured quad
			gQuad.render();
			
			g3D->end(); // End the scene
		}
		else
			Sleep(1); // Give the OS a slice of time to process other things
	
	} // end of while(1)

	g3D->deinit(); // Free up CD3DObj
	UnregisterClass(kClassName, hinstance); // Free up WNDCLASSEX
	    return EXIT_SUCCESS; // Application was a success
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


// DirectXion

/*
	Once speed becomes an issues in your rendering framework (and it won't take long
	before it does) you'll want to use D3D vertex buffers to store your data.  Rendering
	when using D3D vertex buffers will give you a serious performance boost.
	
	As always, if you have any questions about D3D vertex buffers or questions about
	programming in general, be sure to post 'em at the forums at www.GameTutorials.com  
*/

/*-------------------------*\
|  Programmed by:  TheTutor |
|  ©2000-2006 GameTutorials |
\*-------------------------*/

