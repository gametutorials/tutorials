// Done by TheTutor

/*	
	This tutorial is all about matrices.  Now this tutorial will NOT cover all the math 
	behind matrices (linear algebra).  However, it will go over some of their uses specifically
	geared towards D3D programming.

	So for starters what really is a matrix (besides a kick-nut movie :) ???
	Well a matrix is a concise, useful, robust way of uniquely representing 
	linear transformations.  Yeah but what does that mean?  Well in 3D land there are 
	three basic operations that one can do:  translate, rotate, and scale.  A matrix 
	allows us to easily do all three.  And on top of that a homogenous matrix (4x4 matrix)
	allows us to do translating, rotating and scaling all in ONE matrix.

	So that's the short explanation of what a matrix is.

	In D3D, there are three main types of matrices:

	D3DMATRIX // This is your standard 4x4 matrix
	D3DXMATRIX // This is the same as D3DMATRIX (notice no X), however it has been decked out
			  // with some C++ operator overloading goodies.  This makes it easier to work with.
	D3DXMATRIXA16 // This is the same as a D3DXMATRIX with the exception that it is 16-byte
				 // aligned.  The reason we care that its 16-byte aligned is because structures
				// in D3D that are 16-byte aligned are significantly faster than ones that are
			   // not.  So basically we want to use this if we want to improve our speed
		      // performance.

	The end result of this tutorial is to get a spinning triangle on the screen, however, 
	we'll have to go through a couple of steps (using matrices of course) to get there.

	First we'll be setting up our "projection matrix".  This dictates how 3D coordinates (our
	triangle) is to be projected into 2D coordinates (our computer screen)

	Then we'll need to set up our "view matrix".  This dictates how we want to view our 
	3D scene.  Whenever you hear a about a "3D camera" what eventually happens is the 
	data associated with that 3D camera is used to create the "view matrix".

	Last but not least, we'll use a matrix to set up our "world matrix".  By changing the 
	world matrix we'll be able to spin our triangle.

	Okay, it's time to enter the matrix...
*/

#include <windows.h>
#include "d3d_obj.h"

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

const char kClassName[] = "GT_D3DMatrix"; // WNDCLASSEX's name

// The R, G, B of our triangle
const int kRed = 25;
const int kGreen = 225;
const int kBlue = 125;

//////////////
// Globals //
////////////

// Here's where we create our triangle.  By using a little math
// we'll rest the triangle on the X-axis
SVertex triangle[] = 
{	
	{ 1.0f, 0.0f, 0.0f, D3DCOLOR_XRGB(kRed,kGreen,kBlue), },
	{ -1.0f,  0.0f, 0.0f, D3DCOLOR_XRGB(kRed,kGreen,kBlue), }, 
	{ 0.0f, 2.0f, 0.0f, D3DCOLOR_XRGB(kRed,kGreen,kBlue), },
};

bool LockFrameRate(int frame_rate = 60); // Locks the frame rate
void DrawTriangle(); // Draws our triangle to the screen

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
	wndclassex.lpszClassName = kClassName;
	wndclassex.hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(IDC_ARROW),
											IMAGE_CURSOR, 0, 0, LR_SHARED);

    RegisterClassEx(&wndclassex); // Register the WNDCLASSEX

	RECT rect = { 0, 0, kWinWid, kWinHgt }; // Desired window client rect
	
	DWORD winStyleEx = WS_EX_CLIENTEDGE; // Window's extended style
	DWORD winStyle = WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX; // Window style

	// Adjust window rect so it gives us our desired client rect when we 
	// create the window
	AdjustWindowRectEx(&rect, winStyle, false, winStyleEx);

	// Create the window
    hwnd = CreateWindowEx(winStyleEx,
					      kClassName,
						  "www.GameTutorials.com -- D3D Matrix",
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
	g3D->setViewMatrix(CPos(0,0,-5), CPos(0,0,0)); 

	ShowWindow(hwnd, ishow);
	UpdateWindow(hwnd);

	// While the application is active
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
		else if(LockFrameRate()) // If it is time to render
		{
			static int angle = 0;
			angle += 2; // Increment 2° every frame
			
			// Here is a world matrix for us to fill in with our current rotation
			// around either the X, Y, or Z axis (depending on what code is commented in)
            D3DXMATRIX wMatrix;
			
			// Here spin the triangle around an axis -- Try commenting in the below 
			// lines and spinning around the Y or Z axis.  
			// **NOTE** These calls only rotate around ONE axis and DO NOT concatenate.
			//			So if you call D3DXMatrixRotationX() followed by D3DXMatrixRotationY()
			//			the matrix will ONLY be filled with the rotation around the Y cause it's
			//			the last function called
			D3DXMatrixRotationX(&wMatrix, DEG2RAD(angle)); // Spin around the X
			//D3DXMatrixRotationY(&wMatrix, DEG2RAD(angle)); // Spin around the Y
			//D3DXMatrixRotationZ(&wMatrix, DEG2RAD(angle)); // Spin around the Z
			
			g3D->setWorldMatrix(&wMatrix); // Set our world transformation
			
			DrawTriangle(); // Draw the sweet, sweet triangle
		}
		else
			Sleep(1); // Otherwise give the OS a slice of time to process other things
	
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
		case WM_KEYDOWN:

			// If the user hits ESC, close the app
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

	g3D->render(triangle, 3); // Draw our triangle

	g3D->end(); // End the scene (Quit rendering and blit to the screen)
}

// DirectXion

/*
	Well hopefully this tutorial has shed a little light on the subject of matrices.
	As stated early, this tutorial didn't cover the linear algebra behind the matrix.
	It is STRONGLY suggested that if concepts such as "2D projection" and "translating",
	"rotating", "scaling", etc aren't crystal clear, that you snuggle up with your favorite
	linear algebra book for a little one on one session.  You can find books and web links
	that we recommend on the subject of "linear algebra" on the www.GameTutorials.com site.

	Now for those totally intimidated by that last paragraph, 'cause you don't know any math, 
	do not fear.  You DO NOT have to be a math whiz to program in 3D.  Having a basic 
	understanding of the underlying 3D concepts will though help quicken the pace in which you 
	learn 3D.  Now if you want to become a game developer, then yes you HAVE to know 3D math.
	However, you don't have to be a guru at it.  So you don't need a doctorate in math, but 
	you do need to understand the concepts and be able to apply the equations (not necessarily
	derive the equations like they make you in school) to achieve different things in 3D.
	This tutorial is a perfect example of that.  Take for instance the "view matrix".  Well
	we didn't have to know any of the math that goes behind making the matrix (and yes there
	is linear algebra being done to create that view matrix).  But what is important is knowing
	what a view matrix represents, how to use it, what function(s) to call to create one, 
	and how to change it to achieve the desired result.  So to sum it up, if you're not 
	already schooled in 3D math, do yourself a favor and learn a little linear algebra.

	As always, send your questions and comments to the message board at www.GameTutorials.com
*/
	
/*-------------------------*\
|  Programmed by:  TheTutor |
|  ©2000-2006 GameTutorials |
\*-------------------------*/



