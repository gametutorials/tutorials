// Done by TheTutor

/*	
	There is an assumption in this tutorial that the reader has a decent understanding of linear
	algebra.  We'll do a review of the basics, but this is not intended to be a math tutorial.  Instead
	we'll write a two classes CRay and CPlane.  Using the two classes, we'll be able to determine
	when a ray and plane collide and we'll be able to visualize the ray and plane.  Inside of the header
	file for each class you find a quick review of what the mathematical definition of a ray and plane
	are.
	
	Remember when viewing the tutorial that we are doing ray and plane collision.  Planes extended
	forever, but when we render the plane, we only render a finite quad.  Thus it may look
	like the ray is not colliding with the plane, even though it's registering a collision.

	Now, lets get to the good stuff...
*/

#include <windows.h>
#include "d3d_obj.h"
#include "plane.h"

// Include needed libraries
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

const char kClassName[] = "GT_RayPlaneCollision"; // WNDCLASSEX name

// Width and height of the window (specifically the client rect)
const int kWinWid = 800;
const int kWinHgt = 600;

//////////////
// Globals //
//////////// 

// Ray and plane used in the program
CRay gRay(CVector(0,-1,0), CVector(0,1,0));
CPlane gPlane(CVector(0.0f, -1.0f, 0.0f), CPos(0, 0, 0));

bool LockFrameRate(int frame_rate = 60); // Locks the frame rate

// The WinProc
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
						  "www.GameTutorials.com -- Ray and Plane Collision",
						  winStyle, // Window style
						  CW_USEDEFAULT,
						  CW_USEDEFAULT,
						  rect.right - rect.left, // Window width
						  rect.bottom - rect.top, // Window height
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

	// We set up our projection matrix once because it will never change
	g3D->setProjMatrix(DEG2RAD(60), (float)rect.right / (float)rect.bottom, 1.0f, 8192.0f);
	
	// We set our view matrix once because it will never change
	g3D->setViewMatrix(CPos(0,1,-3.0f), CPos(0,0,0));

	ShowWindow(hwnd, ishow);
	UpdateWindow(hwnd);

	// While the app is running
    while(1)
	{
		// If the OS has messages for us, handle them
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
				break;
			
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else if(LockFrameRate()) // If it is time to draw, do so
		{
			g3D->begin(); // Begin drawing
			g3D->clear(); // Clear the screen
				
			// Draw our ray
			gRay.render(D3DCOLOR_ARGB(255,225,225,0));
			
			// Draw our plane in GREEN if our ray intersects with it
			if(gPlane.intersect(gRay))
				gPlane.render(D3DCOLOR_ARGB(255,25,225,25));
			else // Otherwise draw it in RED
				gPlane.render(D3DCOLOR_ARGB(255,225,25,25));
						
			g3D->end(); // Finish drawing
		}
		else
			Sleep(1); // Give the OS a little bit of time to process other things
	
	} // end of while(1)
	
	g3D->deinit(); // Free up CD3DObj
	UnregisterClass(kClassName,hinstance); // Free up WNDCLASSEX
	    return EXIT_SUCCESS; // Application was a success
}

// WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	// Constant amounts to move the origin of the ray by
	const CVector kXAxisMoveAmt(0.05f, 0.0f, 0.0f);
	const CVector kYAxisMoveAmt(0.0f, 0.05f, 0.0f);
	
	switch(message)
    {
		case WM_SYSKEYDOWN:

			// Toggle on ALT + ENTER				
			if(wparam == VK_RETURN && (lparam & (1 << 29)))
			{
				g3D->toggleFullScreen();
				g3D->setViewMatrix(CPos(0,1,-3.0f), CPos(0,0,0)); // Reset the view of our scene
					return 0;
			}

			break; // Allow other system keys to be handled by DefWindowProc()
			
		case WM_KEYDOWN: // If we get a key down message, do stuff

			switch(wparam)
			{
				case VK_ESCAPE: // If they push ESC, close the app
					SendMessage(hwnd, WM_CLOSE, 0, 0);
						break;
						
				case VK_LEFT: // Move the origin of the ray to the left (-X axis)
					gRay.setOrigin(gRay.getOrigin() - kXAxisMoveAmt);
						break;
						
				case VK_RIGHT: // Move the origin of the ray to the right (+X axis)
					gRay.setOrigin(gRay.getOrigin() + kXAxisMoveAmt);
						break;
						
				case VK_DOWN: // Move the origin of the ray down (-Y axis)
					gRay.setOrigin(gRay.getOrigin() - kYAxisMoveAmt);
					break;

				case VK_UP: // Move the origin of the ray up (+Y axis)
					gRay.setOrigin(gRay.getOrigin() + kYAxisMoveAmt);
					break;
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

// When worlds collide... 

/*
	Ray and plane collision is the basic most operation needed for any advanced graphics and
	simple simulations in 3D space.  If you don't fully understand what's going on after reading
	this tutorial, post your question(s) at the forums on www.GameTutorials.com and let the community
	give you a mathamatical helping hand :)    
*/
	
/*-------------------------*\
|  Programmed by:  TheTutor |
|  ©2000-2006 GameTutorials |
\*-------------------------*/