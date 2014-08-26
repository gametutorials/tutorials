// Done by TheTutor

/*	
	Multi-texturing is the process of taking multiple textures and blending them together
	to create a "final texture" which will ultimately be used in the texture mapping process.
	
	This process looks something like this:
	
	---------		---------		---------
	|*******|		|		|		|*******|
	|		|	+	|~~~~~~~|	=	|~~~~~~~|
	|*******|		|		|		|*******|
	---------		---------		---------
									
	Texture 1		Texture 2		Multi-texture
	
	Any current games (UT2K4 jumps to mind) make extensive use of multi-texturing to
	acheive their breathe taking visuals.  While pixel shaders are employed to do most of their
	multi-texturing tasks, D3D allows you to blend up to eight textures using the fixed function
	pipeline (ie by NOT using shaders).
	
	This tutorial shows the basics of how to go about doing light mapping, the process of blending
	a light texture (a grey scale texture which makes up the shape and intensity of a light striking
	an object) and a diffuse texture (the texture which gives an object it's color).
	
	So without further ado...	
*/

#include <windows.h>
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

const char kClassName[] = "GT_MultiTexturing"; // WNDCLASSEX's name

const int kMaxTextures = 3; // Max number of textures
const int kMaxVerts = 4; // Number of vertices in quad to be rendered
const int kMaxIndices = 6; // Number of indices in quad to be rendered

//////////////
// Globals //
////////////

// The vertices that we'll use to draw a quad to the screen
// **NOTE** You notice that even though we are doing multi-texturing we 
// only have ONE pair of UV's.  We use the same set of UVs for both 
// textures thus there is only need to store one set.
SVertex gQuadVerts[kMaxVerts] =
{
	{ -1.0f, -1.0f, 0.0f, 0.0f, 1.0f,  },
	{ -1.0f,  1.0f, 0.0f, 0.0f, 0.0f,  },
	{  1.0f,  1.0f, 0.0f, 1.0f, 0.0f,  },
	{  1.0f, -1.0f, 0.0f, 1.0f, 1.0f,  },
};

// The indices to our vert list that define the two triangles,
// when together, make our quad for displaying
WORD gQuadIndices[kMaxIndices] = { 0, 1, 2, 2, 3, 0 };

// The textures that we'll use to map to our quad
CD3DTexture gTextures[kMaxTextures];

// Locks the frame rate
bool LockFrameRate(int frame_rate = 60);

// The function name says it all :)
void DrawMultiTexturedQuad();

// Standard Win32 WinProc
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
						  "www.GameTutorials.com -- Multi-texturing",
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
	
	// Load the textures
	if(gTextures[0].load("wall.bmp") == false ||
	   gTextures[1].load("lightmap.bmp") == false)
		return EXIT_FAILURE; // If we can't load one of the textures, bail!

	// Get the client rect and make sure our client is the size we want
	GetClientRect(hwnd, &rect);
	assert(rect.right == kWinWid && rect.bottom == kWinHgt);

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
			g3D->clear(D3DCOLOR_XRGB(0, 0, 128)); // Clear the window's color to blue and the z-buffer
			
			D3DXMATRIX world;
			
			// Translate to upper left of window
			D3DXMatrixTranslation(&world, -1.5f, 1.5f, 0.0f);
			g3D->setWorldMatrix(&world);
			
			gTextures[0].select(0);
			g3D->render(gQuadVerts, kMaxVerts, gQuadIndices, kMaxIndices);
			
			// Translate to lower left of window
			D3DXMatrixTranslation(&world, -1.5f, -1.5f, 0.0f);
			g3D->setWorldMatrix(&world);

			gTextures[1].select(0); // Select the texture to be used
			g3D->render(gQuadVerts, kMaxVerts, gQuadIndices, kMaxIndices);
			
			// Translate to middle right of window
			D3DXMatrixTranslation(&world, 1.5f, 0.0f, 0.0f);
			g3D->setWorldMatrix(&world);
			
			DrawMultiTexturedQuad();
			
			g3D->end(); // End the scene
		}
		else
			Sleep(1); // Give the OS some time back
	
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

// As the function name suggests, this draws a multi-textured quad %)
void DrawMultiTexturedQuad()
{
	// Following is the wrapper function texture stage parameters that need to be set in order
	// for multi-texturing to work.  Let's look at what each one means in detail

	// For the 2nd stage (using zero based counting) we want to set the D3DTSS_COLOROP
	// to D3DTOP_MODULATE.  The D3DTSS_COLOROP property stipulates the color operation between
	// the two color arguments for this particular stage. The value D3DTOP_MODULATE says to
	// take the two color arguments (which in this specific case will be the color from the first
	// texture and the color from the second texture) and modulate, a.k.a multiply, them together.
	g3D->setTextureState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
	
	// For the 2nd stage we also want to specify which UV's we want to use.  Since our vertex
	// only has one set of UV's we set the 2nd stage to use the 1st stage's UVs
	g3D->setTextureState(1, D3DTSS_TEXCOORDINDEX, 0);

	gTextures[0].select(0); // Select the texture for the 1st pass
	gTextures[1].select(1); // Select the texture for the 2nd pass

	// Render the textured quad
	g3D->render(gQuadVerts, kMaxVerts, gQuadIndices, kMaxIndices);

	// Once we've rendered, we need to disable the modulation of colors.  Otherwise
	// all rendering will blend the second texture with the first.  Comment this out
	// out if you don't believe me %)
	g3D->setTextureState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
}

// DirectXion

/*
	Multi-texturing is essential to acheive the visuals found in todays games.  D3D provides
	lots of ways you can blend textures.  The modes that can be set using the 
	SetTextureStageState() method provide lots of ways to blend textures.
	
	Questions regarding multi-texturing or any programming question should be posted to the 
	forums located on www.GameTutorials.com
*/

/*-------------------------*\
|  Programmed by:  TheTutor |
|  ©2000-2006 GameTutorials |
\*-------------------------*/



