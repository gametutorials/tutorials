//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	digiben@gametutorials.com		 //
//																		 //
//		$Program:		Flash											 //
//																		 //
//		$Description:	Load a Macromedia Flash movie in Win32			 //
//																		 //
//***********************************************************************//

#include "CFlash.h"		// Include our CFlash class


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// This tutorial shows you how to load Macromedia Flash media into a Win32 
// application.  You must have a Flash player installed to do this.  You
// can download a free player from Macromedia's website.
//
// * What is Macromedia Flash? *
//
// Flash is a program that allows you to create amazing multimedia and
// animation using vector graphics, among other standard formats.  Flash
// presentations can create dynamic content as well.  It has its own special
// language called "ActionScript".  Flash applications can be really small
// in size when doing animation because Flash can save vector data instead
// of per-frame pixel data.  Calculations are made each frame to animation
// the objects.  You can embed most standard video and graphic files into
// flash as well.
//
// * GameTutorials.com and Flash *
//
// If you have noticed, the GameTutorials.com site uses Flash for the main
// navigation.  Notice how much content is packed into the animations and such,
// yet the file size is only around 300K.
//
// So, what we do in this tutorial is load a .SWF file and embed the Flash
// into our Win32 application.  A .swf file is the final compiled file that
// Flash makes from the initial .FLA file, which is the workable project.
// 
// Let's jump into the code.  It's pretty straight forward.
// 


// Instead of doing a global object, let's make our CFlash object a pointer.
CFlash *g_pFlashObject = NULL;

// Let's create a rectangle that defines our desired window position.
RECT g_WindowRect = {0, 0, 800, 600};						

// This functions handles all the messages for windows
LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);		

// This will be our file that we want to load
const char g_szFile[] = "Main.swf";

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


// Our Win32 Main() function
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)			
{														
    HWND        hWnd;										// HWND - a HWND is a handle to a window.  This is used to keep track of a certain window.  Programs can have multiple windows.  We only have one though.
    MSG         msg;										// MSG  - A MeSsaGe variable to hold what messages are being sent to the window (If the window was clicked, closed, moved, etc...).
    WNDCLASSEX  wndclass;									// WNDCLASSEX - This variable will hold all the information about the window (The name, icon, cursor, color, menu bar...)

    wndclass.cbSize        = sizeof (wndclass);				// Here we set the size of the wndclass. You will see this a lot in windows, it's kinda odd.  We use the "sizeof()" function to tell windows the size of our class.
    wndclass.style         = CS_HREDRAW | CS_VREDRAW;		// The style we want is Verticle-Redraw and Horizontal-Redraw
    wndclass.lpfnWndProc   = WndProc;						// Here is where we assign our CALLBACK function.  Remember up above our function "WndProc"?  This just tells windows which function it needs to call to check for window messages.
    wndclass.cbClsExtra    = 0;								// We don't want to allocate any extra bytes for a class (useless for us)
    wndclass.cbWndExtra    = 0;								// Another useless thing for us.  I believe these last 2 are initialized to 0 anyway.
    wndclass.hInstance     = hInstance;						// We assign our hInstance to our window.  Once again, You can have several instances of a program, so this keeps track of the current one.
    wndclass.hIcon         = LoadIcon (NULL, IDI_WINLOGO);	// We call a function called LoadIcon that returns information about what icon we want.  I chose the Wndows Logo.  The NULL is in place of a hInstance.  We don't need one in this case.
    wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW);	// We call a function called LoadCursor that returns information about what cursor we want, I chose an arrow.  The NULL is in place of a hInstance.  We don't need one in this case.
															// Here we set the background color.  GetStockObject() returns a void so we must "cast" (turn it into) it as a HBRUSH to be compatible with the variable "hbrBackground".  I chose GRAY
    wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH);
    wndclass.lpszMenuName  = NULL;							// We don't have a menu, so let's set this to NULL.
    wndclass.lpszClassName = "GameTutorials";				// Here we set a name for our class, to distinguish it against others.  We need to use this name for later when we create the window.					
    wndclass.hIconSm       = LoadIcon (NULL, IDI_WINLOGO);	// We want the icon to be the Windows logo.  This is the icon in the top left color of the window.

	RegisterClassEx (&wndclass);							// We need to register our windows class with the windows OS.  We also need to pass the memory address of wndclass, so we use the "&".
															
															// Now we actually create the window.
															// CreateWindow() returns a handle to the window, which we store in our HWND "hwnd".
    hWnd = CreateWindow ("GameTutorials",					// window class name - This tells CreateWindow() to use our class up above.
						 "Flash Video",		  				// window's Title    - This will be the name on the title bar of the window.
						 WS_OVERLAPPEDWINDOW,				// window style		 - This flag tells windows to create a typical window, (options like resize, minimize, close, etc).
						 g_WindowRect.left,					// initial x position- The top left corner X value of the window in screen coordinates.  We will use our rectangle to position the window.
						 g_WindowRect.top,					// initial y position- The top left corner Y value of the window in screen coordinates.  We will use our rectangle to position the window.
						 g_WindowRect.right,				// initial x size	 - The bottom right corner X value of the window in screen coordinates.  We will use our rectangle to position the window.
						 g_WindowRect.bottom,				// initial y size	 - The bottom right corner Y value of the window in screen coordinates.  We will use our rectangle to position the window.
						 NULL,								// This is the parent window handle.  Since we don't have a parent window, we set this to NULL
						 NULL,								// This is the window menu handle, but since we don't have a menu, we set this to NULL.
						 hInstance,						    // This is the programs instance handle.  We just pass it our hInstance from our WinMain().  By the way, Windows OS passes the info to WinMain(). It's all taken care of for us.
						 NULL);								// If we wanted to pass in a variable or structure to the "WndProc", we would do it here.  (I just prefer using global variables though).  I just pass in NULL.

    ShowWindow (hWnd, iCmdShow);							// This shows our window.  We give it our handle to our window, which now has all our windows info, and our WinMain() variable iCmdShow.
    UpdateWindow (hWnd);									// This pretty much paints our window to the screen.
															// Here is our main loop.  This will continue to go until GetMessageReturns WM_QUIT, which will close the program.  This would happen if we closed the window.
	while (GetMessage (&msg, NULL, 0, 0))					// We need to pass in the address of "msg" because GetMessage fills in the structure "msg".  We pass in NULL for the HWND because that makes GetMessage check ALL windows that use our WndProc.
    {														// The rest we pass in 0, they are not important.
		TranslateMessage (&msg);							// "The TranslateMessage() function translates virtual-key messages into character messages." This basically means it translates it so windows understands it.
		DispatchMessage (&msg);								// "The DispatchMessage() function dispatches a message to a window procedure."  - This means it handles the message, like it the message was to close the window, it closes the window.
    }

	UnregisterClass("GameTutorials", hInstance);			// We need to unregister our windows class with the windows OS.  This way we free the memory that registering the class has taken. 
    
	return (int)msg.wParam ;								// We return the wParam of the structure "msg".  This is why we chose WinMain() to return an int.  A wParam/lParam is a 32-bit message parameter.  It holds data about the message.  You will see it's uses later. 
}


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// This is our callback function for our window
LRESULT CALLBACK WndProc (HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{									
	// This is the rectangle for our flash window
	RECT flashRect = {0, 0, 715, 715};

	// This will be used to store the full path for our Flash file.  The
	// function that loads the Flash file must have a full path to load.
	char szFullPath[MAX_PATH] = {0};

	// Check our messages
    switch (iMsg)											
    {					
		// When we create our window
		case WM_CREATE:
			
			// Get the full path for the file and add the path to the file name
			GetCurrentDirectory(MAX_PATH - (int)strlen(g_szFile), szFullPath);
			strcat(szFullPath, "\\");
			strcat(szFullPath, g_szFile);

			// Here we allocate memory for our flash object.  We need to pass in
			// the window handle, the full path of the file to load, the window rect,
			// as well as setting the loop and play flags to "true".
			g_pFlashObject = new CFlash(hWnd, szFullPath, &g_WindowRect, true, true);
			break;

		case WM_KEYDOWN:
			switch(wParam) {								// Check if we hit a key
				case VK_ESCAPE:								// If we hit the escape key

				// Now we need to free our object
				if(g_pFlashObject)
				{
					delete g_pFlashObject;
					g_pFlashObject = NULL;
				}
			
				PostQuitMessage(0);						// Send a QUIT message to the window
				break;
			}
			break;

		
		case WM_DESTROY:									// If we destroy the window

			// Now we need to free our object
			if(g_pFlashObject)
			{
				delete g_pFlashObject;
				g_pFlashObject = NULL;
			}

			// Quit the program and break
			PostQuitMessage(0);																							
			break;											
	}													

	return DefWindowProc (hWnd, iMsg, wParam, lParam);															
}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


///////////////////////////////////////////////////////////////////////////////
//
// * Quick Notes *
//
// In this file we created a window and allocated memory for our Flash object.
// We loaded a .SWF file with a specific rectangle.  Now check out CFlash.cpp
// to see the underlying code for our Flash object.
//
// Flash is a registered trademark of Macromedia.  All rights reserved.
//
//
// © 2000-2006 GameTutorials	   
