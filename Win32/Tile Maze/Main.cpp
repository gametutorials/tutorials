//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	DigiBen@gametutorials.com		 //
//																		 //
//		$Program:		Tile Maze										 //
//																		 //
//		$Description:	Creates a tiled maze with player collision		 //
//																		 //
//***********************************************************************//


#include "main.h"											// Include our own header file which holds all the include and structure/function prototype information

// Create our double buffering structure
BUFFER g_Buffer;												

// This will hold our player sprite
SPRITE g_sprPlayer;

// This 2D array will hold all of our tiles
HBITMAP g_bmpTiles[BOARD_WIDTH][BOARD_HEIGHT];

// This is our 2D array for our board.  We place a 1 where we want the wall texture.
// We place a 0 where we want to have the floor texture.
// Ideally, You will probably want to read something like this in from a file.
// If you want, you can make some more things in this maze.  Maybe 2
// could be a door, and 3 could be a person to talk too... you might also
// want to make this an array of char's instead, this would make it less cryptic.
int g_BoardArray[] =  { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
						1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
						1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1,
						1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
						1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 
						1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1,
						1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1,
						1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1,
						1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1,
						1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 0, 1, 1, 1,
						1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1,
						1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1,
						1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0,
						1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0,
						1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0}; // The end of the maze

																													
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
						 "Tile Maze",			  			// window's Title    
						 WS_SYSMENU | WS_POPUP,				// window style	- This style won't allow the window to resize
						 100,								// initial x position
						 100,								// initial y position
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
				// Go through out 2D array and display each bitmap at their respective location
				for(int y = 0; y < BOARD_HEIGHT; y++)
				{
					for(int x = 0; x < BOARD_WIDTH; x++)
					{
						// Display each title according to their index.
						// We take the x and y index and times it by the width and height
						// in pixels to get the tile position. This makes the nice tile effect we see on screen  
						DisplayBitmap(&g_Buffer, g_bmpTiles[x][y], TILE_WIDTH * x, TILE_WIDTH * y);  
					}
				}

				// Let's the draw our character over top of the tiles
				AnimateSprite(&g_Buffer, &g_sprPlayer);
				
				// Now swap the backbuffer to display it to the front and clear the screen
				SwapBackBuffer(&g_Buffer, FALSE);
			}

		}
    }

	// Free all the data and set everything back to normal
	DeInit();

	// Unregister the window class with the operating system
	UnregisterClass("Window Class",hInstance);
    
	// Quit the program
	return msg.wParam ;										
}

// Here is the WndProc that handles all the messages															
LRESULT CALLBACK WndProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// Get the old position of our sprite in case we collide, so we can restore it.
	POINT oldPosition = g_sprPlayer.position;
	POINT playerIndex = {0, 0};

	// Check which message was passed in
    switch (iMsg)											
    {
		// This message is sent when the window is created (CreateWindow())
		case WM_CREATE:

			// Create the double buffer and load the bitmaps	
			Init(hwnd);										
			break;	
	
		case WM_KEYDOWN:
			switch (wParam) 
			{
				case VK_LEFT:
					
					// Check if we won't be going off the screen
					if( (g_sprPlayer.position.x - g_sprPlayer.speed.x) >= 0)
					{
						// Set our direction to WEST, then Move our player
						SetSpriteState(&g_sprPlayer, WEST);
						MoveSprite(&g_sprPlayer);
					}
					break;
				case VK_RIGHT:

					// Check if we won't be going off the screen
					if( (g_sprPlayer.position.x + g_sprPlayer.speed.x) < WIDTH)
					{
						// Set our direction to EAST, then Move our player
						SetSpriteState(&g_sprPlayer, EAST);
						MoveSprite(&g_sprPlayer);
					}
					break;
				case VK_UP:

					// Check if we won't be going off the screen
					if( (g_sprPlayer.position.y - g_sprPlayer.speed.y) >= 0)
					{
						// Set our direction to NORTH, then Move our player
						SetSpriteState(&g_sprPlayer, NORTH);
						MoveSprite(&g_sprPlayer);
					}
					break;
				case VK_DOWN:

					// Check if we won't be going off the screen
					if( (g_sprPlayer.position.y + g_sprPlayer.speed.y) < HEIGHT)
					{
						// Set our direction to SOUTH, then Move our player
						SetSpriteState(&g_sprPlayer, SOUTH);
						MoveSprite(&g_sprPlayer);
					}
					break;
				case VK_ESCAPE:
					
					// Tell the window we want to CLOSE
					SendMessage(hwnd, WM_CLOSE, 0, 0);
					break;
			}

			// Here we divide the x and y positon by the title size 
			// to get an index into g_BoardArray[].  Now it is like how we
			// did it in our ConsoleMaze Tutorial.  We index into the grid.
			playerIndex.x = g_sprPlayer.position.x / TILE_WIDTH;
			playerIndex.y = g_sprPlayer.position.y / TILE_HEIGHT;

			// Check if we collided with a wall (Used the 2D to 1D index conversion)
			if(g_BoardArray[playerIndex.x + playerIndex.y * BOARD_WIDTH] == WALL)
			{
				// Restore the player's position with it's old position
				SetSpritePosition(&g_sprPlayer, oldPosition.x, oldPosition.y);
			}
			break;

		// This message is sent when the window is destroyed
		case WM_DESTROY:									
			
			// Tell the window we need to close
			PostQuitMessage(0);							
			break;				
	}													

	// Process/handle the default messages
	return DefWindowProc (hwnd, iMsg, wParam, lParam);		
															
}

void Init(HWND hwnd)
{	
	// Create our double buffering
	CreateDoubleBuffering(&g_Buffer, hwnd);

	// Load the players bitmap
	LoadSprite(&g_sprPlayer, "player.bmp", TILE_WIDTH, TILE_HEIGHT, PLAYER_FRAMES);
	// Set the sprites position
	SetSpritePosition(&g_sprPlayer, 0, 0);

	// Set the X and Y speed of the monster
	SetSpriteSpeed(&g_sprPlayer, TILE_WIDTH, TILE_HEIGHT);

	// Read in the titles, depending on our g_BoardArray
	for(int y = 0; y < BOARD_HEIGHT; y++)
	{
		for(int x = 0; x < BOARD_WIDTH; x++)
		{
			//Do a switch on the value of the current index
			// Since we are using a 1D array, we use the equation:
			// x + y * BOARD_WIDTH  - This allows us to treat it like a 2D array
			switch(g_BoardArray[x + y * BOARD_WIDTH])
			{
				case WALL:
					// Load a wall tile into this index
					g_bmpTiles[x][y] = LoadABitmap("wall.bmp");
					break;
				case FLOOR:
					// Load a floor tile into this index
					g_bmpTiles[x][y] = LoadABitmap("floor.bmp");
					break;
				//case ETC...
					// Add as many tile types as you want!
			}
		}
	}
														
	// Set the backbuffer to white first (This clears the backbuffer)
	ClearScreen(g_Buffer.hdcBack, g_Buffer.scrnRect, WHITE_BRUSH);			
}

void DeInit()
{
	// Free the sprite we created
	FreeSprite(&g_sprPlayer);

	// Go through and free all of our tiles
	for(int y = 0; y < BOARD_HEIGHT; y++)
	{
		for(int x = 0; x < BOARD_WIDTH; x++)
		{
			// Free the tile stored in this index
			DeleteObject(g_bmpTiles[x][y]);
		}
	}

	// Free the back buffer
	FreeBuffers(&g_Buffer);
}

////////////////////////////////////////////////////////////////////
//
// *Quick Notes*
//  
// There you have it! There is a tile maze for you to play with.
// Once again, we would recommend that you read in something like this
// from a file.  That way you can change your levels by editing a text file.
// 
// We didn't change any code in Bitmap.cpp or Sprite.cpp, so you can just
// focus on this file.  We did however, add some #define's in main.h.
// 
// Basically, what we did to make tiles it read in 2 different types of bitmaps.
// We happen to know that they are 32 x 32 (pixels).  Then, we just set them
// next to each other across the screen.  We set our players speed to be the size
// of the tiles in pixels.  That way we move to a new tile each time we move.  See
// if you can get the player to smoothly move from tile to tile, without jumping.
// 
// To check for collision, all we did was take the players position, then divided
// it by the tile width and height for the X and Y index into the board array.
// Just like the ConsoleMaze tutorial, we use that index to see if we are on the
// same index as a wall, if we are... then just restore the old position of the player
// before it moved.
//
// The controls for this APP is:
//
//	- ARROW KEYS : Move the player up down left and right
//  - ESCAPE KEY : Quits the maze
// 
// Let us know what you think of this tutorial.  We are interested to see if this helps!
// 
// DigiBen
// www.GameTutorials.com
//
// © 2000-2005 GameTutorials      
