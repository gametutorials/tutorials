// Done by TheTutor

/*	
	Inside of this tutorial, we will tackle two topics.  First we will render an object
	in a vertex shader while scaling it along its normals.  Second, we will show how
	to debug Direct3D shaders.
	
	For the rendering part, we'll render a box and do two specific things while rendering the box.
	First it shinks/grows the box by moving the vertices that make up the box along it's vertex normals.
	Second,	it linear interpolates between the two colors that make up the boxes vertices giving the
	overall color of the box an oscillating bluish/green look.

	To debug shaders there are a few things you have to set up.  The Direct3D device gets
	properly set up for debugging in the "initForDebug" function.  You'll see in WinMain() where
	you could optionally change the normal "init" to "initForDebug".  Also when launching
	the debugger for a shader, take the following steps :

		1)  Select Debug from the menu
		2)	If you have a current Direct3D SDK installed properly, you should see
			a menu option named, "Direct3D"
		3)  Under the option list for "Direct3D", should be an option named,
			"Start With Direct3D Debugging".  Select this option and you can now debug shaders!

		**NOTE** Debugging shaders is VERY slow.  This is because it simulates all the work of the
				 video card in software.

	 For the rest of the details on how to setup the Direct3D device for debugging, be sure
	 to scout out the method "initForDebug".
	
	Off to the code...	
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

const char kClassName[] = "GT_D3DVertexShader2"; // WNDCLASSEX's name

const int kCubeVertCount = 8; // Number of vertices in the cube
const int kCubeIndiceCount = 36; // Number of indices to define the cube as triangles

//////////////
// Globals //
////////////

// Our 8 cube vertices, half blue and half green
SVertex gCubeVerts[kCubeVertCount] =
{
	{ -1.0f, -1.0f, -1.0f, -0.57735f, -0.57735f, -0.57735f, D3DCOLOR_RGBA(0, 0, 200, 255), }, 
	{ -1.0f, 1.0f, -1.0f, -0.57735f, 0.57735f, -0.57735f, D3DCOLOR_RGBA(0, 0, 200, 255), }, 
	{ 1.0f, -1.0f, -1.0f, 0.57735f, -0.57735f, -0.57735f, D3DCOLOR_RGBA(0, 0, 200, 255), }, 
	{ 1.0f, 1.0f, -1.0f, 0.57735f, 0.57735f, -0.57735f, D3DCOLOR_RGBA(0, 0, 200, 255), }, 
	{ 1.0f, -1.0f, 1.0f, 0.57735f, -0.57735f, 0.57735f, D3DCOLOR_RGBA(0, 200, 0, 255), }, 
	{ 1.0f, 1.0f, 1.0f, 0.57735f, 0.57735f, 0.57735f, D3DCOLOR_RGBA(0, 200, 0, 255), },
	{ -1.0f, -1.0f, 1.0f, -0.57735f, -0.57735f, 0.57735f, D3DCOLOR_RGBA(0, 200, 0, 255), }, 
	{ -1.0f, 1.0f, 1.0f, -0.57735f, 0.57735f, 0.57735f, D3DCOLOR_RGBA(0, 200, 0, 255), }
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
void DrawAndUpdateCube(); // Renders the cube and updates it's position

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
						  "www.GameTutorials.com -- Vertex Shader Part2",
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
		return EXIT_FAILURE; // Error!

	// If you want to debug your shader, comment out the above two lines of code
	// and comment in the below two lines of code
	//if(g3D->initForDebug(hwnd) == false) // Init for shader debugging
	//	return EXIT_FAILURE; // Error!

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
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE)) // If the OS has a message for us
		{
			if(msg.message == WM_QUIT)
				break;
			
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else if(LockFrameRate()) // Else, if it's time to draw
		{
			DrawAndUpdateCube();
		}
		else
			Sleep(1); // Otherwise, give the OS some time to process other programs
	
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
void DrawAndUpdateCube()
{
	D3DXMATRIXA16 matrix;
	
	static int angle = 0; // The angle of rotation
	
	static float curTime = 0.0f; // The current scale
	static float timeInc = 1.0f / 60.0f; // We've locked the frame rate to 1/60th of second so
										// we'll add  that in each frame

	g3D->begin(); // Begin the scene
	g3D->clear(); // Clear the viewport
	
	// Rotate around the +Y axis and increment the angle by 1°
	D3DXMatrixRotationY(&matrix, DEG2RAD(angle++));
			
	// Set the world matrix
	g3D->setWorldMatrix(&matrix);
	
	// We set the time value of our effect file
	g3D->mEffect->SetFloat("gTime", curTime);

	// Render the cube
	g3D->render(gCubeVerts, 8, gCubeIndices, 36);

	g3D->end(); // End the scene
	
	// Update current time
	curTime += timeInc;
	
	// Update timeInc and make sure curTime stays between 0.0f and 1.0f
	if((curTime > 1.0f) || (curTime < 0.0f))
	{
		timeInc = -timeInc;
		curTime += timeInc;
		
		assert(curTime >= 0.0f && curTime <= 1.0f);
	}
}

// Something Smells Shady...

/*
	The ability to debug shaders is fantastic!  Just remember your application will crawl
	while debugging :)

	lerp() is just one of the many great functions that HLSL provides.  Be sure to check 
	out MSDN for all the intrinsic HLSL functions.
	
	Please feel free to post any questions you may have at the GameTutoirals' message board
	which you can find at www.GameTutorials.com 
*/
	

/*-------------------------*\
|  Done by:  TheTutor		|
|  ©2005 GameTutorials, LLC |
\*-------------------------*/
