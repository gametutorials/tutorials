//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	digiben@gametutorials.com		 //
//																		 //
//		$Program:		Bitmaps (double buffering)						 //
//																		 //
//		$Description:	Display bitmaps to the screen using double buffering
//																		 //
//***********************************************************************//

#include "bitmap.h"											// Include our own header file which holds all the include and structure/function prototype information

// Create a define for our window height
#define HEIGHT 600											
// Create a define for our window width
#define WIDTH  800							
				

// Create our double buffering structure
BUFFER gBuffer;												
// Create a handle to our first bitmap
HBITMAP hTileBitmap;										
// Create a handle to our second bitmap
HBITMAP hTileBitmap2;				
																														
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)			// Anyway, so here is our "main()" for windows.  Must Have this for a windows app.
{														
	// Create the handle to the window.  
    HWND        hwnd;										
	// Create the variable to hold the window messages
    MSG         msg;										
	// Create the structure that holds the attributes of the window
    WNDCLASSEX  wndclass;
	

	// Set the size of the class (required)
    wndclass.cbSize        = sizeof (wndclass);				
	// Set the style of the window to redraw horizontally and vertically.
    wndclass.style         = CS_HREDRAW | CS_VREDRAW;		
	// Assign the callback function to our window.
    wndclass.lpfnWndProc   = WndProc;						
	// We don't need any extra bytes
    wndclass.cbClsExtra    = 0;								
	// We don't need any extras
    wndclass.cbWndExtra    = 0;								
	// Assign the hInstance passed from the OS to the window
    wndclass.hInstance     = hInstance;						
	// Set the windows logo Icon
    wndclass.hIcon         = LoadIcon (NULL, IDI_WINLOGO);	
	// Set the default arrow to the window
    wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW);		
	// Set the background to white
    wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH); 
	// Set the menu to NULL
    wndclass.lpszMenuName  = NULL;							
	// Set the name of our windows class
    wndclass.lpszClassName = "Window Class";				
	// Assign the small icon to the windows logo
    wndclass.hIconSm       = LoadIcon (NULL, IDI_WINLOGO);	

	// Register the window class with the operating system
	RegisterClassEx (&wndclass);	
	
	// Now, we actually create the window
    hwnd = CreateWindow ("Window Class",					// window class name 
						 "Multiple Bitmaps",	  			// window's Title    
						 WS_OVERLAPPEDWINDOW,				// window style		 
						 CW_USEDEFAULT,						// initial x position
						 CW_USEDEFAULT,						// initial y position

//////// * ///// * NEW * //////// *  ASSIGN WINDOW WIDTH / HEIGHT * //////// * ///// * NEW * //////////

						 WIDTH,								// Here we pass in our desired width (800)	 
						 HEIGHT,						    // Here we pass in our desired height (600)	 

//////// * ///// * NEW * //////// *  ASSIGN WINDOW WIDTH / HEIGHT * //////// * ///// * NEW * //////////

						 NULL,								// This is the parent window handle.  
						 NULL,								// This is the window menu handle
						 hInstance,						    // This is the programs instance handle.
						 NULL);								// We don't want to pass any extra data in, so NULL

	// This shows our window. 
    ShowWindow (hwnd, iCmdShow);							

	// This pretty much paints our window to the screen.
    UpdateWindow (hwnd);									

	// Here is our main loop. 
	while (GetMessage (&msg, NULL, 0, 0))					
    {	
		// This Translates messages so windows understands them.
		TranslateMessage (&msg);					
		
		// This sends the messages to the WndProc().
		DispatchMessage (&msg);								
    }

	// Unregister the window class
	UnregisterClass("Window Class",hInstance);
    
	// Quit the program
	return msg.wParam ;										
}

// Here is the WndProc that handles all the messages															
LRESULT CALLBACK WndProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	static int x=0, y=0;

	// Check which message was passed in
    switch (iMsg)											
    {

		case WM_CREATE:

			// Create the double buffer and load the bitmaps
			Init(hwnd);										
			break;

		case WM_SIZE:

			// If the window changes width and heigth, we need to resize the buffers
			ResizeBuffers(&gBuffer);
			break;
	
		case WM_MOUSEMOVE:		

			// Get the X position of the mouse
			x = LOWORD(lParam);								

			// Get the Y position of the mouse
			y = HIWORD(lParam);								

			// Display the  tile bitmap at the mouse position
			DisplayBitmap(&gBuffer, hTileBitmap, x, y);		

			// Display another bitmap along the y axis of the cursor
			DisplayBitmap(&gBuffer, hTileBitmap2, 200, y);	

			// Display another bitmap along the x axis of the cursor
			DisplayBitmap(&gBuffer, hTileBitmap2, x, 200);	

			// Now that we filled the back buffer with images, display it on screen
			SwapBackBuffer(&gBuffer, TRUE);					
			break;

		case WM_DESTROY:									

			// Free all the data and set everything back to normal, then post the quit message
			DeInit();										
			break;				
	}													

	// Process/handle the default messages
	return DefWindowProc (hwnd, iMsg, wParam, lParam);		
															
}

void Init(HWND hwnd)
{	
	// Load the bitmap and store it in a handle
	hTileBitmap = LoadABitmap("Bitmap.bmp");		

	// Load another bitmap and store it in a handle
	hTileBitmap2 = LoadABitmap("Bitmap2.bmp");		
	
	// Display an error message if we didn't load the bitmaps
	if(!hTileBitmap)  MessageBox(hwnd, "Can't find the tile bitmap!", "Error", MB_OK);
	if(!hTileBitmap2) MessageBox(hwnd, "Can't find the tile bitmap2!", "Error", MB_OK);

	// Create our double buffering
	CreateDoubleBuffering(&gBuffer, hwnd);					
															
	// Set the backbuffer to white first (This clears the backbuffer)
	ClearScreen(gBuffer.hdcBack, gBuffer.scrnRect, WHITE_BRUSH);			
}

void DeInit()
{
	// Set the back buffer to it's original bitmap (This is necessary for clean-up)
	SelectObject(gBuffer.hdcBack, gBuffer.hOldBitmap);

	// Delete our handles to the bitmaps (Frees memory)
	DeleteObject(hTileBitmap);
	DeleteObject(hTileBitmap2);

	// Post the QUIT message to the window (0 = WM_QUIT)															
	PostQuitMessage(0);				
}	

////////////////////////////////////////////////////////////////////
//
// *Quick Notes*
//
// WHAT IS BACKBUFFERING!?!!?!?
//
// Back Buffering is what we do to get rid of the flicker when we display	
// images to the screen.  It basically works like this:
//
// A. Display all our bitmaps to a buffer (a copy of the screen).
// B. Then display all the bitmaps at once to the screen by flipping
//    the back buffer to the front.  
//
// Think of it this way.  You are cutting out pictures in a magazine to
// make a collage.  You paste them one by one onto a piece of paper.
// Let's pretend someone is watching you do this, and they see you
// paste each one to the paper, one by one.  That is our flicker.
// Instead of doing it like that, we leave the piece of paper blank,
// so all the user see's in the blank piece of paper.  Then behind the
// scenes you paste all the pictures onto a seperate piece of paper.
// After you are done, you just "flip" the 2 pieces of paper, so now
// the user see the final product, and not the process.
//
// This is exactly what we are doing, except the computer does it liquid fast.
// If it sounds silly to you, and you don't understand the concept, then just
// know that it works, and you should use it.  Look at the last tutorial to see
// The difference between flickering bitmaps and crystal clear back buffered ones.
//
//
// Let me explain the sequence that you will want to use in order
// to display bitmaps to the screen using backbuffering: 
// 
// 1) You need to call the CreateDoubleBuffering() function (once) to set up
//    our double buffering.  This needs the address of BUFFER structure
//    to fill in, and the handle to the window.  This function fills in
//    the buffer structure appropriately and allows us to display bitmaps
//    without all the flicker.  We will be using the buffers in this structure
//    later in our DisplayBitmap() function.  You will notice we clear the 
//    screen of the backbuffer so it doesn't look horrible the first frame. 
//
// 2) Next you want to load the bitmaps you want by calling LoadABitmap().
//    This only needs the name of the bitmap, (Ie, "Bitmap.bmp").  This
//    function passes back an HBITMAP that holds the info about the bitmap.
// 
// 3) After we have our buffers and bitmaps loaded, we want to Display them!
//    We do this by calling DisplayBitmap().  This function needs the buffer
//    structure, the HBITMAP we are displaying, finally, the x and y position 
//    to draw it to the screen.  This function doesn't draw it to the screen.
//    It draws it to the backbuffer.  You need to "flip" the front buffer with
//    The back buffer.  You do that by calling SwapBackBuffer().
// 
// 4) Now that you called DisplayBitmap(), you need to flip the buffers.
//    SwapBackBuffer() actually brings the images to the screen where we can
//    see them.  The reason why we do this is because we want ALL the bitmaps
//    to be on the screen at the same time.  If we didn't do it this way, it
//    flicker and we might even see some tearing as EACH bitmap went to the
//    screen at a time.  It's better this way, it shows the bitmaps all at once.
//	  That is the principle of backbuffering.
//
// 5) Now you have bitmaps displaying to the screen.  But when you are finished
//    With your program, you need to free all the data we created.  
//
// There you have it!  That's how we display bitmaps to the screen with no flicker.
// Next we will cover animation.  You will be using backbuffering from now on.
//
// © 2000-2005 GameTutorials	       
