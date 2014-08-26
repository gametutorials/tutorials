// Done by TheTutor

/*	This tutorial is all about vector based collision -- Vector based collision is an
	excellent way to do collision.  If you do any "higher up" game programming
	you undoubtedly will use vectors (in one way or another).

	This tutorial assumes you already know the basics about vector math.  If the terms
	"dot product" and "projection of a vector" are foreign to you, you'll have to brush up on
	these topics before this tutorial will be of much use. (There's a link to a decent 
	"vector math site" in vector_class.h)

	The heavy commented parts of this tutorial is the "collision code" -- Everything else IS
	NOT as heavily as commented.  This tutorial assumes you can make a window for instance.

	So with that lets go do some vector goodness :)
*/

#include "collide.h"
#include "winobj.h"

#define class_name "GT_VectorCollision"

// Width and height of the window
#define WIN_WIDTH 640
#define WIN_HEIGHT 480

// Locks the frame rate to the "frames per second" passed in
// Returns true when it's okay to draw, false otherwise
bool LockFrameRate(int FPS = 60);

LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprev, PSTR cmdline, int ishow)
{
    HWND hwnd;
	MSG msg;
    WNDCLASSEX wndclassex = {0};

	// Init fields we care about
	wndclassex.cbSize = sizeof(WNDCLASSEX); // Always set to size of WNDCLASSEX
    wndclassex.style = CS_HREDRAW | CS_VREDRAW;
    wndclassex.lpfnWndProc = WinProc;
    wndclassex.hInstance = hinstance;
    wndclassex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclassex.lpszClassName = class_name;
	wndclassex.hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(IDC_ARROW), IMAGE_CURSOR,
											0, 0, LR_SHARED);

    RegisterClassEx(&wndclassex);

    hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, // Window has "sunken edge"
					      class_name,
						  "www.GameTutorials.com -- Vector Based Collision",
						  WS_SYSMENU, // No resizing of the window
						  CW_USEDEFAULT,
						  CW_USEDEFAULT,
						  WIN_WIDTH,
						  WIN_HEIGHT,
						  NULL,
						  NULL,
						  hinstance,
						  NULL);

	srand(GetTickCount()); // Seed the random timer

	// Create our window object 
	CWinObj win_obj;

	// Init our window object
	if(win_obj.init(hwnd) == false)
		return EXIT_FAILURE;
	
	// Init the "collision part" of the application
	Init(hwnd);

	// Create an HBRUSH
	HBRUSH	hbrush = CreateSolidBrush(RGB(20,20,127));

		// Error check
		if(!hbrush)
			return EXIT_FAILURE;
			
	// Select into our back HDC
	HBRUSH old_brush = (HBRUSH)SelectObject(win_obj.getBackHDC(),hbrush);

    ShowWindow(hwnd, ishow);
    UpdateWindow(hwnd);

    while(1)
	{
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
				break;
			
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else if(LockFrameRate()) // If it's time to draw
		{
			// Clear the window
			FillRect(win_obj.getBackHDC(),&win_obj.getClientRect(),
					(HBRUSH)GetStockObject(WHITE_BRUSH));

			RunApp(win_obj.getBackHDC());

			// Draw to the screen
			BitBlt(win_obj.getHDC(),0,0,win_obj.getClientWid(),win_obj.getClientHgt(),
				   win_obj.getBackHDC(),0,0,SRCCOPY);
		}
	
	} // end of while(1)

	// Free up the brush
	SelectObject(win_obj.getBackHDC(),old_brush);
	DeleteObject(hbrush);

	UnregisterClass(class_name,hinstance); // Free up WNDCLASSEX
	    return msg.wParam;
}

// WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    PAINTSTRUCT ps;
		
	switch (message)
    {
		case WM_PAINT:
		
			BeginPaint(hwnd, &ps);	
			EndPaint(hwnd, &ps);
				return 0;

		case WM_KEYDOWN:

			// If escape key is pressed, close the app
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

// You've collided with the end of this tutoiral

/*
	Collision is a major part of most games and is something that certainly should be 
	considered when making your game.  Vector based collision is just one of many
	ways to do collision.  If you haven't tried your hand at a little linear algebra
	yet, now is an excellent time.  You'll use this stuff all over once you go 3D.

	As always, post your questions to www.GameTutorials.com
*/

/*----------------------------*\
|  TheTutor                    |
|  thetutor@gametutorials.com  |
|  © 2000-2005 GameTutorials   |
\*----------------------------*/


