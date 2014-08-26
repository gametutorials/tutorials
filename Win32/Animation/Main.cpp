//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	digiben@gametutorials.com		 //
//																		 //
//		$Program:		Animation										 //
//																		 //
//		$Description:	Create animation from multiple bitmaps			 //
//																		 //
//***********************************************************************//

#include "main.h"											// Include our own header file which holds all the include and structure/function prototype information


// This tutorial shows us how to display animation from many bitmaps that have
// frames of animation already created.  We know how to display a bitmap by
// now, so we just display many bitmaps over one another to create animation.
// Just like Disney creates animation for its cartoons, we do the same thing 
// by quickly display different images to the screen.  We create a "cuaustic 
// effect", which is an effect that looks like water or reflections of light 
// through water.  We use these in 3D to product realistic water in our 3D 
// terrain (height map) tutorial series.  Check below to see how we display 
// our animation to the screen.


// Create a define for our window height
#define HEIGHT 600											
// Create a define for our window width
#define WIDTH  800							
// We want 30 frames per second for our animation speed
#define FRAME_RATE 30

#define VC_EXTRALEAN

// Create our double buffering structure
BUFFER gBuffer;												

// Create a sprite to animate
SPRITE gSprite;

// Create a second sprite to animate
SPRITE gSprite2;
																														
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)			// Anyway, so here is our "main()" for windows.  Must Have this for a windows app.
{														
	// Create the handle to the window.  
    HWND        hwnd;										
	// Create the variable to hold the window messages
    MSG         msg;	
	
	// Create the structure that holds the attributes of the window.
	// I just crammed them in like this to save space because we know what they do.
    WNDCLASSEX  wndclass = {sizeof (wndclass), CS_HREDRAW | CS_VREDRAW, WndProc, 0, 0, hInstance,
						  LoadIcon (NULL, IDI_WINLOGO), LoadCursor (NULL, IDC_ARROW), (HBRUSH) GetStockObject (WHITE_BRUSH),
						  NULL, "Window Class", LoadIcon (NULL, IDI_WINLOGO)};
	

	// Register the window class with the operating system
	RegisterClassEx (&wndclass);	
	
	// Now, we actually create the window
    hwnd = CreateWindow ("Window Class",					// window class name 
						 "Animation",			  			// window's Title    
						 WS_OVERLAPPEDWINDOW,				// window style		 
						 CW_USEDEFAULT,						// initial x position
						 CW_USEDEFAULT,						// initial y position
						 WIDTH,								// Here we pass in our desired width (800)	 
						 HEIGHT,						    // Here we pass in our desired height (600)	 
						 NULL,								// This is the parent window handle.  
						 NULL,								// This is the window menu handle
						 hInstance,						    // This is the programs instance handle.
						 NULL);								// We don't want to pass any extra data in, so NULL

	// This shows our window. 
    ShowWindow (hwnd, iCmdShow);							

	// This pretty much paints our window to the screen.
    UpdateWindow (hwnd);									

	// Here is our main loop. 
	while (1)					
    {	
		// *We use PeekMessage() instead of GetMessage() to see if there is a message
		// from windows, if not, then we want to animate when nothing is going on.*

		// Check if there is a window message and remove it from the queue
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// If the message was to quit, break out of the while loop
			if(msg.message == WM_QUIT) break;

			// This Translates messages so windows understands them.
			TranslateMessage (&msg);					
		
			// This sends the messages to the WndProc().
			DispatchMessage (&msg);							
		}
		else
		{
			// *Now we do the computationally expensive stuff in this else*

			// Check if we want to animate the next frame of animation
			if(AnimateNextFrame(FRAME_RATE))
			{
				// Animate the first water sprite
				AnimateSprite(&gBuffer, &gSprite); 

				// Animate the second water sprite
				AnimateSprite(&gBuffer, &gSprite2); 

				// Now swap the backbuffer to display it to the front and clear the screen
				SwapBackBuffer(&gBuffer, TRUE);
			}
		}
    }

	// Unregister the window class with the operating system
	UnregisterClass("Window Class",hInstance);
    
	// Quit the program
	return msg.wParam ;										
}

// Here is the WndProc that handles all the messages															
LRESULT CALLBACK WndProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// Check which message was passed in
    switch (iMsg)											
    {
		// This message is sent when the window is created (CreateWindow())
		case WM_CREATE:

			// Create the double buffer and load the bitmaps	
			Init(hwnd);										
			break;	

		// This message is sent when the window is resized
		case WM_SIZE:

			// This resizes the buffers to the new screen/window size
			ResizeBuffers(&gBuffer);
			break;

		// This message is sent when the mouse moves
		case WM_MOUSEMOVE:
				
			// Set the first sprite's position to the mouse position
			SetSpritePosition(&gSprite, LOWORD(lParam), HIWORD(lParam));

			// Set the second sprite's position to the mouse's X position
			gSprite2.position.x = LOWORD(lParam);
			break;
		
		// This message is sent when the window is destroyed
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
	// Hide the Cursor
	ShowCursor(FALSE);

	// Load and init the sprite
	LoadSprite(&gSprite, "caust", 32);
	SetSpritePosition(&gSprite, 200, 200);

	// Load and init the second sprite
	LoadSprite(&gSprite2, "caust", 32);
	SetSpritePosition(&gSprite2, 300, 100);

	// Create our double buffering
	CreateDoubleBuffering(&gBuffer, hwnd);					
															
	// Set the backbuffer to white first (This clears the backbuffer)
	ClearScreen(gBuffer.hdcBack, gBuffer.scrnRect, WHITE_BRUSH);			
}

void DeInit()
{
	// Set the back buffer to it's original bitmap (This is necessary for clean-up)
	SelectObject(gBuffer.hdcBack, gBuffer.hOldBitmap);

	// Free the 2 sprites we created
	FreeSprite(&gSprite);
	FreeSprite(&gSprite2);

	// Show the cursor again
	ShowCursor(TRUE);

	// Post the QUIT message to the window (0 = WM_QUIT)															
	PostQuitMessage(0);				
}	

////////////////////////////////////////////////////////////////////
//
// *Quick Notes*
//
// Here are the steps to animating a sprite to the screen:
//
// 1) First we need to create a SPRITE and call the LoadSprite() function
//
// 2) Then we want to set the sprites position on the screen.  We do this
//    by calling the SetSpritePosition() function.
//
// 3) Then we check to see if we need change to the next frame of animation.
//    I created a function that allows us to specify a frame rate.  Basically,
//    If we want the animation to change pictures 30 times a second, we can just
//    pass in 30.  This function, AnimateNextFrame() returns a true if we need 
//    to animate or not.  Then if so, we call the AnimateSprite() function.
//
// *Peek Message*
//
// We use peek message when there is animation or something else that is happening,
// that doesn't require us to trigger is.  If we just used GetMessage(), then we
// would have to move the mouse around constantly for the animation to happen.
// This way, we check if there's a window/system message, if so, handle it, otherwise
// we process our stuff in the "else" statement.  From now on, we will use PeekMessage().
//
// © 2000-2005 GameTutorials
