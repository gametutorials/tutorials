// Done by TheTutor

/*	This tutorial is going to load a menu into the window we create, instead of 
	loading the menu into our WNDCLASSEX.

	Once we have a menu, we'll do a little bit of user interaction with it.  When it's all
	said and done, you should be one step closer to mastering menus.

	So enough of this introduction lets get a move on!
*/

// In Visual Studio 2005 Microsoft added safer versions (denoted by a "_s" suffix) to many 
// common C/C++ run time functions.  Currently the ISO Standards Committee for C and C++ 
// is determining if they are going to add them to the official language set.  Until then, 
// this define will remove warnings about using the "unsafe" versions of these functions.
#define _CRT_SECURE_NO_DEPRECATE

#include <windows.h>
#include <stdio.h>
#include "resource.h"

////////////////
// Constants //
//////////////

const char kClassName[] = "GT_MenusPart2"; // WNDCLASSEX name

const int kWinWid = 640; // The width of the window
const int kWinHgt = 480; // The height of the window

const int kXStart = 295; // The beginning x-pos for printing text to in our window
const int kYStart = 220; // The beginning y-pos for printing text to in our window

/////////////
// Macros //
///////////

// Since when we use strlen() for this app we want the result to be of 
// type "int" not "size_t", we'll use this handy dandy macro for typecasting
#define istrlen(x) (int)(strlen(x))

//////////////
// Globals //
////////////

HMENU gMenu = NULL; // Handle to the menu
HDC gHDC = NULL; // HDC of the window
RECT gRect = {0}; // Client RECT of the window

// WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

// WinMain
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprev, PSTR cmdline, int ishow)
{
	HWND hwnd = NULL;
    MSG msg = {0};
    WNDCLASSEX wndclassex = {0};

	// Fill the fields we care about
	wndclassex.cbSize = sizeof(WNDCLASSEX);
    wndclassex.style = CS_HREDRAW | CS_VREDRAW;
    wndclassex.lpfnWndProc = WinProc;
    wndclassex.hInstance = hinstance;
    wndclassex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclassex.lpszClassName = kClassName;
    wndclassex.hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(IDC_ARROW), IMAGE_CURSOR,
											0, 0, LR_SHARED);

	RegisterClassEx(&wndclassex); // Register our wndclassex
	
	// As the name suggests, LoadMenu() loads the menu.  
	// By parameter:
	// hinstance -- The HINSTANCE passed to by WinMain
	// MAKEINTRESOURCE(IDR_MENU1) -- If you look at the MenuScript.rc file you notice that by
	//								 default our menu got assigned the id "IDR_MENU1".  Now LoadMenu()
	//								 expects a string to be passed in as the identifier of the menu
	//								 MAKEINTRESOURCE() is a Win32 macro that coverts the int id we
	//								 received from the resource file into it's corresponding string
	//								 version that LoadMenu() can then use
	gMenu = LoadMenu(hinstance,MAKEINTRESOURCE(IDR_MENU1));

	// Create the window
	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
						  kClassName,
						  "www.GameTutorials.com -- Menus Part 2",
						  WS_OVERLAPPEDWINDOW,
						  CW_USEDEFAULT, 
						  CW_USEDEFAULT,
						  kWinWid,
						  kWinHgt,
						  NULL,
						  gMenu, // This is where we associate the "gMenu" with our window we
								 // are creating.  If we DID NOT have a menu we wanted to
								 // associate with this window, we'd put NULL here.
						  hinstance,
						  NULL);

		// Error Check
		if(!hwnd)
		{
			DestroyMenu(gMenu); // This frees the menu that we created
				return EXIT_FAILURE;
		}
		
	// Get the HDC of the window
	gHDC = GetDC(hwnd);
	
		// Error check
		if(!gHDC)
			return false;
			
	// Get the client area of the window
	GetClientRect(hwnd, &gRect);
	
	// Show and update window
    ShowWindow(hwnd, ishow);
    UpdateWindow(hwnd);

	while(1) // Loop until app is finished
	{
		// Check the OS for messages
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
				break;
			
			// Translate messages and dispatch them to the WinProc()	
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// The rest of our program would go here...
		} 

	} // end of while(1)
	
	UnregisterClass(kClassName,hinstance); // The frees memory associated with WNDCLASSEX
		return int(msg.wParam);
}

// The WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    char buff[256] = {0}; // A character string for "filling" with text to print
		
	switch(message)
    {
		// This message gets sent if a "command item" from a menu is selected 
		// (it also gets sent for other stuff but this is what we care about)
		case WM_COMMAND:
		
			switch(LOWORD(wparam)) // The LOWORD(wparam) will equal the "identifier" of
			{					   // of whatever was selected -- These are #defined in 
								   // resource.h
								   // **NOTE**  If using VC++6.0 these are given to you when you
								   // "make a resource script"  So I didn't come up with
								   // these names (I could change 'em) but I'm using the
								   // defaults

				
				case ID_QUIT: // If they select "Quit" -- quit the program
				
					SendMessage(hwnd, WM_CLOSE, 0, 0); // Send the message that tells Windows to
						break;						  // close the window
						
				
				// The user selected "Green" under the "PrintText" menu
				case ID_PRINTTEXT_GREEN:

					// This fills the rect we just got completely to white
					// (effectively "erasing" any text that may have previously been there)
					FillRect(gHDC,&gRect,(HBRUSH)GetStockObject(WHITE_BRUSH));

					sprintf(buff,"This is green"); // Fill buff with appropriate message

					SetTextColor(gHDC,RGB(0,200,0)); // Set the text color to green
					TextOut(gHDC,kXStart,kYStart,buff,istrlen(buff)); // Print out the "green text"
						break;

				// The user selected "Blue" under the "PrintText" menu
				case ID_PRINTTEXT_BLUE:

					// This fills the rect we just got completely to white
					// (effectively "erasing" any text that may have previously been there)
					FillRect(gHDC,&gRect,(HBRUSH)GetStockObject(WHITE_BRUSH));

					sprintf(buff,"This is blue"); // Fill buff with appropriate message

					SetTextColor(gHDC,RGB(0,0,200)); // Set the text color to blue
					TextOut(gHDC,kXStart,kYStart,buff,istrlen(buff)); // Print out the "green text"
						break;

				// The user selected "Bright" off of the "Red" under the "PrintText" menu
				case ID_PRINTTEXT_RED_BRIGHT:

					// This fills the rect we just got completely to white
					// (effectively "erasing" any text that may have previously been there)
					FillRect(gHDC,&gRect,(HBRUSH)GetStockObject(WHITE_BRUSH));

					sprintf(buff,"This is bright red"); // Fill buff with appropriate message

					SetTextColor(gHDC,RGB(250,45,45)); // Set the text color to bright red
					TextOut(gHDC,kXStart,kYStart,buff,istrlen(buff)); // Print out "bright red text"
						break;

				// The user selected "Dark" off of the "Red" under the "PrintText" menu
				case ID_PRINTTEXT_RED_DARK:

					// This fills the rect we just got completely to white
					// (effectively "erasing" any text that may have previously been there)
					FillRect(gHDC,&gRect,(HBRUSH)GetStockObject(WHITE_BRUSH));

					sprintf(buff,"This is dark red"); // Fill buff with appropriate message

					SetTextColor(gHDC,RGB(140,10,30)); // Set the text color to dark red
					TextOut(gHDC,kXStart,kYStart,buff,istrlen(buff)); // Print out "dark red text"
						break;

			} // end of switch(LOWORD(wparam))
			
			return 0;
		
		case WM_DESTROY:

			ReleaseDC(hwnd, gHDC); // Free up the global HDC
			DestroyMenu(gMenu); // This frees the menu that we created
			PostQuitMessage(0); // Post message to quit application
				return 0;
    
	} // end of switch(message)

    return DefWindowProc(hwnd, message, wparam, lparam);
}

// What's on the menu?

/*
	Menus are another handy dandy tool Windows provides to us for accepting user input.

	Of course they have their limits and if you want the user to be able to input numbers,
	strings, or do something like click on radio buttons, then you really should be
	using dialog boxes not menus.

	Be sure to hit up the forums at www.GameTutorials.com if you have any questions.
*/

/*-------------------------*\
|  Programmed by:  TheTutor |
|  ©2000-2005 GameTutorials |
\*-------------------------*/
