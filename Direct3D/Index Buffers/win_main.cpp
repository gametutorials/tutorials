// Done by TheTutor

/*	
	This tutorial is all about D3D index buffers.  Before we jump into the specifics
	of implementation, here's a quick review of the high level concept of an 
	index buffer.
	
	Index buffers are memory buffers (typically arrays) that contain index data. Index data
	are integer offsets into vertex buffers (typically arrays containing vertex information)
	which specify what vertices to use to render a primitive (typically one triangle).		

	Lets go through that definition with a concrete example.  Lets suppose we 
	wanted to draw a 3D cube with a color at each of the vertices.  Lets look at some numbers
	we know for a cube:

	Sides (Square) = 6
	Vertices = 8
	Triangles = 2 per side * 6 sides = 12
	Vertices needed to render = 3 per triangle * 12 triangles = 36

	So we'd need 36 vertices (some of which would be redundant) to render a cube.  This is
	where index buffers come into play.  Instead of copying the vertices several times 
	we only use one copy, then create an array which indexes into our vertex array and
	specifies which vertices to use to make the 12 triangles.

	Why do we do this?  The main reason is that it saves memory.  Lets prove that
	quickly.  Here's the memory foot print for rendering a cube using 36 vertices
	
	struct SVertex
	{
		float x, y, z; // World position
		DWORD color; // Diffuse color
	};

	Size of our vertex = 3 floats + 1 DWORD = 16 bytes
	Total size = 36 verts * 16 bytes = 576 bytes

	If we're using indices where each index is a WORD:

	Size of our vertex = 3 floats + 1 DWORD = 16 bytes
	Vertex buffer size = 8 verts * 16 bytes = 128 bytes
	Index buffer size = 36 indices * 2 bytes = 72 bytes
	Total Size = 128 + 72 = 200 bytes

	As you can see with just a simple cube we save 376 bytes!  If you extend that to a 
	several thousand vertex model, multiplied by the number of models in the application,
	you can imagine how it's gonna add up to huge savings.
	
	So now that we have had done a quick index buffer overview, lets jump to the nitty-gritty
	D3D details...
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

const char kClassName[] = "GT_D3DIndexBuffers"; // WNDCLASSEX's name

// Max vertices and indices for quad
const int kMaxVerts = 4;
const int kMaxIndices = 6;

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
	{  1.0f, -1.0f, 0.0f, 1.0f, 1.0f, },
};

// This is the indices that will make up the two triangles in our quad
WORD gQuadIndexData[kMaxIndices] = { 0, 1, 2, 2, 3, 0 };

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
						  "www.GameTutorials.com -- D3D Index Buffers",
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
		
	// Set the vertex buffer in our CD3DMesh object
	if(gQuad.setVertexData(gQuadVertexData, kMaxVerts) == false)
		return EXIT_FAILURE; // This is bad!  The tutorial is officially broken
		
	// Set the index buffer in our CD3DMesh object
	if(gQuad.setIndexData(gQuadIndexData, kMaxIndices) == false)
		return EXIT_FAILURE; // This is bad!  The tutorial is officially broken

	// Set the perspective matrix once because it will never change
	g3D->setProjMatrix(DEG2RAD(60), (float)rect.right / (float)rect.bottom, 1.0f, 8192.0f);
	
	// We set up our view matrix once because it will never change
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
		else if(LockFrameRate()) // If it is time to render
		{
			g3D->begin(); // Begin the scene
			g3D->clear(); // Clear the Z-buffer
			
			gTexture.select(); // Select the texture to be used
			
			// Render the textured quad
			gQuad.render();
			
			g3D->end(); // End the scene
		}
		else
			Sleep(1); // Give the OS some time to process other things
	
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
	Index buffers are a great way to save space and in the long run can give you a speed 
	boost as well.  They are essential if you need to get your program to run in small
	memory constraints, such as the PS2, Xbox, or GameCube.  With D3D's simple way to
	utilize vertex and index buffers, you may as well use in your own application %)

	If you have any questions or comments be sure to post 'em on the forums at:
	www.GameTutorials.com	
*/

/*-------------------------*\
|  Programmed by:  TheTutor |
|  ©2000-2006 GameTutorials |
\*-------------------------*/



