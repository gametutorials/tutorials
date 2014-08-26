// You should include this file and bitmap.cpp with your projects from now on.
// Now you don't need to cut and paste the functions you want to use, just include
// the .h and .cpp files to your project if you want to use bitmaps

#ifndef _BITMAP_H				// If we haven't included this file
#define _BITMAP_H				// Set a flag saying we included it     

#include "main.h"

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

// This resizes the buffers to the current window size
void ResizeBuffers(BUFFER *pBuffer);

#endif


////////////////////////////////////////////////////////////
//
// *Quick Notes*
//
// This needs to be include from now on in your projects if
// you want to use bitmaps, also bitmap.cpp.  This allows you
// to not have to copy and paste functions into your program,
// just include these files in your project.  This is also
// needed for animation if you are including sprite.cpp/sprite.h.
// 
// 
// 
// 
// 
// 
// 
// 
// 
