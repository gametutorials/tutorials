#ifndef _MAIN_H				// If we haven't included this file							
#define _MAIN_H				// Set a flag saying we included it

#include <iostream>			// Include the standard C++ file
#include <windows.h>		// We need to include windows.h
#include <mmsystem.h>		// We need this for timeGetTime()
#include "bitmap.h"			// Include the bitmap funciton prototypes first
#include "sprite.h"			// Inc;ude the sprite function prototypes second
using namespace std;		// Include the standard namespace															


// The function prototype for our WndProc (The function that handles all the windows messages)																																																																									
LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);		

// Loads the bitmaps and creates the double buffer
void Init(HWND hwnd);

// This frees memory and quits the program
void DeInit();

#endif


////////////////////////////////////////////////////////////
//
// *Quick Notes*
//
// From now on, in programs larger than 1 cpp file, always
// make a main.h.  Store all the random function prototypes here.
// Also, you can include all the files here, and then other .h
// files can include "main.h".  One bad thing about having one
// header file include all the files, is that if you change one
// header file, you have to recompile the whole project.  In this
// case, it's not a long compile so it doesn't matter.  But you
// would in the future want to consider this.
// 
// 
// 
// 
// 
// 
