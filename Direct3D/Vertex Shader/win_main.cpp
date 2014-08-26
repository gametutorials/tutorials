// Done by TheTutor

/*	
	The latest buzz word in graphics for the past few years is "shaders".  Now days, shaders have
	finally stablized, somewhat, with higher level langauges, specifically HLSL (High Level
	Shader Language) for D3D and GLSL (GL Shader Langauge) for OpenGL.
	
	This tutorial explains what a shader is and how it works in general.  Then we show
	how to use a simple vertex shader, with an D3D effect file (and explain what that is :)
	
	First a little history.  Shaders first started with Quake3 and Carmack, the genius that
	he is, implemented them all in software with small text files he called "shaders" to
	manipulate the texture matrix.  The the graphic card makers started allowing shaders
	to be done on the graphics card.  Originally, each graphic card maker had it's own language,
	which started as assembly language, to code the shaders.  Now skipping ahead, as mentioned
	at the top of this tutoiral, the two main languages (though still pretty knew) HLSL and
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
								full control of how the your vertices go from model space
								to being drawn on the screen.  This means you have to do
								all the matrix multiplies and math calculations, to get from
								A to B.  This happens in the shader.
								
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
	to conform to how the API expects it data and are limited to the API's render states
	to do your effects.  With the power of being able to program how the vertex data gets 
	rendered you have a lot more options at your hand.   
	
	So lets get going on THIS tutorial.  What we are going to do is create a simple cube, with
	color at each vertex.  Using exclusively the programmable pipeline, we will render the cube
	and spin around the +Y axis.  So lets get going...	
	
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
const int kWinWid = 640;
const int kWinHgt = 480;

const char kClassName[] = "GT_D3DVertexShader"; // WNDCLASSEX's name

const int kCubeVertCount = 8; // Number of vertices in the cube
const int kCubeIndiceCount = 36; // Number of indices to define the cube as triangles

//////////////
// Globals //
////////////

// Our 8 cube vertices, half blue and half green
SVertex gCubeVerts[kCubeVertCount] =
{
	{ -1.0f, -1.0f, -1.0f, D3DCOLOR_RGBA(0, 0, 200, 255), }, 
	{ -1.0f, 1.0f, -1.0f, D3DCOLOR_RGBA(0, 0, 200, 255), }, 
	{ 1.0f, -1.0f, -1.0f, D3DCOLOR_RGBA(0, 0, 200, 255), }, 
	{ 1.0f, 1.0f, -1.0f, D3DCOLOR_RGBA(0, 0, 200, 255), }, 
	{ 1.0f, -1.0f, 1.0f, D3DCOLOR_RGBA(0, 200, 0, 255), }, 
	{ 1.0f, 1.0f, 1.0f, D3DCOLOR_RGBA(0, 200, 0, 255), },
	{ -1.0f, -1.0f, 1.0f, D3DCOLOR_RGBA(0, 200, 0, 255), }, 
	{ -1.0f, 1.0f, 1.0f, D3DCOLOR_RGBA(0, 200, 0, 255), }
};

// Our cube indices
// Remember every cube index, is literally an index into
// our cube vertices array, since we only have 8 cube vertices,
// you'll notice our indices are in the range of 0 - 7
WORD gCubeIndices[kCubeIndiceCount] = { 0, 1, 2,	// Triangle 1
										2, 1, 3,	// Triangle 2
										2, 3, 4,	// Triangle 3
										4, 3, 5,	// Triangle 4
										4, 5, 6,	// Triangle 5
										6, 5, 7,	// Triangle 6
										6, 7, 0,	// Triangle 7
										0, 7, 1,	// Triangle 8
										1, 7, 3,	// Triangle 9
										3, 7, 5,	// Triangle 10
										0, 2, 6,	// Triangle 11
										2, 4, 6 };	// Triangle 12

bool LockFrameRate(int frame_rate = 60); // Locks the frame rate
void DrawAndRotateCube(); // Draws and rotates a cube 

// The WinProc()
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
    wndclassex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclassex.lpszClassName = kClassName;
	wndclassex.hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(IDC_ARROW),
											IMAGE_CURSOR, 0, 0, LR_SHARED);
    
	RegisterClassEx(&wndclassex); // Register the WNDCLASSEX

	RECT rect = { 0, 0, kWinWid, kWinHgt }; // Desired window client rect
	
	DWORD winStyleEx = WS_EX_CLIENTEDGE;
	DWORD winStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME;

	// Adjust window rect so it gives us our desired client rect when we 
	// create the window
	AdjustWindowRectEx(&rect, winStyle, false, winStyleEx);

	// Create the window
    hwnd = CreateWindowEx(winStyleEx, // Window extended style
					      kClassName,
						  "www.GameTutorials.com -- Vertex Shader",
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

	// Set up our projection matrix once because it will not change
	g3D->setProjMatrix(DEG2RAD(60), (float)kWinWid / (float)kWinHgt, 1.0f, 8192.0f);
	
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
		else if(LockFrameRate()) // If it's time to render, do so
		{
			DrawAndRotateCube();
		}
		else
			Sleep(1); // Give the OS some time to process other things
	
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
	
	// Rotate around the +Y axis and increment the angle by 1°
	D3DXMatrixRotationY(&matrix, DEG2RAD(angle++));
	
	// Set the world matrix
	g3D->setWorldMatrix(&matrix);

	// Render the cube
	g3D->render(gCubeVerts, 8, gCubeIndices, 36);

	g3D->end(); // End the scene
}

// Something Smells Shady...

/*
	Shaders are VERY powerful.  Integrating shaders with effect files makes them easy
	to use and manage render state.  Once you get going on shaders, you find that
	linear algebra class you took in high school to become very useful.
	
	If you have any questions, be sure to post 'em at www.GameTutorials.com
	
*/
	

/*-------------------------*\
|  Done by:  TheTutor		|
|  ©2005 GameTutorials, LLC |
\*-------------------------*/
