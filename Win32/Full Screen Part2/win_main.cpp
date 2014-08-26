//	Done by TheTutor

/*
	Inside of this tutorial we are going to switch to and from full screen with a menu present.
	To do this we will have to do a couple of things.  First we'll need to get rid of the 
	menu when full screened and put it back when in windowed mode.  Next, we'll need to
	dynamically update the style of the window depending on if we are full screen or
	windowed.
	
	There are two ways to change to full screen.  Either select "On" under the full screen
	menu or press the F1 key. 

	Okay we know what we want to do, so lets go do it!
*/

#include <windows.h>
#include "resource.h"

////////////////
// Constants //
//////////////

const int kWinWid = 800; // Width of window
const int kWinHgt = 600; // Height of window

const char kClassName[] = "GameTutorials_FullScreen";

// The window's extended style and regular style
const DWORD kExStyle = WS_EX_CLIENTEDGE;
const DWORD kStyle = WS_SYSMENU | WS_OVERLAPPED | WS_CAPTION;

//////////////
// Globals //
////////////

HMENU gMenu = NULL; // A handle to our menu

// A boolean flag that tracks whether we're in full screen mode (true)
// or in windowed mode (false).  We start in windowed mode, so we
// initialize the variable to false
bool gFullScreen = false;

// WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

// This is the function that will switch us to and from full screen mode.
// The first parameter is the handle to window to toggle to/from full screen.
// The second parameter, "toFullScreen", says whether we want to change to full
// screen mode (true) or if we want to change to windowed mode (false).
bool ToggleFullScreen(HWND hwnd, bool toFullScreen);

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprev, PSTR cmdline,	int ishow)
{
    HWND hwnd;
    MSG msg;
    WNDCLASSEX wndclassex = {0};
	
	// Fill the WNDCLASSEX fields we care about
	wndclassex.cbSize = sizeof(WNDCLASSEX);
    wndclassex.style = CS_HREDRAW | CS_VREDRAW;
    wndclassex.lpfnWndProc = WinProc;
    wndclassex.hInstance = hinstance;
    wndclassex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); // Make the window background black
    wndclassex.lpszClassName = kClassName;
    wndclassex.hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(IDC_ARROW), IMAGE_CURSOR,	
											0, 0, LR_SHARED); // Load the default arrow cursor
	
	// Load our menu
	gMenu = LoadMenu(hinstance,MAKEINTRESOURCE(IDR_MENU1));
	
		// Error check
		if(!gMenu)
			return EXIT_FAILURE;

    RegisterClassEx(&wndclassex); // Register the WNDCLASSEX with the OS

    hwnd = CreateWindowEx(kExStyle, // Window should have a sunken edge
						  kClassName,
						  "www.GameTutorials.com -- Full Screen via a Menu",
						  kStyle,
						  CW_USEDEFAULT, // Windows chooses upper-left X-Pos of the window
						  CW_USEDEFAULT, // Windows chooses upper-left Y-Pos of the window
						  kWinWid,
						  kWinHgt,
						  NULL,
						  gMenu, // The Win32 menu to associate with the window
						  hinstance,
						  NULL);

	// Show and update window
    ShowWindow(hwnd, ishow);
    UpdateWindow(hwnd);

	while(1)
	{
		// Get messages if we have any
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
				break;
				
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// If we wanted to, we could do important things here...			
		}

	} // end of while(1)

	DestroyMenu(gMenu); // Free up menu
	UnregisterClass(kClassName,hinstance); // Free up WNDCLASSEX
		return msg.wParam;
}

LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
 	switch(message)
    {
		// If somebody hits a key
		case WM_KEYDOWN:
		
			// If they press F1 we want to toggle to/from full screen mode.
			// To accomplish this, just pass in the opposite flag of whatever mode
			// we're currently in
			if(wparam == VK_F1)
				ToggleFullScreen(hwnd, !gFullScreen);
				
			return 0;
    
		// If somebody clicks the menu...
		case WM_COMMAND:

			// If the user selected to change to full screen mode
			if(LOWORD(wparam) == ID_FULLSCREEN_ON)
			{
				// Try and change to full screen mode.  If we can't give us
				// a warning, something must be up!
				if(!ToggleFullScreen(hwnd,true))
					MessageBox(hwnd,"Couldn't change to full screen mode!","Warning",MB_OK);
			}
				
			return 0;
		
		case WM_DESTROY:

			ToggleFullScreen(hwnd, false); // Make sure we back in window mode before we quit 
			PostQuitMessage(0);
				return 0;
    
	} // end of switch(message)

    return DefWindowProc(hwnd, message, wparam, lparam);
}

// This is what actually changes the screen resolution
bool ToggleFullScreen(HWND hwnd, bool toFullScreen)
{
	// If we're trying to switch to full screen mode
	if(toFullScreen == true)
	{
		// If we're trying to switch to full screen but we are already
		// in full screen mode, don't do anything 
		if(gFullScreen == true)
			return true; // We didn't do anything, but we didn't encounter an error either 
		
		// A DEVMODE is a huge struct that contains some data we want
		// about our display device 
		DEVMODE devMode = {0};
		
		// The first step when changing to full screen is that we need to query 
		// the display and get all of it's current information.  We can do
		// that with the EnumDisplaySettings() function.
		// By parameter:
		// NULL -- Pointer to the display device to get information from.  Passing
		//		   NULL means get information from the current display device.
		// ENUM_CURRENT_SETTINGS -- A flag that specifies what settings we want to 
		//							obtain from the display device.  This flag says to
		//							get whatever the current settings for the display device are.
		// &devMode -- Address of a DEVMODE structure to fill with the display settings.
		if(!EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&devMode))
			return false; // No enumeration of settings, no change to full screen
			
		// Now we change settings in the DEVMODE to be what we want.  Here we 
		// specify our new resolution to be the size of our window.
		devMode.dmPelsWidth = kWinWid;
		devMode.dmPelsHeight = kWinHgt;
			
		// The "dmFields" member of the DEVMODE struct is a flag that says 
		// what members in the DEVMODE we want to use when passing the DEVMODE to
		// a Win32 function.  Well we want to use the width and height we just
		// set in the DEVMODE, so we set those flags, while at the same time
		// keeping any flags that were set by calling EnumDisplaySettings().
		devMode.dmFields |= DM_PELSWIDTH | DM_PELSHEIGHT;

		// No we need to update the window to these acquired settings.
		// By parameter:
		// &devMode -- Address of a DEVMODE structure containing the settings to
		//			   change to
		// CDS_FULLSCREEN -- A flag that governs how the display settings will be changed.
		//					 This flag states the change in display settings is only temporary.
		// *** NOTE ***	ChangeDisplaySettings() returns DISP_CHANGE_SUCCESSFUL when it's successful
		//				so if we are not successful, exit the function
		if(ChangeDisplaySettings(&devMode,CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
			return false; // No full screen for you!
	
		// Okay we changed to full screen!  Now we need to do some more
		// stuff so everything looks as we expect.

		// First we get unattach the menu with the window
		// By parameter:
		// hwnd -- Handle of the window to affect
		// NULL -- Handle of the menu to associate with the window.  If this
		//		   handle is NULL, remove the current menu associated with the window.
		SetMenu(hwnd, NULL);

		// Next we need to change the window to a pop-up window so we
		// need to change it's styles
		// By parameter:
		// hwnd -- The handle of the window to affect
		// GWL_STYLE -- The attribute of the window to change
		// WS_POPUP -- The value to change the attribute to specified by the
		//			   second parameter  
		SetWindowLongPtr(hwnd, GWL_STYLE, WS_POPUP); // Change the style to pop-up
		SetWindowLongPtr(hwnd, GWL_EXSTYLE, NULL); // Change the ex-style to "no extended style"

		// Then we need to move the window to screen coordinates (0,0)
		// so that it takes up the full screen.
		// By parameter:
		// hwnd -- The handle of the window to affect
		// NULL -- Handle to the window to be inserted after in Z-order.  We don't want to
		//		   change Z-order so we can pass NULL.
		// 0 -- Upper-left X-pos of the window
		// 0 -- Upper-left Y-pos of the window
		// 0 -- The new width of the window.  We're not changing the width so we pass 0
		// 0 -- The new height of the window.  We're not changing the height so we pass 0
		// SWP_NOZORDER | SWP_NOSIZE | SWP_FRAMECHANGED -- These are additional flags you can pass to 
		//												   SetWindowPos().  These flags mean respectively,
		//												   "Do not change the window's Z-order",
		//												   "Do not change the window's size",
		//												   and "The window's style recently changed,
		//												   so make sure it gets updated". 
		SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_FRAMECHANGED);
		
		// Lastly we need to make sure the window is visible.  After all this 
		// changing it can get embarrased and not want to show itself.
		ShowWindow(hwnd, SW_SHOW);
				
		// We made it!  We changed to full screen.  Better update the full screen flag
		gFullScreen = true;
			return true;
	}
	else
	{
		// Well if "toFullScreen" doesn't equal true, it must equal false.  This means
		// we're changing back to a windowed mode.
	
		// If we're trying to switch to windowed mode but we are already
		// in windowed mode, don't do anything 
		if(gFullScreen == false)
			return true; // We didn't do anything, but we didn't encounter an error either 
		
		// First lets change back to windowed mode.  By passing NULL and 0 to the
		// ChangeDisplaySettings() it will change the display settings back to
		// whatever they display settings are that are stored in the registry and get 
		// loaded on system startup.
		ChangeDisplaySettings(NULL,0);
		
		// Now we will put the window's menu
		SetMenu(hwnd, gMenu);
		
		// Next we need to put back the window's styles 
		SetWindowLongPtr(hwnd, GWL_STYLE, kStyle);
		SetWindowLongPtr(hwnd, GWL_EXSTYLE, kExStyle);
		
		// Lastly we need to make sure the window is visible.  After all this 
		// changing, it can get embarrased and not want to show itself.
		ShowWindow(hwnd, SW_SHOW);

		// And that's it.  We could put the window back in it's original position 
		// before we toggled to full screen, but for simplicity sake, we'll just
		// leave at (0,0).  You can always drag it wherever you want %)
		gFullScreen = false; // Set flag appropriately
			return true;
	}
}

// Wrap up...  
//	
//	Changing to/from full screen can be a useful mechanic for your application.  Additionally,
//	you may want to add the functionality to do a full screen that changes your window to
//	whatever the current resolution is.
//
//  All questions and comments should be posted to www.GameTutorials.com.

/*-------------------------*\
|  Programmed by:  TheTutor |
|  ©2005 GameTutorials, LLC |
\*-------------------------*/


