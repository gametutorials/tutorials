// Done by TheTutor

/*	
	This tutorial is all about receiving keyboard input in your Win32 application.
	Windows provides multiple ways to receive keyboard input, but this tutorial
	focuses on a method to get the keyboard input inside of the WinProc().

	What we're going to do is pretty simple.  If you press the 'R' key we'll print "RED" to
	the window in the color red.  If you press the 'G' key, we'll print "GREEN" to the window
	in the color green.  And I bet you get the pattern for the 'B' key :)

	So without further ado, lets get coding.
*/

#include <windows.h>

////////////////
// Constants //
//////////////

// Width and height of window
const int kWinWid = 640;
const int kWinHgt = 480;

// Where to begin drawing the text to
const int kTextX = 300;
const int kTextY = 220;

const char kClassName[] = "GameTutorials_HDC"; // WNDCLASSEX's name 

// Save us a little typing %)
// As a quick reminder, typedef allows you to define your own 
// type of variable as another type of variable.  So now "uchar" is equivalent
// to "unsigned char".
typedef unsigned char uchar;

// This function will print to the screen the text passed in, using
// the RGB color passed in
void PrintColoredText(HWND hwnd, char *text, uchar R, uchar G, uchar B);

// Standard callback function
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprev, PSTR cmdline, int ishow)
{
	HWND hwnd; // This will hold the handle to our window
    MSG msg; // This will hold any messages (such as a key press) that
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
	wndclassex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); // This sets
																   // the background color
																  // of the window to white
	wndclassex.hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(IDC_ARROW), IMAGE_CURSOR,
											0, 0, LR_SHARED); // This loads the default arrow
															 // cursor as the cursor for the window    

    RegisterClassEx(&wndclassex); // Register the window class so calls to CreateWindow()
								 // and CreateWindowEx() will know what window class to
								// use when creating the window
	
    hwnd = CreateWindowEx(NULL, // Extra window attributes, NULL means none
						  kClassName, // Name we gave our WNDCLASSEX			
						  "www.GameTutorials.com -- Keyboard Input", // Text for the title bar
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
		// Get message(s) (for instance a key press) if there is any
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
		// This is the message where all the magic happens.  A WM_CHAR message
		// gets sent every time WM_KEYDOWN message is translated by the
		// TranslateMessage() function.  So what does that mean?  Okay see in our WinMain()
		// how we have the function call TranslateMessage(&msg).  You HAVE to have
		// this call in WinMain() (with a similar type set up we are using here)
		// for you to receive the WM_CHAR message in your WinProc().

		case WM_CHAR:
		{
			// Okay the first thing is what in the heck is a TCHAR?
			// For all intensive purposes, a TCHAR == char (the normal C/C++ char)
			// On a UNICODE machine it actually equals something else, but either
			// way you can think of it as just a good ole' char

			// Now for the WM_CHAR message, the WPARAM equals the "character code"
			// that was pressed.  You can think of the WPARAM equaling the ASCII 
			// representation of the key that was pressed.
			
			TCHAR tchar = (TCHAR)wparam; // Get the key that was pressed.

			// Now lets check for the 'R', 'G' or 'B' key

			// Notice how we check for BOTH upper and lowercase so not 
			// to miss one case or the other
			if(tchar == 'r' || tchar == 'R')
				PrintColoredText(hwnd,"RED",255,0,0);

			else if(tchar == 'g' || tchar == 'G')
				PrintColoredText(hwnd,"GREEN",0,255,0);

			else if(tchar == 'b' || tchar == 'B')
				PrintColoredText(hwnd,"BLUE",0,0,255);

			return 0;
		
		}
	
		// Destroy the window when it's time
        case WM_DESTROY:
            
			PostQuitMessage(0);
             return 0;

    } // end of switch(message)

    return DefWindowProc(hwnd, message, wparam, lparam);
}

// Fill the window color to the RGB color passed in
void PrintColoredText(HWND hwnd, char *text, uchar red, uchar green, uchar blue)
{
	// Error Check
	if(!text)
		return; // If "text" isn't valid, we'll just return

	HDC hdc = GetDC(hwnd); // Get the windows device context

		// Error Check
		if(!hdc)
			return; // Can't fill the window without an HDC, we'll just return

	RECT rect;
	GetClientRect(hwnd,&rect); // Get the window's client area RECT

	// Clear the client area of the window to all white
	FillRect(hdc,&rect,(HBRUSH)GetStockObject(WHITE_BRUSH));

	// This Win32 API does just what you expect, it sets the text color for the
	// specified device context to the RGB color value passed
	SetTextColor(hdc,RGB(red,green,blue));

	// This function prints text to the window -- Lets break it down by parameter:
	// hdc == The device context to draw to (here hdc == "our window")
	// kTextX == The Window X coordinate to begin drawing the text to
	// kTextY == The Window Y coordinate to begin drawing the text to
	// text == a char* (Windows defines this variable as a LPCSTR, but really that's just
	//		   the same as a char*) that contains the string to be displayed
	// strlen(text) == The number of characters in the string
	TextOut(hdc,kTextX,kTextY,text,strlen(text));

	ReleaseDC(hwnd,hdc); // Be sure to always free up your HDC

	/***** 
	
	  NOTE:

		In SetTextColor() we use the macro RGB()

		RGB() is a Win32 macro that makes a COLORREF.  A COLORREF is a 32-bit value
		that specifies an RGB color.  A COLORREF looks like this in hex:

		0x00bbggrr 

		With bb == the blue component, gg == the green component, and rr == red component
		which are each an unsigned value between 0 - 255

	*****/
}

/*
	More on WM_CHAR ---

	You probably noticed that we did not do ANYTHING with the LPARAM part of the
	WM_CHAR message.  Well it's not that it doesn't include any information.  On the 
	contrary, it's chucked full with useful information.  So here's the table of
	information you could get out of the LPARAM if it tickled your fancy.

	This following is the BIT values breakdown of the 32-bit LPARAM -- Be sure to check
	MSDN for any changes

	0 - 15	Specifies the repeat count for the current message. The value is the number of
			times the keystroke is auto-repeated as a result of the user holding down the key

	16 - 23	Specifies the scan code. The value depends on the original equipment
			manufacturer (OEM)
			
	   24	Specifies whether the key is an extended key, such as the right-hand alt and 
			ctrl keys that appear on an enhanced 101-key or 102-key keyboard. The value is 1 
			if it is an extended key otherwise it's 0
			
	25 - 28 Reserved

	   29	The context code. The value is 1 if the alt key is held down while 
			the key is pressed otherwise it's 0
			
	   30	The previous key state. The value is 1 if the key is down
			before the message is sent, or it is 0 if the key is up

	   31	The transition state. The value is 1 if the key is being released, 
			or it's 0 if the key is being pressed
			
			
	For further assistance be sure to check out the forums at www.GameTutorials.com  

*/

/*----------------------------*\
|  TheTutor                    |
|  thetutor@gametutorials.com  |
|  © 2000-2005 GameTutorials   |
\*----------------------------*/
