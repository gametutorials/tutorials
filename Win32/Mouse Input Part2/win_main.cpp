// Done by TheTutor

/*	
	Here we'll take a second look at obtaining mouse input in your Win32 app.  Specifically
	we'll go over how to check the left mouse button, right mouse button, and mouse wheel
	for input.

	Here's the short list of what our program will do:

	1) While the left mouse button is held down, the text "Left MB" will appear
	   in the center of the screen
	2) While the right mouse button is held down, the text "Right MB" will appear 
	   in the center of the screen
	3) Scrolling the mouse wheel will change the color of the text for both the left
	   and right mouse button messages.

	Simple right?  Right!?!  You bet it will be.  So lets get on with the show
	
*/

// Okay first things first.  We have to cover up a MS boo-boo.  If you read MSDN it says 
// that you can program for the mouse wheel as long as you have 98 and up.  Well it turns
// out that this simply isn't true.  Depending on your OS/compiler you may have to 
// define the mouse wheel yourself.
#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL 522 // Value plucked from Winuser.h
#endif

#include <windows.h>

////////////////
// Constants //
//////////////

const int kWinWid = 320;
const int kWinHgt = 240;

// The Y-pos for the display the left mouse button text and the right mouse
// button text
const int kYPosLB = 80;
const int kYPosRB = 120;

const char kClassName[] = "GT_MouseInput2";

// This function prints text to the screen, horizontally centered in the window
// with the passed in color, stating vertically at "yPos"
void TextOutCentered(HWND hwnd, char *text, int yPos, COLORREF color);

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
											0, 0, LR_SHARED); // Load the default arrow cursor
    
    RegisterClassEx(&wndclassex); // Register the window class so calls to CreateWindow()
								 // and CreateWindowEx() will know what window class to
								// use when creating the window
	
    hwnd = CreateWindowEx(NULL, // Extra window attributes, NULL means none
						  kClassName, // Name we gave our WNDCLASSEX			
						  "www.GameTutorials.com -- Mouse Input Part2", // Text in title bar
						  WS_OVERLAPPEDWINDOW, // Style of window
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

	UnregisterClass(kClassName,hinstance); // Free up the memory associated with the WNDCLASSEX
		return msg.wParam; // And we're out
}

// The WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	static unsigned char red, green, blue; // Our text colors

	// Depending on the message -- we'll do different stuff
    switch(message)
    {
		case WM_CREATE:

			// Set our text color initially to solid black
			red = green = blue = 0;
				return 0;

		// This is the message that is sent when the LEFT mouse button is pressed
		// while the cursor is in the CLIENT AREA (the area of the window that excludes
		// the title bar and window borders) of the window.  So when we receive this
		// message we'll draw the "Left MB" text to the screen
		case WM_LBUTTONDOWN:

			TextOutCentered(hwnd,"Left MB",kYPosLB,RGB(red,green,blue));
				return 0;

		// This is the message that is sent when the LEFT mouse button is released
		// while the cursor is in the CLIENT AREA of the window. So when we receive 
		// this message we're going to be really sneaky and display the same text
		// only using the color (255,255,255), which since that's what we set our 
		// windows background color to, will make it look like the text was erased
		case WM_LBUTTONUP:

			TextOutCentered(hwnd,"Left MB",kYPosLB,RGB(255,255,255));
				return 0;

		// This is the message that is sent when the RIGHT mouse button is pressed
		// while the cursor is in the CLIENT AREA (the area of the window that excludes
		// the title bar and window borders) of the window.  So when we receive this
		// message we'll draw the "Right MB" text to the screen
		case WM_RBUTTONDOWN:

			TextOutCentered(hwnd,"Right MB",kYPosRB,RGB(red,green,blue));
				return 0;

		// This is the message that is sent when the RIGHT mouse button is released
		// while the cursor is in the CLIENT AREA of the window. So when we receive 
		// this message we're going to be really sneaky and display the same text
		// only using the color (255,255,255), which since that's what we set our 
		// windows background color to, will make it look like the text was erased
		case WM_RBUTTONUP:

			TextOutCentered(hwnd,"Right MB",kYPosRB,RGB(255,255,255));
				return 0;

		// This is the message that is sent when the mouse wheel is rotated.  This 
		// message will be sent to the window with the current focus.  Lets get
		// the information that we care about from this message
		case WM_MOUSEWHEEL:
		{
			int flags = LOWORD(wparam); // The LOWORD of the WPARAM holds info on whether
									   // various virtual keys are down (like the CRTL key)
									  // or not -- More info about the "flags" is provided
									 // at the bottom of this file

			// The HIWORD of the WPARAM holds info on the
			// distance that the wheel is rotated, expressed in
			// multiples or divisions of WHEEL_DELTA, which is
			// 120. A positive value indicates that the wheel was
			// rotated forward, away from the user; a negative
			// value indicates that the wheel was rotated backward,
			// toward the user -- Check out the bottom of this file for
			// more info on using WHEEL_DELTA
			short delta = (short)HIWORD(wparam); 

			// All we're going to worry about is if the mouse wheel
			// was rolled forwards or backwards
			// If it's rolled forward, we'll increment the color by 5
			// If it's rolled backward, we'll decrement the color by 5
			int amount = (delta >= 0) ? 5 : -5;

			// If the CRTL key was being held
			if(flags & MK_CONTROL)
				red += amount; // Change the red color component by "amount"

			else if(flags & MK_SHIFT) // Else if the SHIFT key was being held
				green += amount; // Change the green color component by "amount"

			else // Else just modify the blue component by amount
				blue += amount;

			// If the left mouse button is down
			if(flags & MK_LBUTTON)
				TextOutCentered(hwnd,"Left MB",kYPosLB,RGB(red,green,blue));

			// If the right mouse button is down
			if(flags & MK_RBUTTON)
				TextOutCentered(hwnd,"Right MB",kYPosRB,RGB(red,green,blue));


			return 0;

			// **NOTE**
			//
			// Additionally the following information can be obtained from this message:
			//
			// int xPos = LOWORD(lparam); // horizontal position of cursor
			//
			// int yPos = HIWORD(lparam); // vertical position of cursor
		}
	
        case WM_DESTROY: // Destroy the window
            
			PostQuitMessage(0); // Quit the application
             return 0;

    } // end of switch(message)

    return DefWindowProc(hwnd, message, wparam, lparam);
}

// This function does most of dirty work.  It will print "text" to the 
// screen horizontally centered staring at "yPos".  The text will also
// be the same color as the COLORREF passed in.
void TextOutCentered(HWND hwnd, char *text, int yPos, COLORREF color)
{
	HDC hdc = GetDC(hwnd); // Get the windows device context

		// Error Check
		if(!hdc)
			return;

	RECT rect;
	GetClientRect(hwnd,&rect); // Get the window's client area RECT
	
	TEXTMETRIC textmetric; // struct for holding information about fonts
	GetTextMetrics(hdc,&textmetric); // Fill the TEXTMETRIC with information about
									// our window's font

	// Set the text color to the color passed in
	SetTextColor(hdc,color);
	
	// Calculate the staring X-pos to begin displaying the text
	// (rect.right / 2) equals the "center X" of the window
	int xPos = (rect.right / 2) - ((strlen(text) / 2) * textmetric.tmAveCharWidth);

	// Draw the text to the screen
	TextOut(hdc, xPos, yPos, text, strlen(text));

	ReleaseDC(hwnd,hdc); // Be sure to always free up your HDC

}

// Wheel Of Unfortunate...

/*
	Unfortunately, you have to potentially do something special to get 
	the mouse wheel to work (yes you can blame this on MS :)

	So lets look at the code we use for WM_MOUSEWHEEL and what it's doing:

		#ifndef WM_MOUSEWHEEL // This says "Okay if what MS says is supposed to be
							  // defined isn't, lets do something about it"
		#define WM_MOUSEWHEEL 522 // We define the value to what it's suppose to equal
								  // We get this from from Winuser.h
		#endif

    If you wanted to use the WHEEL_DELTA value, you would have to write code similar to what's 
	above.  WHEEL_DELTA indicates the distance that the wheel is rotated, expressed
	in multiples or divisions of WHEEL_DELTA, which is 120.  So if you wanted to use WHEEL_DELTA
	set it to 120. 

	If you are interested to why this just doesn't work,  open up winuser.h (just search for 
	it on your computer) and then inside the file search for WM_MOUSEWHEEL.  You'll see the
	code preventing this from working.  I would HIGLY recommend NOT changing winuser.h.
	Just do the fix as it's done in	this tutorial and all is well :)

	When dealing with messages in the WinProc() of any type, it's important to know
	exactly what they do.  So for an exercise try (for example) pressing and holding the
	left mouse button, moving the cursor completely off the window, and releasing.
	See if you can get the app to show/erase the text NO MATTER where your mouse is.

	*HINT* look up WM_NCLBUTTONDOWN (and others similar) in MSDN

	Any other questions or concerns, you know where to post -- www.GameTutorials.com
*/

/*-------------------------*\
|  Programmed by:  TheTutor |
|  ©2005 GameTutorials, LLC |
\*-------------------------*/
