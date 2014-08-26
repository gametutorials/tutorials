#ifndef _MAIN_H				// If we haven't included this file							
#define _MAIN_H				// Set a flag saying we included it

// In Visual Studio 2005 Microsoft added safer versions (denoted by a "_s" suffix) to many 
// common C/C++ run time functions.  Currently the ISO Standards Committee for C and C++ 
// is determining if they are going to add them to the official language set.  Until then, 
// this define will remove warnings about using the "unsafe" versions of these functions.
#define _CRT_SECURE_NO_DEPRECATE

#include <iostream>			// Include the standard C++ file
#include <windows.h>		// We need to include windows.h
#include "bitmap.h"			// Include the bitmap funciton prototypes first
#include "sprite.h"			// Include the sprite function prototypes second
using namespace std;		// Include the standard namespace															

// This is the number of X tiles for our board
#define BOARD_WIDTH  15
// This is the number of the Y tiles for our board
#define BOARD_HEIGHT 15

// This is the width in pixels of each tile
#define TILE_WIDTH		32
// This is the height in pixels of each tile
#define TILE_HEIGHT		32
// This is how many frames of animation our players's sprite has
#define PLAYER_FRAMES	1


////////////// TILE DEFINES ///////////////////

// This values represents a wall tile in our g_BoardArray
#define FLOOR 0
// This values represents a wall tile in our g_BoardArray
#define WALL  1

////////////// TILE DEFINES ///////////////////


// Create a define for our window height
#define HEIGHT         (TILE_HEIGHT * BOARD_HEIGHT)											
// Create a define for our window width
#define WIDTH          (TILE_WIDTH * BOARD_WIDTH)											

// We want 30 frames per second for our animation speed
#define FRAME_RATE     20				

// The function prototype for our WndProc (The function that handles all the windows messages)																																																																									
LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);		

// Loads the bitmaps and creates the double buffer
void Init(HWND hwnd);

// This frees memory and quits the program
void DeInit();

#endif

