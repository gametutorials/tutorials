//***********************************************************************//
//																		 //
//		- "Talk to me like a 3 year old!" Programming Lessons -			 //
//                                                                       //
//		$Author:		DigiBen		digiben@gametutorials.com			 //
//																		 //
//		$Program:		Full Screen Apps								 //
//																		 //
//		$Description:	This gives the user the option to go full screen //
//																		 //
//***********************************************************************//

#include <windows.h>										// We need to include windows.h
#include <stdio.h>											// Include stdio.h for the basics

#define SCREEN_WIDTH  800									// We want a 800 pixel width resolution
#define SCREEN_HEIGHT 600									// We want a 600 pixel height resolution

BOOL bFullScreen = FALSE;									// Create a boolean and set it to false.  If we choose full screen, set it to TRUE


///////////////////////////////// CHANGE TO FULL SCREEN \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This changes the screen to FULL SCREEN
/////
///////////////////////////////// CHANGE TO FULL SCREEN \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void ChangeToFullScreen(int width, int height)
{
	DEVMODE dmSettings;									// Device Mode variable - Needed to change modes

	memset(&dmSettings,0,sizeof(dmSettings));			// Makes Sure Memory's Cleared

	// Get the current display settings.  This function fills our the settings.
	if(!EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&dmSettings))
	{
		// Display error message if we couldn't get display settings
		MessageBox(NULL, "Could Not Enum Display Settings", "Error", MB_OK);
		return;
	}

	dmSettings.dmPelsWidth	= width;					// Set the desired Screen Width
	dmSettings.dmPelsHeight	= height;					// Set the desired Screen Height
	dmSettings.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;	// Set the flags saying we're changing the Screen Width and Height
	
	// This function actually changes the screen to full screen
	// CDS_FULLSCREEN Gets Rid Of Start Bar.
	// We always want to get a result from this function to check if we failed
	int result = ChangeDisplaySettings(&dmSettings,CDS_FULLSCREEN);	

	// Check if we didn't receive a good return message From the function
	if(result != DISP_CHANGE_SUCCESSFUL)
	{
		// Display the error message and quit the program
		MessageBox(NULL, "Display Mode Not Compatible", "Error", MB_OK);
		PostQuitMessage(0);
	}
}


LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);		// Create a function prototype for the wndProc.
															// Here is our "main()" equivalent in windows, WinMain().	
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)			
{														
    HWND        hwnd;										// Create a variable that holds a handle to our window.
    MSG         msg;										// Create a variable to hold the message information
    WNDCLASSEX  wndclass;									// This variable will hold all the information about the window (The name, icon, cursor, color, menu bar...)
	DWORD dwStyle;

    wndclass.cbSize        = sizeof (wndclass);				// Here we set the size of the wndclass. 
    wndclass.style         = CS_HREDRAW | CS_VREDRAW;		// The style we want is Vertical-Redraw and Horizontal-Redraw
    wndclass.lpfnWndProc   = WndProc;						// Here is where we assing our CALLBACK function. (The function to handle the messages)
    wndclass.cbClsExtra    = 0;								// We want zero extra bytes
    wndclass.cbWndExtra    = 0;								// Init this useless thing to 0
    wndclass.hInstance     = hInstance;						// We assign our hInstance to our window.  
    wndclass.hIcon         = LoadIcon (NULL, IDI_WINLOGO);	// Load a logo for our window
    wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW);	// Load a arrow cursor for our window.
															// Set the background color for the window
    wndclass.hbrBackground = (HBRUSH) GetStockObject (BLACK_BRUSH);
    wndclass.lpszMenuName  = NULL;							// We pass NULL because we don't want a menu.
    wndclass.lpszClassName = "Full Screen App";				// Create a name that we identify this window class with.
    wndclass.hIconSm       = LoadIcon (NULL, IDI_WINLOGO);	// Create an icon for the top left of the window.

	RegisterClassEx (&wndclass);							// We need to register the wndclass.
															
	if(MessageBox(NULL, "Click Yes to go to full screen (Recommended)", "Options", MB_YESNO | MB_ICONQUESTION) == IDNO)
	{
		dwStyle = WS_OVERLAPPEDWINDOW;						// If we don't want full screen, open a simple window
	} 
	else													// If we chose YES
	{			
		bFullScreen = TRUE;									// Set our boolean to TRUE, we wanted full screen

															// This is the style that we need our window to have in order to be windowless full screen
		dwStyle = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		ChangeToFullScreen(SCREEN_WIDTH, SCREEN_HEIGHT);	// This changes our screen to full screen with our desired resolution
	}

															// Here we create the window returning our handle to the window.
    hwnd = CreateWindow ("Full Screen App",					// window class name 
						 "Full Screen App",		  			// window's Title    
						 dwStyle,							// window style		 
						 0,									// initial x position
						 0,									// initial y position
						 SCREEN_WIDTH,						// initial x size - Our resolution width
						 SCREEN_HEIGHT,					    // initial y size - Our resolution height	 
						 NULL,								// Pass NULL for the parent window
						 NULL,								// Pass NULL for a menu
						 hInstance,						    // Pass in our hInstance
						 NULL);								// Pass NULL to the wndProc, we don't want to pass it any variables address's.

    ShowWindow (hwnd, iCmdShow);							// Show the window
    UpdateWindow (hwnd);									// Update our window and paint it to the screen
															
	while(1)												// Loop forever
	{	
															
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))		// Get any messages the OS has for us
		{
			
			if(msg.message == WM_QUIT)						// If the message says to quit, quit!
				break;

			TranslateMessage(&msg);							// Translate from virtual keys to character messages						
			DispatchMessage(&msg);							// Send the message to the WndProc
		}							
	}

	UnregisterClass("Full Screen App",hInstance);			// We need to unregister the wndclass.
    
	return msg.wParam ;										// Quit the program
}
															// Start our window procedure (wndProc).  This handles the messages
LRESULT CALLBACK WndProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{	
	switch (iMsg)											// This checks what the message is.  Below is some of the message that windows might return.  There of course is HUNDREDS.
    {
		case WM_KEYDOWN:									// If we pressed a key
			switch (wParam) 								// Check what the wParam holds.  The wParam holds which key we pressed.
			{		
				case VK_ESCAPE:								// If we pressed the ESCAPE key.
					if(bFullScreen)							// If we went to full screen mode
					{										// Calling the same function with NULL and 0 reset the screen settings and resolution
						ChangeDisplaySettings(NULL, 0);
					}
					SendMessage(hwnd, WM_CLOSE, 0, 0);		// Close the program
					break;
			}
			break;

		case WM_DESTROY:									// This message is sent when the user closes the window.
			PostQuitMessage(0);								// Post a WM_QUIT to our message loop to quit the application
			break;											// Break from the switch statement
	}													

	return DefWindowProc (hwnd, iMsg, wParam, lParam);		// Return generic information to our message loop
}															// End of the WndProc

/////////////////////////////////////////////////////////////////////////////////
//
// ** SPECIAL NOTE **
//
// If you run Win95 you should use this instead:
//
// void ChangeToFullScreen(int width, int height)
// {
//		DEVMODE devmode = {0};
//  
//		devmode.dmSize = sizeof(DEVMODE);
//		devmode.dmPelsWidth	= width;
//		devmode.dmPelsHeight = height;
//		devmode.dmBitsPerPel = 16; // Bits per pixel
//		devmode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
//	
//		if(ChangeDisplaySettings(&devmode, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
//		{
//			// Display the error message and quit the program
//			MessageBox(NULL, "Display Mode Not Compatible", "Error", MB_OK);
//			PostQuitMessage(0);
//		}
//	
// }
//
//

// * QUICK NOTES * 
//
// This shows you how to go to full screen in a window app.  Before we create our
// window you will notice that we have to change the style, depending on if we
// chose full screen or not.  If we didn't change the style it would be full screen,
// but it would look like a window.  We usually want to get rid of the window look,
// and that is why we change the style of it.
//
// We use a message box to get the choice from the user.  This is a very good idea.
// The function MessageBox() returns the button ID that was clicked.  In our case,
// we checked on IDNO.  If we clicked the NO button, then don't go to full screen.
// If we didn't click NO, it will return IDYES, and then sets our flag/boolean to TRUE.
// This is important because when we close the program we need to restore the settings.
// To do this, we call the ChangeDisplaySettings() again, but pass in NULL and 0 for
// the parameters.  It restores the old settings before we changed it, which is nice
// that we don't need to keep track of the old settings.  The ChangeToFullScreen()
// function should be modular enough to plug into any project.  It's simple to use
// and easy to cleanup.
//
// In the ChangeToFullScreen() function, what we do is enumerate the current display
// settings. This means, that we fill in our DEVMODE structure with our current screen
// settings.  If you debug you will find it will hold your current screen resolution,
// along with other information after the function is called.  But, before we pass that
// information into ChangeDisplaySettings(), we want to change the resolution to our
// desired resolution that was passed in.
//
// Just remember to call ChangeDisplaySettings() again before you close the program or
// else you will be stuck in the changed mode until you manually change it in your
// display settings from the control panel.
//
// If you haven't seen the WM_KEYDOWN message yet, you can kinda see what it does.
// The wParam holds the key that was pressed down, so we can query it like we did above.
// Each key has a virtual key code, hence the VK_.  Look in MSDN for the rest of them.
// In this program, we check if the user hit the escape key.  If so, restore the settings
// and quit the program.
//
// © 2000-2005 GameTutorials
