// Done by TheTutor

/*	
	Okay what we're going to do here is a little drawing using an HBRUSH.  Windows refers
	to all objects that can be used to draw things in a window collectively as GDI
	(Graphic Device Interface) objects.  To us, a HBRUSH is nothing more than a handle to a brush.
	So all that's really important to know up front is that we can draw stuff by using HBRUSHs.
	But lets dig a bit deeper...

	In Win32 brushes are used to paint shapes.  So if we wanted to draw a rectangle to the 
	screen, using a Win32 API, the color contained inside the bounding area of the rectangle would
	be filled in by whatever brush is currently selected.  

	Brushes are associated with a device context.  A device context is a pointer that lets us
	talk to hardware devices, such as the graphics card, without knowing anything about the
	hardware device.  In this tutorial we will get the device context of our window and 
	associate different brushes with it, drawing different rectangles to the screen.

	So without further ado, lets draw.

*/

#include <windows.h>
#include <assert.h>

////////////////
// Constants //
//////////////

const int kWinWid = 640;
const int kWinHgt = 480;

const char kClassName[] = "GameTutorials_HBRUSH";

//////////////
// Globals //
////////////

HDC gHDC = NULL; // This will be our global device context
RECT gRect = {0}; // This will hold our window's client area

// This function draws a random rectangle with a random pen
void DrawRandomRectangle();

// Standard callback function
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

// Win32 main()
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprev, PSTR cmdline, int ishow)
{
 	HWND hwnd;
    MSG msg;
    WNDCLASSEX wndclassex = {0};

	// Fill the fields we care about
	wndclassex.cbSize = sizeof(WNDCLASSEX);
    wndclassex.style = CS_HREDRAW | CS_VREDRAW;
    wndclassex.lpfnWndProc = WinProc;
    wndclassex.hInstance = hinstance;
    wndclassex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclassex.lpszClassName = kClassName;
    wndclassex.hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(IDC_ARROW), IMAGE_CURSOR,
											0, 0, LR_SHARED); // Load default window's arrow cursor

    RegisterClassEx(&wndclassex);	// Register the window

    hwnd = CreateWindowEx(NULL, // No extra window attributes
						  kClassName,
						  "www.GameTutorials.com -- HBRUSHs",
						  WS_OVERLAPPEDWINDOW,
						  CW_USEDEFAULT, // Window will receive a default X-pos on screen
						  CW_USEDEFAULT, // Window will receive a default Y-pos on screen
						  kWinWid,
						  kWinHgt,
						  NULL,
						  NULL,
						  hinstance,
						  NULL);

		// Error check
		if(!hwnd)
			return EXIT_FAILURE; // Something really bad happened!

	gHDC = GetDC(hwnd); // Get the window's device context
		
		//Error check
		if(!gHDC)
			return EXIT_FAILURE; // Couldn't get window's hdc
			
	// Get the client area of the window
	GetClientRect(hwnd, &gRect);

	ShowWindow(hwnd, ishow);
    UpdateWindow(hwnd);

    while(1)
	{
		// Get message(s) if there is one
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
				break;
				
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			DrawRandomRectangle(); // Draw some rectangle goodness
			Sleep(1000); // Wait for a second, draw another random rectangle
		}
	}

	UnregisterClass(kClassName,hinstance); // Free up WNDCLASSEX
		return msg.wParam;
}

// The WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
   	// Handle messages we care about
    switch(message)
    {
       case WM_DESTROY: // Destroy the window
            
            ReleaseDC(hwnd,gHDC); // Free up the global HDC
			PostQuitMessage(0); // Post WM_QUIT message
				return 0;

    } // end of switch(message)

    return DefWindowProc(hwnd, message, wparam, lparam);
}

// Draw a randomly filled in rectangle to the screen
void DrawRandomRectangle()
{
	// This is our brush object that we will allocate 
	// and use to draw our rectangle with.  So what is "oldBrush" you ask?
	// It will get explained a few lines down, so just hold tight %)
	HBRUSH hbrush = NULL;
	HBRUSH oldBrush = NULL;

	// We want a different color each time we draw a rectangle so we
	// create a random color every time. 
	// *** NOTE ***	rand() % n returns a number between 0 and (n-1).  That's why
	//				we modulus rand() by 256 since color values are between 0 and 255.
	COLORREF color = RGB(rand()%256, rand()%256, rand()%256);

	// Windows has a few different functions for creating brushes so be sure to check
	// out MSDN to learn about all of them.  We are going to use CreateSolidBrush(), it as
	// the name suggest, creates a solid colored brush.
	hbrush = CreateSolidBrush(color); // Create brush with the specified color.
	assert(hbrush != NULL); // If CreateSolidBrush fails it returns NULL.

	// Okay so we have a newly created HBRUSH.  Yeah!  But now what?  Well we
	// need to tell the device context we're using to draw with about it.  Now when
	// you create a window, you get some things by default.  One of these 
	// things is a device context associated with the window.  On top of that,
	// the device context already has a default brush associated with it.  Windows
	// will allow you to color with other brushes, but it asks that before you
	// are done playing, that you put all the toys back where you found them.  So 
	// the next line of code first associates the HBRUSH with our HDC (remember you
	// can only have ONE brush associated with an HDC at a time).  Next it returns
	// to us the currently selected brush in the HDC (again by default Windows gives
	// us a brush).  We store that in "oldBrush", because when we are done using our
	// brush we need to put back the brush that was already there.
	oldBrush = (HBRUSH)SelectObject(gHDC,hbrush);
	assert(oldBrush != NULL); // If SelectObject() fails, it returns NULL

	// Draw a random rectangle from (0,0) to some a random (x,y) inside of
	// the window.  When drawing the rectangle, the inside area will get filled
	// with the brush we've selected.  The outside border will get drawn with 
	// the default pen which is a thin black pen.
	Rectangle(gHDC, 0, 0, rand()%gRect.right, rand()%gRect.bottom);

	// Okay we've drawn a rectangle, fantastic.  Now we get to be good
	// Windows children and put back all of our toys.  

	// First we put back the original brush associated with our HDC
	SelectObject(gHDC,oldBrush);

	// Now when we called CreateSolidBrush() a brush was dynamically allocated for us.
	// We need to free that memory up.  To do this we call DeleteObject()
	DeleteObject(hbrush);
}

/* 
	Brush with the law...

	Yes Windows has laws about brushes, and matter of factly, GDI (Graphic Device Interface) objects in
	general.  This general set of laws is the following:

		1)  Create the object
		2)  Select it into the device context you are using
		3)	Draw things
		4)  When finished, put back the object that was there before 

	If you follow those rules, you shouldn't get any ill effects or memory leaks when
	using GDI objects.

	If you have any questions or want to talk code, stop by the forums at www.GameTutorials.com.

*/

/*-------------------------*\
|  Programmed by:  TheTutor |
|  ©2005 GameTutorials, LLC |
\*-------------------------*/
