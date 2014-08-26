// Done by TheTutor

/*	
	Okay what we're going to do here is a little drawing using an HPEN.  Windows refers
	to all objects that can be used to draw things in a window collectively as GDI
	(Graphic Device Interface) objects.  To us, a HPEN is nothing more than a handle to a pen.
	So all that's really important to know up front is that we can draw stuff by using HPENs.
	But lets dig a bit deeper...
	
	In Win32 pens are used to draw lines.  So if we wanted to draw a rectangle to the 
	screen, using a Win32 API, the border line that defines the rectangle would
	be drawn by whatever style pen is associated with the window.  
	
	Pens are associated	with a device context.  A device context is a pointer that lets us
	talk to hardware devices, such as the graphics card, without knowing anything about the
	hardware device.  In this tutorial we will get the device context of our window and 
	associate different pens with it, drawing different rectangles to the screen.

	So without further ado, lets draw.

*/

#include <windows.h>
#include <assert.h>

////////////////
// Constants //
//////////////

const int kWinWid = 320; // Window's width
const int kWinHgt = 240; // Window's height

const char kClassName[] = "GameTutorials_HPENs"; // Name of the WNDCLASSEX

//////////////
// Globals //
////////////

HDC gHDC = NULL; // This will be our global device context
RECT gRect = {0}; // This will hold our window's client dimensions

// This function draws a random rectangle with a random pen
void DrawRandomRectangle();

// Standard callback function
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

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
											0, 0, LR_SHARED); // Use the default window arrow cursor

    RegisterClassEx(&wndclassex);	// Register the window
    
    hwnd = CreateWindowEx(NULL, // No extra window attributes
						  kClassName,
						  "www.GameTutorials.com -- HPENs",
						  WS_OVERLAPPEDWINDOW,
						  CW_USEDEFAULT, // Window will receive a default x pos on screen
						  CW_USEDEFAULT, // Window will receive a default y pos on screen
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
			return EXIT_FAILURE; // Couldn't get window's HDC
			
	// Get the windows client area
	GetClientRect(hwnd, &gRect);

	// Show and draw the window
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
			DrawRandomRectangle(); // What could this function do?
			Sleep(1000); // Wait for a second, draw another random rectangle
		}

	}

	UnregisterClass(kClassName,hinstance); // Free up WNDCLASSEX
		return msg.wParam;
}

// The WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	// Deal with the messages that we want to
    switch(message)
    {
		case WM_DESTROY: // Destroy the window
            
            ReleaseDC(hwnd,gHDC); // Free up the global HDC
			PostQuitMessage(0); // Post the WM_QUIT message
				return 0;

    } // end of switch(message)

    return DefWindowProc(hwnd, message, wparam, lparam);
}

void DrawRandomRectangle()
{
	// This is our pen object that we will allocate 
	// and use to draw our rectangle with.  So what is "oldPen"?  Well we
	// explain that in the next few lines...
	HPEN hpen = NULL;
	HPEN oldPen = NULL;
	
	// We want a different color each time we draw a rectangle so we
	// create a random color every time. 
	// *** NOTE ***	rand() % n returns a number between 0 and (n-1).  That's why
	//				we modulus rand() by 256 since color values are between 0 and 255.
	COLORREF color = RGB(rand()%256, rand()%256, rand()%256);
	
	// Windows has a few different functions for creating pens, but there's
	// nothing quite like the good old fashioned CreatePen().  Lets look
	// at each parameter:
	// PS_SOLID -- This is the style of pen we want.  This style means we want a solid,
	//			   fully drawn in, pen.  There are other styles that could be selected
	//			   such as PS_DASH that would draw a dashed line. 
	// 5 -- This is the width of the pen
	// color -- The color of the pen.
	hpen = CreatePen(PS_SOLID, 5, color);
	assert(hpen != NULL); // If CreatePen fails it returns NULL.
	
	// Okay so we have a newly created HPEN.  Yeah!  But now what?  Well we
	// need to tell the device context we want to draw with it about it.  Now when
	// you create a window, you get some things by default.  One of these 
	// things is a device context associated with the window.  On top of that,
	// the device context already has a default pen associated with it.  Windows
	// will allow you to color with other pens, but it asks that before you
	// are done playing, you put all the toys back where you found them.  So 
	// the next line of code first associares the HPEN with our HDC (remember you
	// can only have ONE pen associated with an HDC at a time).  Next it returns
	// to us the currently selected pen in the HDC (again by default we'll have a
	// pen).  We store that in "oldPen", because when we are done using our pen
	// we need to put back the pen that was already there.
	oldPen = (HPEN)SelectObject(gHDC,hpen);
	
	// Draw a random rectangle from (0,0) to some a random (x,y) inside of
	// the window
	Rectangle(gHDC, 0, 0, rand()%gRect.right, rand()%gRect.bottom);

	// Okay we've drawn a rectangle, fantastic.  Now we get to be good
	// Windows children and put back all of our toys.  

	// First we put back the original pen associated with our HDC
	SelectObject(gHDC,oldPen);

	// Then when we called CreatePen() a pen was dynamically allocated for us.
	// We need to free that memory up.  To do this we call DeleteObject()
	DeleteObject(hpen);
}

/* Penmanship...

	You will find that there is a common set of commands to use Windows GDI objects.
	It goes something like this:
	
		1)  Create the object
		2)  Select it into the device context you are using
		3)	Draw things
		4)  When finished, put back the object that was there before 

	If you follow those rules, you shouldn't get any ill effects or memory leaks when
	using GDI objects.
	
	Questions, comments, suggestions, or if you just want to talk code, stop by the 
	forums at www.GameTutorials.com
*/

/*-------------------------*\
|  Programmed by:  TheTutor |
|  ©2005 GameTutorials, LLC |
\*-------------------------*/
