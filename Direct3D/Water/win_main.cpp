// Done by TheTutor
/*	
	Water is something that is common to many video games.  Take for instance GTA III.  The water
	around Liberty City was done with an technique similar to what is inside of this tutorial.
	
	Here's a high level look at what we are going to do:
	
		1)  Create a vertex type with an (x,y,z), diffuse color, and a set of texture coordinates
		2)  Create a tessellated quad using our created vertex type
		3)  Apply a water texture to the quad
		4)  Move the quad's verts up and down in a sine wave pattern to make the quad have motion
		 
	Now lets jump to the nitty, gritty details %)	
*/

#include <windows.h>
#include <assert.h>
#include "water.h"

// Add the following libraries
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

////////////////
// Constants //
//////////////

// Width and height of the window (specifically the client rect)
const int kWinWid = 800;
const int kWinHgt = 600;

const char kClassName[] = "GT_SimpleWater"; // WNDCLASSEX's name

//////////////
// Globals //
////////////

// A struct for drawing quads
struct SQuad
{
	enum {
		eMaxVerts = 4,
		eMaxIndices = 6
	};

	SVertex verts[eMaxVerts];
	WORD indices[eMaxIndices];
	CD3DTexture texture;
};

SQuad gGround; // Our ground for the water 
CWater gWater; // Our water object

bool LockFrameRate(int frameRate = 30); // Water simulation is tweaked for locked frame rate
bool InitGround(float width, float length, float depth); // Initializes our ground structure
void RenderGround(); // Renders the ground

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
						  "www.GameTutorials.com -- Simple Water",
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
		
	// Init the water object and ground making the ground the same (width x height) as the water
	if((gWater.init() == false) || (InitGround(kWaterWid, kWaterHgt, -2.0f) == false))
		return EXIT_FAILURE; // No water or ground, no tutorial...

	// Set the perspective matrix once because it will never change
	g3D->setProjMatrix(DEG2RAD(60), (float)rect.right / (float)rect.bottom, 1.0f, 8192.0f);
	
	// We set up our view matrix once because it will never change
	g3D->setViewMatrix(CPos(0.0f, 25.0f, -15.0f), CPos(0.0f, 0.0f, 0.0f));

	ShowWindow(hwnd, ishow);
	UpdateWindow(hwnd);

    while(1) // While the app is running...
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
			g3D->clear(); // Clear the color and Z-buffer
			
			RenderGround(); // Draw the rocks below the water
			
			gWater.process(); // Update the water's position
			gWater.render(); // Draw the water
						
			g3D->end(); // End the scene
		}
		else
			Sleep(1); // Otherwise, give the OS some time to process other things
	
	} // end of while(1)

	g3D->deinit(); // Free up CD3DObj
	UnregisterClass(kClassName,hinstance); // Free up WNDCLASSEX
	    return EXIT_SUCCESS; // Application was a success
}

// WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	static bool wireFrame = false;

	switch(message)
    {
		case WM_SYSKEYDOWN:

			// Toggle on ALT + ENTER				
			if(wparam == VK_RETURN && (lparam & (1 << 29)))
			{
				g3D->toggleFullScreen();

				// We need to reset our view
				g3D->setViewMatrix(CPos(0.0f, 25.0f, -15.0f), CPos(0.0f, 0.0f, 0.0f));
			}

		break; // Allow other system keys to be handled by DefWindowProc()
    
		case WM_KEYDOWN:
					
			// If the user presses ESC, close the app
			if(wparam == VK_ESCAPE)
				SendMessage(hwnd, WM_CLOSE, 0, 0);
			
			// If the 'W' key is pressed toggle wire frame mode	
			else if(wparam == 'W')
			{
				if(wireFrame)
					g3D->setFillMode(D3DFILL_SOLID);
				else
					g3D->setFillMode(D3DFILL_WIREFRAME);
					
				wireFrame = !wireFrame;
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

// Init the ground verts
bool InitGround(float wid, float len, float depth)
{
	memset(&gGround.verts, 0, sizeof(gGround.verts));
	
	// Scale width and height by half
	wid *= 0.5f;
	len *= 0.5f;
	
	// Set the value for all 4 vertices
	for(int i = 0; i < gGround.eMaxVerts; ++i)
	{
		gGround.verts[i].x = (i < 2) ? -wid : wid;
		gGround.verts[i].y = depth;
		gGround.verts[i].z = (i == 0 || i == gGround.eMaxVerts - 1) ? -len : len;
		gGround.verts[i].u = (i < 2) ? 0.0f : 1.0f;
		gGround.verts[i].v = (i == 0 || i == gGround.eMaxVerts - 1) ? 1.0f : 0.0f;
		gGround.verts[i].color = D3DCOLOR_ARGB(255, 255, 255, 255); // Set the color to white
	}
	
	// Set ground indices
	gGround.indices[0] = 0;
	gGround.indices[1] = 1;
	gGround.indices[2] = 2;
	gGround.indices[3] = 2;
	gGround.indices[4] = 3;
	gGround.indices[5] = 0;
	
	return gGround.texture.load("rock.jpg"); // Load the ground texture
};

// Render the ground
void RenderGround()
{
	gGround.texture.select();
	g3D->render(gGround.verts, gGround.eMaxVerts, gGround.indices, gGround.eMaxIndices);
}


// Some Extra DirectXion

/*
	This tutorial not only severs as a simple way to do water, you can apply this
	same principle to any piece of cloth you want to fake simulate (such as a flag), or
	any other liquid you may want to fake simulate (such as lava).  There are advanced
	algorithms to get more realistic looking water, but for a good portion of games, this
	technique will do the trick.
	
	If you have any questions, be sure and post 'em on the forums at www.GameTutorials.com	
*/

/*-------------------------*\
|  Done by:  TheTutor		|
|  ©2005 GameTutorials, LLC |
\*-------------------------*/


