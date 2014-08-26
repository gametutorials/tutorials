#ifndef _BITMAP_H				// If we haven't included this file
#define _BITMAP_H				// Set a flag saying we included it     

#include <iostream>				// Include the standard C++ file
#include <windows.h>			// We need to include windows.h
using namespace std;			// Include the standard namespace															


struct BUFFER					// This is our back buffering structure
{
	HWND hwnd;					// This holds the current window's handle
	RECT scrnRect;				// This holds the client rectangle of the window
	HANDLE hCompBitmap;			// This holds the compatible bitmap for the backbuffer
	HANDLE hOldBitmap;			// This is used for storage to free when the program quits
	HANDLE hOldBitmap2;			// This is used as storage to swap between selected bitmaps when using selectObject()
	HDC hdcFront;				// This is the front buffer (The part we see)
	HDC hdcBack;				// This is the back buffer (the part we draw to, then flip)
	HDC hdcBitmap;				// This is a temp buffer to swap the bitmap back and forth from
};


////// * ////// * ////// * WINDOW FUNCTIONS * ////// * ////// * ////// *


// The function prototype for our WndProc (The function that handles all the windows messages)																																																																									
LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);		

// Loads the bitmaps and creates the double buffer
void Init(HWND hwnd);

// This frees memory and quits the program
void DeInit();


////// * ////// * ////// * BITMAP FUNCTIONS * ////// * ////// * ////// *


// Create the double buffering for all of our bitmaps
void CreateDoubleBuffering(BUFFER *pBuffer, HWND hwnd);

// This loads a bitmap (ie. "Bitmap.bmp") and returns a handle to that bitmap
HBITMAP LoadABitmap(LPSTR szFileName);

// This displays the bitmap to the screen at a X and a Y location
void DisplayBitmap(BUFFER *pBuffer, HBITMAP hBitmap, int x, int y);

// Clears the backbuffer to what ever color is passed in
void ClearScreen(HDC hdc, RECT scrnRect, int color);

// Swaps the back buffer to the screen
void SwapBackBuffer(BUFFER *pBuffer, BOOL bClearBackBuffer);

// Resizes the buffers and bitmaps according to the new window client rectangle
void ResizeBuffers(BUFFER *pBuffer);

#endif


////////////////////////////////////////////////////////////
//
// *Quick Notes*
//
// I seperated the top 3 functions from the bottom functions
// because you can take the top three out of this file and 
// include this file with every application you use bitmaps.
// 
// Now we will never need to right bitmap functions again 
// because you have them here (Unless you are using directX/OpenGL).
// These function should be able to work for most programs that
// Display bitmaps.  You can make tile games, screen effects,
// What ever you want... In the next tutorial you will see how
// to incorportate these functions with animation.
// 
// 
// 
// 
