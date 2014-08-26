// Done by TheTutor

/*	
	If you've spent any time programming 3D applications you've probably seen a rendition
	of the "rotating cube demo".  World renown, the rotating cube app is 
	a beginning rung on the ladder to 3D nirvana.  With some handy D3D functions, if you know how
	to put a cube on the screen, making it rotate is a piece of cake.
	
	Before we get rotating, a bit of vocabulary needs to be defined.  Now if you're
	a pilot, the following words are old hat.  However if you're not into aviation you
	may have never seen these before.
	
		Yaw -- To turn about the vertical axis (for us that's the Y-axis)
		Pitch -- To turn about a horizontal axis (for us that's the X-axis)
		Roll -- To turn about an axis perpendicular to the vertical and horizontal axis
				(for us that's the Z-axis)
				
	So yaw, pitch, and roll essentially just mean rotation around the Y, X, and Z axis 
	respectively.  We'll use D3D's function D3DXMatrixRotationYawPitchRoll() to achieve
	our rotating cube and thus it's necessary to have an understanding of what the terms
	yaw, pitch, and roll mean :)
	
	Additionally we need to talk about index buffers.  They are something used readily in
	3D programming.  So before we get to far into this tutorial, it's important to understand what 
	a index buffer is.

	Index buffers are memory buffers (arrays) that contain index data. Index data (indices)
	are integer offsets into vertex buffers (arrays containing vertices) which specify
	what vertices to use to render a primitive (typically one triangle).		

	Lets go through that definition with a concrete example.  Lets suppose we 
	wanted to draw a 3D cube (which we do).  Lets look at some numbers we know for a cube:

	Sides (Square) = 6
	Vertices = 8
	Triangles = 2 per side * 6 sides = 12
	Vertices needed to render = 3 per triangle * 12 triangles = 36

	So we'd need 36 vertices (most of which would be redundant) to render a cube.  This is
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
	several hundred/thousand vertices model multiplied by the number of models in the application
	you can imagine how it's gonna add up to huge savings.
	
	So without further ado, lets get coding...	 
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

const char kClassName[] = "GT_D3DRotatingCube"; // WNDCLASSEX's name

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
void DrawAndRotateCube(); // Guess what this does %)

// Standard WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

// Standard WinMain
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
	DWORD winStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME;

	// Adjust window rect so it gives us our desired client rect when we 
	// create the window
	AdjustWindowRectEx(&rect, winStyle, false, winStyleEx);

	// Create the window 
    hwnd = CreateWindowEx(winStyleEx, // Window extended style
					      kClassName,
						  "www.GameTutorials.com -- Rotating Cube",
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
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE)) // Handle messages from the OS
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
			Sleep(1); // Otherwise give the OS some time to process other things
	
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
	
	// Here's where all the magic happens.  This functions fills the matrix we 
	// pass in with a rotation matrix using the angles we pass for yaw, pitch and 
	// roll.  So by parameter:
	// &matrix -- D3DXMATRIX to be filled with the resulting rotation matrix
	// angInRad -- Angle in radians to rotate around the Y-axis
	// angInRad -- Angle in radians to rotate around the X-axis
	// angInRad -- Angle in radians to rotate around the Z-axis	
	D3DXMatrixRotationYawPitchRoll(&matrix, angInRad, angInRad, angInRad);
	
	// Set the world matrix
	// By doing this we apply our rotated matrix to anything we render until
	// we set the world matrix to something different 
	g3D->setWorldMatrix(&matrix);

	// Render the cube
	g3D->render(gCubeVerts, 8, gCubeIndices, 36);

	g3D->end(); // End the scene
}

// DirectXion

/*
	Indexed buffers are the way to go when drawing objects that have vertices used
	by more than one primitive, which is generally most models.

	The D3DXMatrixRotationYawPitchRoll() function is a great way to fill a matrix
	with a rotation around all three axis.  Check out some of these other nice 
	D3D functions for other rotation needs:
	
		D3DXMatrixRotationX() // Rotates around X-axis
		D3DXMatrixRotationY() // Rotates around Y-axis
		D3DXMatrixRotationZ() // Rotates around Z-axis
		D3DXMatrixRotationAxis() // Rotates around arbitrary axis
		
	Questions???  Be sure to post 'em at www.GameTutorials.com		
*/

/*-------------------------*\
|  Programmed by:  TheTutor |
|  ©2000-2006 GameTutorials |
\*-------------------------*/



