// Done by TheTutor

/*	
	Assume we want to draw a single textured and vertex colored piece of geoemtry to the screen.  
	Using FVF (flexible vertex format) we might define our vertex to look something like this:
	
		SVertex						**NOTE** Notice how the vertex is all packed together in one
		{									 struct.  Also note how we have to define a flag
			float x, y, z;					 that says exactly how the structure is laid out.
			DWORD color;					 
			float u, v;						 
		};
		
		const int SVertexFVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | (1 << D3DFVF_TEXCOUNT_SHIFT);
		
	Now giving the above vertex definition, imagine that sometimes we want to draw
	the geoemtry with only using the vertex color, other times we want to draw
	it only textured, and of course sometimes we want to draw it will both vertex coloring
	and texturing.  With using the above struct and FVF codes we could achieve that 
	goal, however, when drawing with only color or only texturing we would be sending
	extra unneeded information to the graphics card which is a performance hit.  Also, we'd 
	take up extra space since each vertex always contain all three pieces of data (position, 
	diffuse color, and texture coordinates), but sometimes only needs two.
	
	Don't you wish we could create a mesh that can have any type of data we
	want and it could be set dynamically when ever we wanted?  Well you have two wishes left
	for your genie, cause that one just got granted!
	
	Inside of this tutorial we are going to create a mesh class that can contain, position,
	diffuse color, and texture coordinates.  But what you set and in what order you set it
	is totally up to you!  To do this we will NOT be using Direct3D FVF.  Instead we will
	use the concept of "vertex declarations" and "vertex elements".  These two concepts
	combined are affectionally referred to as "vertex streams" by this tutorial.    
	
	So hold on tight and prepare for the roller coaster ride of vertex streams.  Head straight
	to "d3d_mesh.h" and "d3d_mesh.cpp" for the killer plunge %)
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

const char kClassName[] = "GT_D3DVertexStreams"; // WNDCLASSEX's name
const char kTextureName[] = "texture.jpg"; // Texture's file name

// Width and height of the window (specifically the client rect)
const int kWinWid = 800;
const int kWinHgt = 600;

// Count of vertices and indices for a quad
const int kQuadVertCount = 4;
const int kQuadIndexCount = 6;

// The indices for the quad
const WORD kQuadIndices[kQuadIndexCount] = { 0, 1, 2, 2, 3, 0 };

//////////////
// Globals //
////////////

// The quad for rendering
CD3DMesh gQuad;

// Lock the frame rate
bool LockFrameRate(int frame_rate = 60);

// This function is responsible for filling in the data of our 
// global mesh that is defined a few lines above
bool CreateQuad();

// Standard WinProc()
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
						  "www.GameTutorials.com -- D3D \"Vertex Streams\"", // \" is for the quotes symbol
						  winStyle, // Window style
						  CW_USEDEFAULT,
						  CW_USEDEFAULT,
						  rect.right - rect.left, // Window width
						  rect.bottom - rect.top, // Window height
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
	
	// Create the quad!  If we can't create it something bad happened...	
	if(!CreateQuad())
		return false;
		
	// Set the perspective matrix once because it will never change
	g3D->setProjMatrix(DEG2RAD(60), (float)rect.right / (float)rect.bottom, 1.0f, 8192.0f);
	
	// We set up our view matrix once because it will never change
	g3D->setViewMatrix(CPos(0.0f, 0.0f, -5.0f), CPos(0.0f, 0.0f, 0.0f));

	// Initially show and paint the window
	ShowWindow(hwnd, ishow);
	UpdateWindow(hwnd);

	// While the app is running
    while(1)
	{
		// If the OS has massages, handle them
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
				break;
			
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else if(LockFrameRate()) // Otherwise if it is time to draw, do so
		{
			D3DXMATRIX wMat; // World matrix
			
			g3D->begin(); // Begin the scene
			g3D->clear(D3DCOLOR_ARGB(255, 128, 128, 128)); // Clear the Z-buffer
			
			// Move over 2 1/2 units on the -X (left)
			D3DXMatrixTranslation(&wMat, -2.5f, 0.0f, 0.0f);
			g3D->setWorldMatrix(&wMat);
			
			// Draw with only the diffuse color applied
			gQuad.render(true, false);
			
			// Move over 2 1/2 units on the +X (right)
			D3DXMatrixTranslation(&wMat, 2.5f, 0.0f, 0.0f);
			g3D->setWorldMatrix(&wMat);
			
			// Draw with only the single texture applied
			gQuad.render(false, true);
			
			// Move up 1.5 units on the +Y (up)
			D3DXMatrixTranslation(&wMat, 0.0f, 1.5f, 0.0f);
			g3D->setWorldMatrix(&wMat);
			
			// Draw the quad with both diffuse color and a texture
			gQuad.render(true, true);
			
			g3D->end(); // End the scene
		}
		else
			Sleep(1); // Give the OS a bit of time to process other things
	
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

// Creates and sets the data for the "gQuad"
bool CreateQuad()
{
	// First we declare arrays to hold the raw data that the
	// quad will contain
	CPos pos[kQuadVertCount];
	CTexCoord tc[kQuadVertCount];
	DWORD colors[kQuadVertCount];
	
	// Fill in the position data
	pos[0] = CPos(-1.0f, -1.0f, 0.0f);
	pos[1] = CPos(-1.0f, 1.0f, 0.0f);
	pos[2] = CPos(1.0f, 1.0f, 0.0f);
	pos[3] = CPos(1.0f, -1.0f, 0.0f);
	
	// Fill in the texture coordinate data
	tc[0] = CTexCoord(0.0f, 1.0f);
	tc[1] = CTexCoord(0.0f, 0.0f);
	tc[2] = CTexCoord(1.0f, 0.0f);
	tc[3] = CTexCoord(1.0f, 1.0f);
	
	// Fill in the color data
	colors[0] = D3DCOLOR_ARGB(255, 255, 255, 255); // White
	colors[1] = D3DCOLOR_ARGB(255, 255, 255, 255); // White
	colors[2] = D3DCOLOR_ARGB(255, 255, 255, 0); // Yellow
	colors[3] = D3DCOLOR_ARGB(255, 255, 255, 0); // Yellow
	
	// Set all of the data, checking for errors with each method
	if(!gQuad.setPosition(pos, kQuadVertCount) ||
	   !gQuad.setDiffuseColor(colors, kQuadVertCount) ||
	   !gQuad.setTexCoords(tc, kQuadVertCount) ||
	   !gQuad.setIndexBuffer(kQuadIndices, kQuadIndexCount) ||
	   !gQuad.loadTexture(kTextureName))
	{
		return false;
	}

	return true; // Quad is created!
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


// Stream of consciousness

/*
	Using streams can be a great way to setup your vertex data for rendering.  Besides of the
	advantage presented in this tutorial, due to the complete flexibility of streams, you
	can add your own custom data types to your vertex data.  For example, the technique of
	bump mapping (also called normal mapping) requires three basis vectors called tangent space
	at each vector.  The tangent space vectors are ususally called the normal (the "normal" vertex
	normal [pun included for free]), binormal (better labeled the bitangent) and the tangent.  There
	is no FVF code for binoral or tangent, but with using vertex streams, you can easily add
	that data to your vertex.
	
	With vertex and pixel shaders taking over, vertex streams are the way to go for current
	generation and next generation graphic technology.
	
	Questions?  Comments... Concerns!  Post 'em at the only and only www.GameTutorials.com	
*/

/*-------------------------*\
|  Programmed by:  TheTutor |
|  ©2000-2006 GameTutorials |
\*-------------------------*/

