#ifndef _MAIN_H				// If we haven't included this file							
#define _MAIN_H				// Set a flag saying we included it

#include <iostream>			// Include the standard C++ file
#include <windows.h>		// We need to include windows.h
#include "bitmap.h"			// Include the bitmap funciton prototypes first
#include "sprite.h"			// Include the sprite function prototypes second
using namespace std;		// Include the standard namespace															

// Create a define for our window height
#define HEIGHT         480											
// Create a define for our window width
#define WIDTH          640							
// We want 5 frames per second for our animation speed
#define FRAME_RATE     5				

// The function prototype for our WndProc (The function that handles all the windows messages)																																																																									
LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);		

// Loads the bitmaps and creates the double buffer
void Init(HWND hwnd);

// This frees memory and quits the program
void DeInit();

#endif

