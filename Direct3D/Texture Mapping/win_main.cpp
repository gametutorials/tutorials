// Done by TheTutor

/*	
	Texture mapping is the process of taking an image (a texture) and 
	placing the image perspective correct onto a piece of geometry (map the 
	texture to a polygon).  So before we get to far, lets define some 
	common terminology associated with texture maps.
	
		Texel = Each individual color of a texture map.  Or another
				way to look at it, if a texture was displayed to the
				screen and it took up the entire screen, then each pixel on the
				screen is a texel.
	
		UV = A "UV" is a set of texture coordinates, commonly in the 
			 range of 0.0 - 1.0.  These coordinates specify where
			 each texel should be mapped to.
			 
		Texture Stage = D3D can blend as many as 8 textures onto primitives in a single pass.
						When multiple textures are used on an object, each one is said to
						have it's own "texture stage", where a texture stage not only defines
						which texture will be used, but a slew of texture parameters such as:
						blending mode, additional color arguments (such as vertex color), UV sets,
						mapping modes, pixel shaders, etc, etc.  There are a TON of options you
						can set per texture stage.
						
		Mipmap = Mipmaps are pre-calculated, collections of bitmap images that accompany a main texture,
				 intended to increase rendering speed and reduce artefacts. Mipmapping is the technique
				 used to decide which mipmap(s) to use to render a pixel and how to blend them.  The
				 process goes as follows:  each bitmap image of the mipmap set is a version of the 
				 main texture, but at a certain reduced level of detail.  Although the main texture
				 would still be used when the view is sufficient to render it in full detail, the 
				 renderer will switch to a suitable mipmap image(s), blended depending on the 
				 parameters that are set, when the texture is viewed from a distance, or at a small size.
				 Rendering speed increases since the number of texels being processed can be much lower
				 than with the full texture.  Artefacts are reduced since the mipmap images are 
				 effectively already anti-aliased, taking some of the burden off the real-time renderer.  					 
	
	Once you have the lingo down, by utilizing texture maps, you can finally
	start to make some sweet looking 3D objects.  Texture mapping today can be 
	quite complex.  You have some different variants of the just "straight 
	texture mapping" such as specular mapping, environment mapping, bump mapping,
	light mapping, etc, etc.
	
	This tutorial doesn't cover those advanced topics, rather it is an 
	introduction to texture mapping in D3D.  This app makes a quad and then 
	applies one texture map, TheTexture.bmp, to the quad.
	
	It's time to get mapping... 
*/

#include <windows.h>
#include <assert.h>
#include "d3d_obj.h"
#include "d3d_texture.h"

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

const char kClassName[] = "GT_TextureMapping"; // WNDCLASSEX's name

// Number of vertices and indices for the quad
const int kMaxVerts = 4;
const int kMaxIndices = 6;

//////////////
// Globals //
////////////

// The vertices that we'll use to draw a quad (square) to the screen
// Notice how each vertex has a pair of texture coordinates
SVertex gQuadVerts[kMaxVerts] =
{
	{ -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, },
	{ -1.0f,  1.0f, 0.0f, 0.0f, 0.0f, },
	{  1.0f,  1.0f, 0.0f, 1.0f, 0.0f, },
	{  1.0f, -1.0f, 0.0f, 1.0f, 1.0f, },
};

/*
	UV Layout
	
	If you look at the cube verts above, you'll notice that each 
	vertex has a pair of texture coordinates which are stored
	as floats.  Below is what the mapping for a texture map looks like:
	
	  (0,0)		  (1,0)
		-------------
		|			|
		|			|
		|			|
		|			|
		-------------
	  (0,1)		  (1,1)
	  
	  So if you have a vertex with texture coordinate (0,0) that vertex will
	  get the upper-left texel of that texture.  If you have a vertex with
	  texture coordinate (1,1), that vertex will get the lower-right texel
	  of that texture.
*/

// The indices to our vert list that define the two triangles,
// when together, make our quad for displaying
WORD gQuadIndices[kMaxIndices] = { 0, 1, 2, 2, 3, 0 };

// The texture that we'll use to map to our quad
CD3DTexture gTexture;

// Lock the frame rate
bool LockFrameRate(int frame_rate = 60);

// Standard WinProc
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

	// Create a the window
    hwnd = CreateWindowEx(winStyleEx, // Window extended style
					      kClassName,
						  "www.GameTutorials.com -- Texture Mapping",
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
		
	if(gTexture.load("Texture.bmp") == false)
		return EXIT_FAILURE; // There's been an error, lets get out of this joint

	// Set the perspective matrix once because it will never change
	g3D->setProjMatrix(DEG2RAD(60), (float)rect.right / (float)rect.bottom, 1.0f, 8192.0f);
	
	// We set up our view matrix once because it will never change
	g3D->setViewMatrix(CPos(0.0f, 0.0f, -5.0f), CPos(0.0f, 0.0f, 0.0f));

	ShowWindow(hwnd, ishow);
	UpdateWindow(hwnd);

    while(1) // While the app is running...
	{
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE)) // Handle any messages from the OS
		{
			if(msg.message == WM_QUIT)
				break;
			
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else if(LockFrameRate()) // If it time to render, do so
		{
			g3D->begin(); // Begin the scene
			g3D->clear(); // Clear the Z-buffer
			
			gTexture.select(); // Select the texture to be used
			
			// Render the textured quad
			g3D->render(gQuadVerts, kMaxVerts, gQuadIndices, kMaxIndices);
			
			g3D->end(); // End the scene
		}
		else
			Sleep(1); // Give the OS some time back to do other things
	
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
	Texture mapping is something that has been going on in 3D games for awhile
	now and no 3D engine can be complete with out the ability to do texture mapping.
	D3D allows us to easily load texture maps with a call to D3DXCreateTextureFromFile()
	
	Once a texture is loaded, it can be used by selecting it, then stating 
	what texture stage it is to be used in and how it should be used in that
	texture stage.
	
	** CHALLENGE **
	
		This tutorial just scrapes the surface of texture mapping.  Look up
		the method:
		
			SetTextureStageState()
			
		See if you can't use this method to change how the texture is displayed
		to the screen
		
	As always if you have any questions, comments, news, or beneficial info
	be sure to post it at www.GameTutorials.com		
*/

/*-------------------------*\
|  Programmed by:  TheTutor |
|  ©2000-2006 GameTutorials |
\*-------------------------*/



