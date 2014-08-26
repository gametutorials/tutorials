// Done by TheTutor

/*	
	This tutorial is all about how to make your very own matrix class and a LITTLE bit about the 
	linear algebra involved with matrices.
	
	So first off why make your own?  Doesn't D3D have a built in matrix class?  Well yes, D3D does have
	a built in matrix class, and a slew of math functions that can operate on them.  If you
	were only ever going to use D3D for your 3D API, then you might consider using all the built
	in D3D math functionality.  However, if you ever want to use any other API (say OpenGL) it's 
	extremely useful to have your own math classes.  Thus, this tutorial is going an introduction
	to building up your own math library and interfacing it with D3D.
		
	With that, we enter the matrix... tutorial. 
*/

#include <windows.h>
#include "d3d_obj.h"
#include "matrix.h"

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

// Width and height of the window
const int kWinWid = 640;
const int kWinHgt = 480;

const char kClassName[] = "GT_D3DMatrixPart2"; // WNDCLASSEX's name

// The R, G, B of our triangle
const int kRed = 20;
const int kGreen = 20;
const int kBlue = 200;

const float kAngleInc = DEG2RAD(2); // Amount to add to the angle of rotation each frame

//////////////
// Globals //
////////////

// Here's where we create our triangle.  By using a little math
// we'll rest the triangle on the X-axis
SVertex gTriangle[] = 
{	
	{ 1.0f, 0.0f, 0.0f, D3DCOLOR_XRGB(kRed,kGreen,kBlue), },
	{ -1.0f,  0.0f, 0.0f, D3DCOLOR_XRGB(kRed,kGreen,kBlue), }, 
	{ 0.0f, 2.0f, 0.0f, D3DCOLOR_XRGB(kRed,kGreen,kBlue), },
};

bool LockFrameRate(int frame_rate = 60); // Locks the frame rate
void DrawTriangle(); // Draws our triangle to the screen

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
	
	DWORD winStyleEx = WS_EX_CLIENTEDGE; // Window's extended style
	DWORD winStyle = WS_CAPTION | WS_SYSMENU; // Window style

	// Adjust window rect so it gives us our desired client rect when we 
	// create the window
	AdjustWindowRectEx(&rect, winStyle, false, winStyleEx);

	// Create the window
    hwnd = CreateWindowEx(winStyleEx,
					      kClassName,
						  "www.GameTutorials.com -- Matrices Part2",
						  winStyle,
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

	// Set up our projection matrix
	// **NOTE** We only need to do this once cause we're not going to change it ever
	g3D->setProjMatrix(DEG2RAD(60), (float)kWinWid / (float)kWinHgt, 1.0f, 8192.0f);
	
	// Set up our view matrix
	// **NOTE** We only need to do this once cause we're not going to change it ever
	g3D->setViewMatrix(CPos(0.0f, 0.0f, -5.0f), CPos(0.0f, 0.0f, 0.0f)); 

	ShowWindow(hwnd, ishow);
	UpdateWindow(hwnd);

	// While the app is running...
    while(1)
	{
		// Handle any messages from the OS
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
				break;
			
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else if(LockFrameRate()) // If it's time to draw
		{
			static float angle = 0; // Angle of rotation
			
			// We want to move 2 units every second, since we're locking 
			// the frame rate to 60 frames a second, we want to move 5 * (1/60) each frame
			static float moveAmt = 2.0f * (1.0f / 60.0f); 
			static float curWorldX = 0.0f; // Current world x-position of triangle
			
			CMatrix mat1;
			CMatrix mat2;
			
			// Using our matrix class we'll create a rotation matrix and a translation 
			// matrix			
			mat1.makeRotX(angle);
			mat2.makeTranslate(CVector(curWorldX, 0.0f, 0.0f));
			
			// We multiply them together to get a matrix that contains both the rotation
			// and the translation
			CMatrix worldMat = mat1 * mat2;
			
			// We set our world matrix to the new matrix we created so our triangle
			// we'll be drawn with the updated translation and rotation
			g3D->setWorldMatrix(&(worldMat.getD3DMatrix()));
			DrawTriangle(); // Draw triangle in it's new rotated/translated position
			
			// Update "angle" and "curWorldX"
			angle += kAngleInc;
			curWorldX += moveAmt;
			
			// Update moveAmt so we move side to side
			if(curWorldX > 1.0f)
				moveAmt = -moveAmt;
			else if(curWorldX < -1.0f)
				moveAmt = -moveAmt;
		}
		else
			Sleep(1); // Give the OS a slice of time to process other things
	
	} // end of while(1)

	g3D->deinit(); // Free up D3D object
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

// This draws our triangle to the screen
void DrawTriangle()
{
	g3D->begin(); // Begin the scene
	g3D->clear(); // Clear our viewport

	g3D->render(gTriangle, 3); // Draw our triangle

	g3D->end(); // End the scene (Quit rendering and blit to the screen)
}

// A Matrix is 1/2 Math and 1/2 Irix

/*
	Hopefully you understand a wee bit more about matrices.  This tutorial is NOT a substitute
	for learning linear algebra, this tutorial only serves as a introduction to the 
	linear algebra necessary to do 3D graphical programming.  If you are truly interested 
	in doing 3D programming, you will immensely help yourself out by learning vector and 
	matrix algebra. 
	
	**CHALLENGE**
	
		The matrix class provided is only the start of something that could be beautiful.  There
		are further optimizations and methods that could be added to make it highly robust and 
		user friendly to the 3D programmer.  See if you can't add the following functionality:
				
			Inverse of a matrix
			Multiplying of a vector
			Making a scaling matrix
			
		Chances are if you're not a linear algebra guru, you'll have to open a book or two %)
		
	If you have any questions about the matrices or 3D math in general, be sure to post 'em at
	the forums at:  www.GameTutorials.com.
*/

/*-------------------------*\
|  Programmed by:  TheTutor |
|  ©2000-2006 GameTutorials |
\*-------------------------*/



