//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	digiben@gametutorials.com		 //
//																		 //
//		$Program:		Bitmap background								 //
//																		 //
//		$Description:	Draws a pattern background to a window			 //
//																		 //
//***********************************************************************//
#include <iostream>											// Include the standard C++ file
#include <windows.h>										// We need to include windows.h
using namespace std;										// Include the standard namespace															
																														
															
LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);		// The function prototype for our WndProc (The function that handles all the windows messages)																																																																									
																												
															
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)			// Anyway, so here is our "main()" for windows.  Must Have this for a windows app.
    {														
    HWND        hwnd;										// Create the handle to the window.  
    MSG         msg;										// Create the variable to hold the window messages
    WNDCLASSEX  wndclass;									// Create the structure that holds the attributes of the window

////////// *NEW* /////////// *NEW* /////////// BRUSH FOR THE BACKGROUND ////////// *NEW* /////////// *NEW* ///////////

	LOGBRUSH background;									// This creates a logical brush that allows us to store a bitmap for the background

////////// *NEW* /////////// *NEW* /////////// BRUSH FOR THE BACKGROUND ////////// *NEW* /////////// *NEW* ///////////

    wndclass.cbSize        = sizeof (wndclass);				// Set the size of the class (required)
    wndclass.style         = CS_HREDRAW | CS_VREDRAW;		// Set the style of the window to redraw horizontally and vertically.
    wndclass.lpfnWndProc   = WndProc;						// Assign the callback function to our window.
    wndclass.cbClsExtra    = 0;								// We don't need any extra bytes
    wndclass.cbWndExtra    = 0;								// We don't need any extras
    wndclass.hInstance     = hInstance;						// Assign the hInstance passed from the OS to the window
    wndclass.hIcon         = LoadIcon (NULL, IDI_WINLOGO);	// Set the windows logo Icon
    wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW);	// Set the default arrow to the window

////////// *NEW* /////////// *NEW* /////////// ASSIGNING THE BITMAP TO A BRUSH ////////// *NEW* /////////// *NEW* ///////////	

	background.lbStyle     = BS_PATTERN;					// Now we assign the brush.  This makes the brush a continuous pattern of the bitmap we loaded.
															// This loads the bitmap from a file.  the 0 and 0 are width and height.  If we say zero it does it for us.
															// We cast the return value as a "long" because the lbHatch is a "long"  (A long is the same thing as a 32-bit integer)
															// LoadImage() returns a HANDLE to a bitmap.  We convert it to a long to satisfy the types.  (It will complain if we don't)
	background.lbHatch     = (long) LoadImage(hInstance,"Bitmap.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);														
															// Here we call a function that creates and returns an HBRUSH (handle to a brush) for our background
															// The brush will be a bitmap that will be used as a pattern.  In the next tutorial I will show how
															// to draw a bitmap to the screen in a more usefull way.
    wndclass.hbrBackground = CreateBrushIndirect(&background); 

////////// *NEW* /////////// *NEW* /////////// ASSIGNING THE BITMAP TO A BRUSH ////////// *NEW* /////////// *NEW* ///////////	

    wndclass.lpszMenuName  = NULL;							// Set the menu to NULL

    wndclass.lpszClassName = "Window Class";				// Set the name of our windows class
    wndclass.hIconSm       = LoadIcon (NULL, IDI_WINLOGO);	// Assign the small icon to the windows logo

	RegisterClassEx (&wndclass);							// Register the window class with the operating system
															
															// Now, we actually create the window
    hwnd = CreateWindow ("Window Class",					// window class name 
						 "Bitmap Background",	  			// window's Title    
						 WS_OVERLAPPEDWINDOW,				// window style		 
						 CW_USEDEFAULT,						// initial x position
						 CW_USEDEFAULT,						// initial y position
						 CW_USEDEFAULT,						// initial x size	 
						 CW_USEDEFAULT,					    // initial y size	 
						 NULL,								// This is the parent window handle.  
						 NULL,								// This is the window menu handle
						 hInstance,						    // This is the programs instance handle.
						 NULL);								// We don't want to pass any extra data in, so NULL

    ShowWindow (hwnd, iCmdShow);							// This shows our window. 
    UpdateWindow (hwnd);									// This pretty much paints our window to the screen.
															
	while (GetMessage (&msg, NULL, 0, 0))					// Here is our main loop. 
    {														
		TranslateMessage (&msg);							// This Translates messages so windows understands them.
		DispatchMessage (&msg);								// This sends the messages to the WndProc().
    }
    
	return msg.wParam ;										// Quit the program
}

															// Here is the WndProc that handles all the messages															
LRESULT CALLBACK WndProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    switch (iMsg)											// Check which message was passed in
    {

		case WM_DESTROY:									// This message is sent when the user closes the window.
			
			PostQuitMessage(0);								// Post the QUIT message to the window (0 = WM_QUIT)															
			break;				
	}													

	return DefWindowProc (hwnd, iMsg, wParam, lParam);		// Process the default messages
															
}															// End of the WndProc

////////////////////////////////////////////////////////////
//
// *Quick Notes*
//
// This program reads in a bitmap, then uses it as a pattern for the background.
// Later, in the next tutorial I will show you how to read in a bitmap, and display
// it to the screen where ever we want.  This tutorial was just an introductory to bitmaps.
// 
// The LOGBRUSH is a logical brush that allows us to assign a style and image bits to it.
// In our case, we assigned a bitmap to it.  If you look back at the previous brush tutorials
// you can apply what you learned there to see how you could fill in circles and rectangles
// with a bitmap.  
// 
// To convert the logical brush to a HBRUSH (a handle to a brush) you use the funciton:
// CreateBrushIndirect().  Then you pass in the address of the logical brush that you used.
// 
// Once again, since CreateBrushIndirect(const LOGBRUSH *) is the prototype, that means
// that we need to pass in the address, by putting the '&' character in front of the variable
// when we call the function.  like, CreateBrushIndirect(&background).  the 'const' part
// assure that it won't get changed, but for some reason it needs a pointer to it.
//
// If you wanted to NOT make a pattern, you can mess around witht he art by making
// the canvas size huge, and putting the picture you want to display in the middle.
// If you mess around with it, you will see what I am talking about.  It starts over
// the pattern when the canvas size is done.  So if you have a huge white canvas, and a little
// picture in the middle, it would do a pattern of the whole canvas, which you wouldn't see
// the pattern because it would be so much larger than the window you would never know.
//
// © 2000-2005 GameTutorials
