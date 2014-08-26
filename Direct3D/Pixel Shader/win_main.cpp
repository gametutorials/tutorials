// Done by TheTutor

/*	
	The latest buzz word in graphics for the past few years is "shaders".  Now days, shaders have
	finally stablized, somewhat, with higher level langauges, specifically HLSL (High Level
	Shader Language) for D3D and GLSL (GL Shader Langauge) for OpenGL.

	This tutorial explains what a shader is and how it works in general.  Then we show
	how to use a simple pixel shader, with an D3D effect file (and explain what that is :)

	First a little history.  Shaders first started with Quake3 and Carmack, the genius that
	he is, implemented them all in software with small text files he called "shaders" to
	manipulate the texture matrix.  Next, the graphic card makers started allowing shaders
	to be done on the graphics card.  Originally, each graphic card maker had it's own language,
	which started as assembly language, to code the shaders.  Now skipping ahead, as mentioned
	at the top of this tutoiral, the two main languages (though still pretty new) HLSL and
	GLSL look like they are going to be the future of hardware shaders.

	Now some definitions:

	Shader:		A precompiled piece of code that runs on the GPU and operates on geometry sent to
	it, with the result getting drawn to the screen.

	Fixed Function Pipeline:	This is currently how most D3D apps run.  You set all
	the properties of how something is going to be rendered
	(for instance the view matrix, perspective matrix, vertex lights,
	etc, etc), send it to the video card and it makes it appear
	on the screen.

	Programmable Pipeline:		This is where shaders come in.  Now as a programmer, you have
	full control of how each pixel is rendered to the screen.  This means you have to do
	all the matrix multiplies and math calculations, to get from
	a collection of vertices in model space to a final RGB color drawn to the 
	monitor.  All this wonder magic happens in the shader.

	Effect File:	This is D3D specific file, usually denoted with a .fx extension.  It 
	is a D3D object that seriously helps setting up and executing shaders in
	a D3D app.  Additionally it offers a concept called "techniques".
	Using techniques, you can write a shader multiple ways, and then pick which
	one to use, based on the graphics card running the application.  Very handy
	for PC development.  We'll talk about these in the .fx file.

	Currently, there are two types of shaders, vertex shaders and pixel shaders.  Lets go
	through a quick example to see how they work together.

	Suppose you have a shader associated with drawing a textured quad.  So you send the quad 
	(in model space) to the graphics cards and say "draw me".  Since the quad is textured, will
	need a vertex shader and pixel shader to make it display as we would expect.  For each vert 
	in the quad (which there would be 4) the shader would run once.  For every pixel that gets 
	drawn to the screen (which the number depends on many factors, such as size of the quad)
	the pixel shader would get run once.

	This is currently how it works in the fixed function pipeline, the main difference is you have
	to conform to how the API wants its data and are limited to the API's rendering states
	to do your special effects.  With the power of being able to program how the vertex/pixel data gets 
	rendered to the screen you have an infinite number of options at your hand.  Prepare to exercise
	your new found power!   

	Okay, okay general introduction is out of the way.  What does this tutorial do?  It renders
	a spinning, textuerd cube to the screen.  Not rocket science (that's now Carmak's business) but
	a good introduction to pixel shaders.  So lets get going...	

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

const char kClassName[] = "GT_D3DPixelShader";

const int kCubeVertexCount = 24; // Number of vertices in the cube
const int kCubeIndexCount = 36; // Number of indices to define the cube as triangles

//////////////
// Globals //
////////////

// Our 8 cube vertices with texture coordinates
// **NOTE** Side labeling assumes we're looking directly at the cube with the front side
//			facing us
SVertex gCubeVerts[kCubeVertexCount] =
{
	// Front side
	{ -1.0f, -1.0f, -1.0f, 0.0f, 1.0f }, 
	{ -1.0f, 1.0f, -1.0f, 0.0f, 0.0f }, 
	{ 1.0f, -1.0f, -1.0f, 1.0f, 1.0f}, 
	{ 1.0f, 1.0f, -1.0f, 1.0f, 0.0f}, 

	// Right Side
	{ 1.0f, -1.0f, -1.0f, 0.0f, 1.0f},  
	{ 1.0f, 1.0f, -1.0f, 0.0f, 0.0f}, 
	{ 1.0f, -1.0f, 1.0f, 1.0f, 1.0f}, 
	{ 1.0f, 1.0f, 1.0f, 1.0f, 0.0f}, 

	// Back Side
	{ 1.0f, -1.0f, 1.0f, 0.0f, 1.0f},  
	{ 1.0f, 1.0f, 1.0f, 0.0f, 0.0f},  
	{ -1.0f, -1.0f, 1.0f, 1.0f, 1.0f}, 
	{ -1.0f, 1.0f, 1.0f, 1.0f, 0.0f},

	// Left Side
	{ -1.0f, -1.0f, 1.0f, 0.0f, 1.0f }, 
	{ -1.0f, 1.0f, 1.0f, 0.0f, 0.0f }, 
	{ -1.0f, -1.0f, -1.0f, 1.0f, 1.0f }, 
	{ -1.0f, 1.0f, -1.0f, 1.0f, 0.0f }, 

	// Top Side
	{ -1.0f, 1.0f, -1.0f, 0.0f, 1.0f }, 
	{ -1.0f, 1.0f, 1.0f, 0.0f, 0.0f }, 
	{ 1.0f, 1.0f, -1.0f, 1.0f, 1.0f}, 
	{ 1.0f, 1.0f, 1.0f, 1.0f, 0.0f}, 

	// Bottom Side
	{ -1.0f, -1.0f, 1.0f, 0.0f, 1.0f},
	{ -1.0f, -1.0f, -1.0f, 0.0f, 0.0f },
	{ 1.0f, -1.0f, 1.0f, 1.0f, 1.0f},
	{ 1.0f, -1.0f, -1.0f, 1.0f, 0.0f},
};

// Our cube indices
WORD gCubeIndices[kCubeIndexCount] = 
{ 
	0, 1, 2, // Front Side Triangle 1
	2, 1, 3, // Front Side Triangle 2
	4, 5, 6, // Right Side Triangle 1
	6, 5, 7, // Right Side Triangle 2
	8, 9, 10, // Back Side Triangle 1
	10, 9, 11, // Back Side Triangle 2
	12, 13, 14, // Left Side Triangle 1
	14, 13, 15, // Left Side Triangle 2
	16, 17, 18, // Top Side Triangle 1
	18, 17, 19, // Top Side Triangle 2
	20, 21, 22, // Bottom Side Triangle 1
	22, 21, 23 // Bottom Side Triangle 2
};

CD3DTexture gTexture; // The cube's texture

bool LockFrameRate(int frame_rate = 60); // Locks the frame rate
void DrawAndRotateCube(); // Three guesses as what this does... First two don't count

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
	DWORD winStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME; // We like our frame's thick!

	// Adjust window rect so it gives us our desired client rect when we 
	// create the window
	AdjustWindowRectEx(&rect, winStyle, false, winStyleEx);

	// Create the window
    hwnd = CreateWindowEx(winStyleEx, // Window extended style
					      kClassName,
						  "www.GameTutorials.com -- Pixel Shader",
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
		
	// Load the texture
	if(gTexture.load("texture.jpg") == false)
		return EXIT_FAILURE; // No texture, no tutorial...

	// Get the client rect and make sure our client is the size we want
	GetClientRect(hwnd, &rect);
	assert(rect.right == kWinWid && rect.bottom == kWinHgt);

	// Set up our projection matrix once because it will not change
	g3D->setProjMatrix(DEG2RAD(60), (float)kWinWid / (float)kWinHgt, 1.0f, 8192.0f);
	
	// We set up our view matrix once because it will never change
	g3D->setViewMatrix(CPos(0.0f, 0.0f, -5.0f), CPos(0.0f, 0.0f, 0.0f));

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
		else if(LockFrameRate()) // If it is time to render
		{
			DrawAndRotateCube();
		}
		else
			Sleep(1); // Give the OS some time back
	
	} // end of while(1)

	g3D->deinit(); // Free up the D3D object
	UnregisterClass(kClassName,hinstance); // Free up WNDCLASSEX
	    return EXIT_SUCCESS; // Application was a success
}

// WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch(message)
    {
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

// Draws and rotates our cube
void DrawAndRotateCube()
{
	D3DXMATRIXA16 matrix;
	static int angle = 0; // The angle of rotation

	g3D->begin(); // Begin the scene
	g3D->clear(); // Clear the viewport
	
	// We add 1° each time we draw and convert to radians
	float angInRad = DEG2RAD(++angle);
	
	// Rotate the cube
	D3DXMatrixRotationYawPitchRoll(&matrix, angInRad, angInRad, 0);
	
	// Set the world matrix
	g3D->setWorldMatrix(&matrix);
	
	// Select the cube's texture
	gTexture.select();

	// Render the cube
	g3D->render(gCubeVerts, kCubeVertexCount, gCubeIndices, kCubeIndexCount);

	g3D->end(); // End the scene
}

// Pixel Power

/*
	Pixel shaders are all the buzz and you can bet your last doughnut that pixel shaders
	will around for the foreseeable future.  The great thing about shaders is that
	they give us, the programmers, all the power!  So if you find this new found freedom
	a bit too much handle, don't hesitate to drop a question on the forums at www.GameTutorials.com   		
*/

/*-------------------------*\
|  Done by:  TheTutor		|
|  ©2005 GameTutorials, LLC |
\*-------------------------*/


