//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	digiben@gametutorials.com		 //
//																		 //
//		$Program:		Menu											 //
//																		 //
//		$Description:	Create a simple menu							 //
//																		 //
//***********************************************************************//

#include <iostream>			// Include the standard C++ file
#include <windows.h>		// We need to include windows.h
#include "resource.h"
using namespace std;		// Include the standard namespace															
																														
// The function prototype for our WndProc (The function that handles all the windows messages)															
LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);																																																																											
																												
// Anyway, so here is our "main()" for windows.  Must Have this for a windows app.															
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)			
{	
	// A HWND is a handle to a window.  This is used to keep track of a certain window.  Programs can
	// have multiple windows.  We only have one though.													
    HWND hwnd = NULL;
    
    // A MeSsaGe variable is used to hold what messages are being sent to the window (If the window was
    // clicked, closed, moved, etc...). 
    MSG msg = {0};
    
    // This variable will hold high level information about the window (icon, cursor, background color, 
    // menu bar, etc...)  Once a window class is registered with the operation system, we can make 
    // cookie cutter windows that all share the properties of a registered window class.  This how
    // templates for buttons, dialog boxes, scroll bars, etc, are done.
    WNDCLASSEX wndclass = {0}; 

    // Init all the important fields in the WNDCLASSEX
	wndclass.cbSize = sizeof(wndclass); // Size in bytes of the WNDCLASSEX		
	wndclass.style = CS_HREDRAW | CS_VREDRAW; // Style of WNDCLASSEX		
	wndclass.lpfnWndProc = WndProc; // Pointer to our window procedure
	wndclass.hInstance = hInstance; // Instance of our window						
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); // Background color of the window
	wndclass.hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(IDC_ARROW), IMAGE_CURSOR,
										  0, 0, LR_SHARED); // Set the cursor to be the default arrow cursor
	wndclass.lpszClassName = "Window Class 1"; // Name of our class

////////// *NEW* /////////// *NEW* /////////// PUT THE MENU ID HERE ////////// *NEW* /////////// *NEW* ///////////
    wndclass.lpszMenuName  = MAKEINTRESOURCE(IDR_MENU1); // Here we set our menu using the menu ID that
														 // corresponds to the menu we created in the resource
														 // editor.  However, the variable lpszMenuName is a string
														 // and thus expects a string!  No worries, the macro
														 // MAKEINTRESOURCE() converts our resource ID into the
														 // corresponding string name that needs to be set.
////////// *NEW* /////////// *NEW* /////////// PUT THE MENU ID HERE ////////// *NEW* /////////// *NEW* ///////////

	// We need to register our windows class with the windows OS.  The function wants a pointer to
	// the WNCLASSEX so we use the "&".
	RegisterClassEx (&wndclass); 
															
	// Now we actually create the window.
	// CreateWindow() returns a handle to the window, which we store in our HWND "hwnd".														
    hwnd = CreateWindowEx(NULL,					// window's extra style - The NULL parameter means no extra style
						  "Window Class 1",		// window class name - This tells CreateWindow() to use our class up above.
						  "My First Window",	// window's Title    - This will be the name on the title bar of the window.
						  WS_OVERLAPPEDWINDOW,	// window style		 - This flag tells windows to create a typical window, (options like resize, minimize, close, etc).
						  CW_USEDEFAULT,		// initial x position- The top left corner X value of the window in screen coordinates.  This flag let's windows choose for us.
						  CW_USEDEFAULT,		// initial y position- The top left corner Y value of the window in screen coordinates.  This flag let's windows choose for us.
						  CW_USEDEFAULT,		// initial x size	 - The bottom right corner X value of the window in screen coordinates.  This flag let's windows choose for us.
						  CW_USEDEFAULT,		// initial y size	 - The bottom right corner Y value of the window in screen coordinates.  This flag let's windows choose for us.
						  NULL,					// This is the parent window handle.  Since we don't have a parent window, we set this to NULL
						  NULL,					// Even though this is a menu parameter, we will leave it NULL because we set our menu in the class variable, "lpszMenuName".  Alternatively we could create a menu and pass it's handle here.
						  hInstance,			// This is the programs instance handle.  We just pass it our hInstance from our WinMain().  By the way, Windows OS passes the info to WinMain(). It's all taken care of for us.
						  NULL);				// If we wanted to pass in a variable or structure to the "WndProc", we would do it here.
	
	// This shows our window.  We give it our handle to our window, which now has all our 
	// windows info, and our WinMain() variable iCmdShow.
    ShowWindow (hwnd, iCmdShow);							
    
    // This intially paints our window to the screen.
    UpdateWindow (hwnd);									
	
	// Here is our main loop.  This will continue to go until PeekMessage returns WM_QUIT, 
	// which will close the program.  This will happen when we close the window.														
	while(1) // Loop forever
	{	
		// Get any messages the OS has for us
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// If the message says to quit, quit!
			if(msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg); // Translate from virtual keys to character messages						
			DispatchMessage(&msg); // Send the message to the WndProc
		}							
	}
    
    // We return the wParam of the structure "msg".  It contains the return code
    // for WinMain()
	return (int)msg.wParam;	
}

// Here is the WndProc that handles all the messages															
LRESULT CALLBACK WndProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    switch (iMsg) // This checks what the message is.  Below is some of the message that windows might
				  // return.  There of course are HUNDREDS.
    {

/////////// *NEW* /////////// CHECK FOR MENU CLICK ////////// *NEW* ///////////

		// This message is sent when the user selects a command item from a menu, and some other things.
		case WM_COMMAND:
			// If we check the Low-bits (LOWORD) of the DWORD wParam we can see what menu item is clicked on.
			if(LOWORD(wParam) == IDM_QUIT) 
			{
				// If they clicked on the menu ID: IDM_QUIT, then we want to close the window so we
				// send that message.
				SendMessage(hwnd, WM_CLOSE, 0, 0); 
			}
			break;

/////////// *NEW* /////////// CHECK FOR MENU CLICK ////////// *NEW* ///////////

		// This message is sent when the user closes the window.
		case WM_DESTROY:									
			
			// You must call this function or else you will need to do control-alt-delete and manually
			// close the program from the programs queue.  This function actually puts a message on 
			// the message queue.  By passing zero, we are passing the WM_QUIT message which
			// we handle in WinMain  
			PostQuitMessage(0);								
			break;
			
		default:
		
			// The DefWindowProc function calls the default window procedure to provide default processing
			// for any window messages that an application does not process.  This function ensures that
			// every message is processed. DefWindowProc is called with the same parameters received by
			// the WndProc. 
			return DefWindowProc (hwnd, iMsg, wParam, lParam);			
	}													

	return 0; // If we handled the message, return zero.
}

////////////////////////////////////////////////////////////
//
// *Quick Notes*
//
// So, now we have a menu!  To make a menu, here is a quick quide:
//
// 1) Go to the main menu in Visual Studio and select "Project" (ALT-P)
//
// 2) Select "Add Resource..."
//
// 3) Select "Menu" from the list of resources then click the "New" button
//
// 4) Rename the menu "IRD_MENU1" (Most likely called that..) to something you want with quotes,
//    for example:  "My_Menu" (Must have quotes!!!)
//
// 5) There should be a box in the top left corner of the new menu resource window..
//    If you double click on the you can choose your menu name. (Ie. File)
//    Then you can double click on it again and there should be a spot in your menu
//    to click on and create something like "Quit" (& in front of the character makes 
//    that a character a "hot key" and you'll get the underline, underneath it).
//    Now save what you've done. It automatically creates a .rc and resource.h file.
//
// That's it!  
//
// © 2000-2005 GameTutorials      
