//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	digiben@gametutorials.com		 //
//																		 //
//		$Program:		Transparency									 //
//																		 //
//		$Description:	Create a TransparentBlit() using DIB Sections	 //
//																		 //
//***********************************************************************//


#include "main.h"											// Include our own header file which holds all the include and structure/function prototype information

// This is the width and height in pixels of one frame from the bitmap.
// This does NOT include the border around each frame of animation.
#define MONSTER_WIDTH  72
#define MONSTER_HEIGHT 76

// This is the amount of frames we animate for each monster in one cycle of animation
#define MONSTER_FRAMES  4

// Create our double buffering structure
BUFFER gBuffer;												

// Create a sprite for the monster
SPRITE gMonster;

// This will hold the background bitmap
HBITMAP hBackground;
																													
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)			// Here is our "main()" for windows.  Must Have this for a windows app.
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
						 "Bitmap Background",	  			// window's Title    
						 WS_SYSMENU,						// window style	- This style won't allow the window to resize
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
				// Calculate the frames per second of our application
				CalculateFrameRate(hwnd);				

				// Animate the monster2 sprite
				AnimateSprite(&gBuffer, &gMonster);  
				
				// Move the sprite along the screen
				MoveSprite(&gMonster);

				// Now swap the backbuffer to display it to the front and clear the screen
				SwapBackBuffer(&gBuffer, FALSE);

				// Now we want erase the position of the monster with a part of the background
				// We do this so we don't have to blit the WHOLE background each time (SLOWWWWW)
				EraseSprite(&gBuffer, &gMonster, hBackground);	
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
	
		case WM_RBUTTONDOWN:
			// By pressing the right mouse button you can switch animations
			SetSpriteState(&gMonster, ++gMonster.state % 4);
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
	// Create our double buffering
	CreateDoubleBuffering(&gBuffer, hwnd);

	// Load the monster
	LoadSprite(&gMonster, "monster.bmp", MONSTER_WIDTH, MONSTER_HEIGHT, MONSTER_FRAMES);
	// Set the sprites position
	SetSpritePosition(&gMonster, 100, 200);
	// Set the direction of the sprite
	SetSpriteState(&gMonster, EAST);
	// Set the X and Y speed of the monster
	SetSpriteSpeed(&gMonster, 8, 12);

	// Load the background image
	hBackground = LoadABitmap("background.bmp");
															
	// Set the backbuffer to white first (This clears the backbuffer)
	ClearScreen(gBuffer.hdcBack, gBuffer.scrnRect, WHITE_BRUSH);			

	// Display the background image
	DisplayBitmap(&gBuffer, hBackground, 0, 0);
}

void DeInit()
{
	// Free the sprite we created
	FreeSprite(&gMonster);

	// Free the background image
	DeleteObject(hBackground);

	// Free the back buffer
	FreeBuffers(&gBuffer);

	// Post the QUIT message to the window (0 = WM_QUIT)															
	PostQuitMessage(0);				
}

////////////////////////////////////////////////////////////////////
//
// *WARNING!!!*
//  
// The function, TransparentBlit() is a dangerous one.  You might be better
// off writting your own, or doing a mask blit (explained in another tutorial).
// TransparentBlit() supposadly only works on win2k and windows 98.  BUT, on
// windows98 it leaks massive amounts of memory.  This is not the best function
// to use.  You can replace it with DisplayPortionOfBitmap(), but then it won't
// be transparent....  It's a cool function, but too bad it leaks memory. :(
// After a while, the monster will dissapear and eventually will probably crash
// if you are running windows 98.  it will just keep sucking up GDI resource in memory.
// 
// *Quick Notes*
//
// Now we have a creature that walks around the screen animating from one file.
// If you right click your mouse, he changes direction (or states...)
// You can have multiple sprites too!! This is a lot of code to digest in one 
// sitting.  Just take parts of it and try to get it to work.  I designed this
// myself, so there is a ton of other ways to make some sort of sprite routine.
// C++ is really nice, because you don't have to call a free functions for every
// sprite or buffer, etc... it takes care of it in the deConstructor.
// 
// I made the window so you couldn't resize it by passing in the WS_SYSMENU flag
// at the top when I create the window.  Since the bitmap is only as big as the 
// initial window, it would look strange if you resized it.  I created a unused
// function called DrawBackgroundImage() that stretches the background image
// to the client rectangle of the screen, but if we are erasing the sprite with
// a part of the screen, it doesn't work because the screen is stretched.  You
// can use that for something else if you want.
//
// *Steps For Using A Sprite*
//
// 1) Define the sprite like so:  SPRITE gMonster;
//
// 2) Load the sprite: LoadSprite(the sprite structure, 
//								  the bitmap to use, the width of a frame, 
//								  the height of a frame, 
//								  amount of frames per animation sequence);
//
// 3) Set values by these functions: SetSpriteState(), SetSpritePosition(), SetSpriteSpeed();
//
// 4) To animate it, stick this function in your game loop:
//				
//			AnimateSprite(the buffer, the sprite structure);
//
// 5) If you want to move it, call this: MoveSprite(the sprite structure);
//    The MoveSprite() function is something you will want to change most likely.
//    I just have it so if they hit the right side or bottom of the screen, they wrap.
//    Your program will differ in it's actions most likely (I hope so!)
//
// 6) Once you have swapped the buffers, you can call this to erase the sprite:
//
//			EraseSprite(the buffer, the sprite, the background image);
//	
//    This takes a chunk from the background image and draws it over the sprite.
//    This is so you don't have to redraw the whole background every time.
//
// 7) Then, after you are done with your program, free it:  FreeSprite(the sprite structure)
//
// And that's pretty much it!
//
// *What Is Transparency???*
//
// If you go and look at the bitmap "monster.bmp", you will see that there is
// black all around the monster in each frame of animation.  Well, we can
// say that we only want to blit the colors that ARENT black.  The color
// just happens to be RGB (0, 0, 0) which is PURE black.  So when we call
// TransparentBlit() (this function is called in DisplayTransparentBitmap()) then
// we tell it that we want to ignore all the colors in the bitmap that have the
// RGB value of (0, 0, 0).  RGB is the Red Green Blue values that make up color.
// So now, when the bitmap is display, it doesn't show the black outlining in the frame,
// This is good because it would look like a blocky creature.  
//
// To see the difference, Call DisplayPortionOfBitmap instead of DisplayTransparentBitmap.
// The create will have all the black around him.  Once again, the transparent function
// have an unknown memory leak in it, which suchs because it is an awesome function.
// In another tutorial we will create our own transparent function.
//
// © 2000-2005 GameTutorials	       



