// Done by TheTutor

/*	
	This tutorial is all about receiving keyboard input in your Win32 application.
	Windows provides multiple ways to receive keyboard input, but this tutorial
	focuses on a method to get the keyboard input outside of the WinProc().

	For the app, we'll keep it pretty simple.  We're going to write a function that determines 
	if a key (specifically an alphabetic key) is pressed.  We'll check for both uppercase and 
	lowercase key presses.  If you press any of the keys found in "GameTutorials" you'll see
	"GameTutorials" printed to the screen in a random color.
	
	So without further ado, lets get coding.	
*/

#include "input.h"

////////////////
// Constants //
//////////////

const int kWinWid = 640;
const int kWinHgt = 480;

// Where to start drawing the text to
const int kTextX = 280; 
const int kTextY = 220;

const char kClassName[] = "GT_KeyboardInput2";

typedef unsigned char uchar; // Less typing is good %)

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
	wndclassex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); // Background color of white
	wndclassex.hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(IDC_ARROW), IMAGE_CURSOR, 
											0, 0, LR_SHARED); // Use default arrow cursor   

    RegisterClassEx(&wndclassex); // Register the window class so calls to CreateWindow()
								 // and CreateWindowEx() will know what window class to
								// use when creating the window
	
    hwnd = CreateWindowEx(NULL, // Extra window attributes, NULL means none
						  kClassName, // Name we gave our WNDCLASSEX			
						  "www.GameTutorials.com -- Keyboard Input Part2", // Title bar text
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
			// If any of the letters (case sensitive) in "GameTutorials" are pressed
			// Print to the screen "GameTutorials" in a random color
			if (KeyPressed('G') || KeyPressed('a') ||
				KeyPressed('m') || KeyPressed('e') ||
				KeyPressed('T') || KeyPressed('u') ||
				KeyPressed('t') || KeyPressed('o') ||
				KeyPressed('r') || KeyPressed('i') ||
				KeyPressed('l') || KeyPressed('s'))
			{
				// Get a random R, G, and B
				uchar red = rand()%256;
				uchar green = rand()%256;
				uchar blue = rand()%256;

				PrintColoredText(hwnd,"GameTutorials",red,green,blue);
			}

			Sleep(100); // Chill out for awhile man!
						// What Sleep() does is not do anything for the number of 
						// milliseconds passed to the function

		}

	}

	UnregisterClass(kClassName,hinstance); // Free up the memory allocated by our WNDCLASSEX
		return msg.wParam; // And we're out
}

// The WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	// We're not doing much here %)
    switch(message)
    {
		case WM_DESTROY: // If we should destroy the window, do so
                    
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

// The Shady Business of Dealing Keys

/*
	Luckily for us Windows makes it pretty painless to get keyboard info outside of
	the WinProc().  However it is important to note two other functions that can also
	get keyboard info:

	short GetKeyState(int virtualKey); // This function is basically exactly what we wrote
									   // You pass in a virtual key code and it returns a 
									   // short.  If the HIGH BIT on the return value is set
									   // to one, the key is pressed.  If the LOW BIT is set
									   // to one the key is "toggled".  Otherwise the key is
									   // not pressed.

	Now if you are writing some input functions for a GAME and speed is critical you want
	to use this:

	short GetAsyncKeyState(int virtualKey); // This function bypasses the normal "okay, a 
											// key was pressed, queue it up for handling" process
											// and gets you the info of a key's state 
											// asynchrounously (ie NOW).  Similar to the 
											// other functions, if the HIGH BIT is set to 
											// one the key is pressed.  This time though, if 
											// the LOW BIT is set to one, this means the key
											// was pressed after the previous call to 
											// GetAsyncKeyState()
	
	So if you're writing input code for a cool game and you could give a 
	monkey's behind about if the user entered a lowercase or uppercase letter, you 
	could simply write the KeyPressed() function like this:

	bool KeyPressed(char key) // Checks to see if a LETTER was pressed
	{
		key = toupper(key);

		return (GetAsyncKeyState(key) & 0x8000) == 0x8000;
	}


CHALLENGE:

	Write a "key pressed" function that returns true or false whether any alphabetic,
	numeric, or punctuation key on the keyboard is pressed -- The function should
	handle case	sensitive keys as well.

*/

// Questions, comments, rantings, ravings ???
//
// -- You know where to post:  www.GameTutorials.com

/*----------------------------*\
|  TheTutor                    |
|  thetutor@gametutorials.com  |
|  © 2000-2005 GameTutorials   |
\*----------------------------*/
