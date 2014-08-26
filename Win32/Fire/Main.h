#ifndef _MAIN_H				// If we haven't included this file							
#define _MAIN_H				// Set a flag saying we included it

// In Visual Studio 2005 Microsoft added safer versions (denoted by a "_s" suffix) to many 
// common C/C++ run time functions.  Currently the ISO Standards Committee for C and C++ 
// is determining if they are going to add them to the official language set.  Until then, 
// this define will remove warnings about using the "unsafe" versions of these functions.
#define _CRT_SECURE_NO_DEPRECATE

#include <iostream>			// Include the standard C++ file
#include <windows.h>		// We need to include windows.h
#include "bitmap.h"
using namespace std;		// Include the standard namespace															

// Create a define for our window height
#define HEIGHT         240											
// Create a define for our window width
#define WIDTH          320							
// We want 30 frames per second for our animation speed
#define FRAME_RATE     30				

// The function prototype for our WndProc (The function that handles all the windows messages)																																																																									
LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);		

// Loads the bitmaps and creates the double buffer
void Init(HWND hwnd);

// This frees memory and quits the program
void DeInit();

#endif

