// Done by TheTutor

/*	
	This tutorial is all about receiving mouse input in your Win32 application.  Specifically
	we'll go over how to get the current position of the mouse.  We'll get this information
	in the WinProc() by checking the WM_MOUSEMOVE message.

	The application is pretty simple.  Basically every time the mouse moves, we'll display
	it's current (x,y) position of the mouse in the center of the screen.  That's it!
	Nothing more and nothing less.  So without further ado, lets get coding.	
	
*/

// In Visual Studio 2005 Microsoft added safer versions (denoted by a "_s" suffix) to many 
// common C/C++ run time functions.  Currently the ISO Standards Committee for C and C++ 
// is determining if they are going to add them to the official language set.  Until then, 
// this define will remove warnings about using the "unsafe" versions of these functions.
#define _CRT_SECURE_NO_DEPRECATE

#include <windows.h>
#include <stdio.h>

////////////////
// Constants //
//////////////

const int kWinWid = 320; // Width of the window
const int kWinHgt = 240; // Height of the window

const char kClassName[] = "GT_MouseInput"; // WNDCLASSEX's name  

// This function will print the mouse coordinates passed in, centered in the window
void PrintMouseCoords(HWND hwnd, int xPos, int yPos);

// Standard callback function
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprev, PSTR cmdline, int ishow)
{
	HWND hwnd; // This will hold the handle to our window
    MSG msg; // This will hold any messages (such as mouse movement) that
			 // our window might receive

    WNDCLASSEX wndclassex = {0}; // This is our "window class" -- The "EX" stands
								// for "extended style" which gives us more options
							   // when creating our window (although we're going to
							  // completely ignore 'em)

	// Fill the fields we care about
	wndclassex.cbSize = sizeof(WNDCLASSEX); // Always must be set
    wndclassex.style = CS_HREDRAW | CS_VREDRAW; // Window classes style
    wndclassex.lpfnWndProc = WinProc; // Pointer to where the WinProc() is defined
    wndclassex.hInstance = hinstance; // The handle to the instance of our window
	wndclassex.lpszClassName = kClassName; // The name of our window class
	wndclassex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); // Make the background white
	wndclassex.hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(IDC_ARROW), IMAGE_CURSOR,
											0, 0, LR_SHARED); // Use the default arrow cursor
    
    RegisterClassEx(&wndclassex); // Register the window class so calls to CreateWindow()
								 // and CreateWindowEx() will know what window class to
								// use when creating the window
	
    hwnd = CreateWindowEx(NULL, // Extra window attributes, NULL means none
						  kClassName, // Name we gave our WNDCLASSEX			
						  "www.GameTutorials.com -- Mouse Input", // Text for the title bar
						  WS_OVERLAPPEDWINDOW, // Style of window (see MSDN for full description)
						  CW_USEDEFAULT, // Upper left xPos of window (Windows picks default)
						  CW_USEDEFAULT, // Upper left yPos of window (Windows picks default)
						  kWinWid, // Width of window in pixels
						  kWinHgt, // Height of window in pixels
						  NULL, // Handle to "parent window" (we have none, this is the parent)
						  NULL, // Handle to a menu (we have none)
						  hinstance, // Handle to instance of this window (passed in by WinMain())
						  NULL); // "Extra info" to pass to the WinProc (we have none)

		// Error check
		if(!hwnd)
			return EXIT_FAILURE; // Something really bad happened!

    ShowWindow(hwnd, ishow); // Show the window (make it visible)
    UpdateWindow(hwnd); // Updates the window (initially draw the window)
   
    while(1)
	{
		// Get message(s) (for instance a mouse movement) if there is any
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
				break;
				
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// Do all the hardcore computational stuff here :)
		}
	}

	UnregisterClass(kClassName,hinstance); // Free up the memory allocated by our WNDCLASSEX
		return (int)msg.wParam; // And we're out
}

// The WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	// Depending on the message -- we'll do different stuff
    switch(message)
    {
		// This is the message that is posted to a window when
		// the mouse moves.  For a window NOT to receive this message
		// when the mouse is moved a different window would have to 
		// currently own the "mouse movement" (a different window would 
		// have to be in focus) and then this message would be posted to 
		// that window
		case WM_MOUSEMOVE: 
		{
			// Here's a quick break down of the information we can get from this
			// message

			int flags = wparam; // "flags" contains flags that say whether certain
							   // virtual keys (such as the CTRL key) are being pushed or not
							  // More info on this is supplied at the bottom of this file

			int xPos = LOWORD(lparam);  // This gives us the horizontal (the X position) of
									   // the cursor

			int yPos = HIWORD(lparam);  // This gives us the vertical (the Y position) of
									   // the cursor
			
			PrintMouseCoords(hwnd,xPos,yPos); // Print 'em to the screen
				return 0;

			// **NOTE**
			//
			// You can also create a POINTS struct that is a Win32 struct that contains
			// an 'x' and 'y' which are both defined to be SHORT's (16-bit integers)
			// by calling this macro:
			//
			//	POINTS point = MAKEPOINTS(lparam); // "point" will equal the (x,y) position
			//									  // of the mouse
		}
	
        case WM_DESTROY: // If it's time to destroy the window, do so
            
			PostQuitMessage(0);
             return 0;

    } // end of switch(message)

    return DefWindowProc(hwnd, message, wparam, lparam);
}

// Prints the X-position and Y-position, with appropriate labels, centered
// in the window
void PrintMouseCoords(HWND hwnd, int xPos, int yPos)
{
	HDC hdc = GetDC(hwnd); // Get the windows device context

		// Error Check
		if(!hdc)
			return; // Can't fill the window without an HDC, we'll just return

	RECT rect;
	GetClientRect(hwnd,&rect); // Get the window's client area RECT

	// Clear the window (the client area of the window) to all white
	FillRect(hdc,&rect,(HBRUSH)GetStockObject(WHITE_BRUSH));

		// **NOTE**
		//
		// What EXACTLY is the client area of a window?  Well when you think 
		// of a window, you should think of two RECT's that describe that window.  The first
		// is the WINDOW RECT.  This, as the name implies, encompasses the ENTIRE window.
		// The second is the CLIENT RECT.  This is the area of the window where "stuff" is
		// actually drawn.  The client area EXCLUDES the title bar and thin borders around the
		// window.  It's upper left corner always starts at (0,0) and is relative to the window.
		// For this app, the client rect is the rectangular area that's pure white and has
		// the mouse coordinates being displayed in it.

	char buffX[128] = {0}; // char buffer for the X position
	char buffY[128] = {0}; // char buffer for the Y position

	// Fill both buffers with the text
	sprintf(buffX,"X Pos = %d",xPos);
	sprintf(buffY,"Y Pos = %d",yPos);

	// Now here's where you math teacher turns out to be right!  To display the 
	// text horizontally centered in the window, we're going to have to use a little algebra.
	// So lets think about this.  We basically want half of the text to the left of 
	// the center of the window and half of the text to the right of the center of 
	// the window so overall it will look perfectly centered.  Well that gives us this 
	// equation:
	//
	//  xStart = centerX - (lengthOfText / 2)
	//  yStart = Whatever we choose, all we care is that the text is centered horizontally
	// 
	// But that won't quite do it.  We forgot one thing, the width of each letter of 
	// text.  So when we add that into the equation we get this:
	//
	//  xStart = centerX - ((lengthOfText / 2) * widthOfACharacter)
	//  yStart = Whatever we choose, all we care is that the text is centered horizontally
	//
	// Okay we have our equation, so now lets fill in the variables

	// First, we'll get the center of the window
	// Notice how we use the CLIENT rect to get the center (x,y) of the window
	int centerX = rect.right / 2;
	int centerY = rect.bottom / 2;

	// Next we'll get the "width of the text".  To do this we'll fill a TEXTMETRIC struct.
	// A TEXTMETRIC is a Win32 struct that holds a whole crap load of information about 
	// a physical font.
	TEXTMETRIC textmetric;

	GetTextMetrics(hdc,&textmetric); // This fills "textmetric" with a bunch of information
									// about the current font.  Now since we didn't set
								   // the font to anything in particular, this will be 
								  // filled with the "default font's" info.
	
	// We'll draw the "Mouse's x position first" so using our equation lets calculate
	// it's starting x-pos for drawing to the window.  You'll notice we use
	// the "tmAveCharWidth" variable of the TEXTMETRIC struct.  This gives us the average
	// width of a character drawn, which is just what we need.
	int start = centerX - ((strlen(buffX) / 2) * textmetric.tmAveCharWidth);

	// Okay now it's time to actually write out the text to the screen
	// We'll draw this slightly above the vertical center of the screen,
	// hence the (centerY - 25)
	TextOut(hdc, start, centerY - 25, buffX, strlen(buffX));

	// Now it's time to draw out the "Mouse's y position" so lets recalculate our starting
	// position
	start = centerX - ((strlen(buffY) / 2) * textmetric.tmAveCharWidth);
	
	// Draw it to the screen -- We'll draw this slightly below the vertical center of 
	// the window
	TextOut(hdc, start, centerY + 25, buffY, strlen(buffY));

	ReleaseDC(hwnd,hdc); // Be sure to always free up your HDC

}

// More Mouse Movement --------------

/*	As promised, here's more on the "flags" that are filled every time the 
	WinProc() receives a WM_MOUSEMOVE message.  It's important to
	note that "flags" (the WPARAM value) could contain any combination of 
	the following:

	MK_CONTROL		Set if the ctrl key is down. 
	MK_LBUTTON		Set if the left mouse button is down. 
	MK_MBUTTON		Set if the middle mouse button is down. 
	MK_RBUTTON		Set if the right mouse button is down. 
	MK_SHIFT		Set if the shift key is down.

	So to determine if, say for instance, the crtl key is down, we would simply utilize our
	old friends the binary operators and do something like this:
 
	if(flags & MK_CONTROL)
		// Do stuff because ctrl key is down

	Well that's all she wrote for this chapter on mouse movement.  As always, any questions 
	can be directed to the forums at www.GameTutorials.com

*/

/*----------------------------*\
|  TheTutor                    |
|  thetutor@gametutorials.com  |
|  © 2000-2005 GameTutorials   |
\*----------------------------*/
