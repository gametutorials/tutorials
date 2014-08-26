#ifndef _BITMAP_H				// If we haven't included this file
#define _BITMAP_H				// Set a flag saying we included it     

#include "main.h"
								
								// This is the RGB color NOT to display from a bitmap (BLACK)
#define TRANSPARENT_COLOR RGB(0, 0, 0)

struct BUFFER					// This is our back buffering structure
{
	HWND hwnd;					// This holds the current window's handle
	RECT scrnRect;				// This holds the client rectangle of the window
	HANDLE hCompBitmap;			// This holds the compatible bitmap for the backbuffer
	HANDLE hOldBitmap;			// This is used for storage to free when the program quits
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

// Displays a portion of a bitmap to the backbuffer (Used from one bitmap animation)
void DisplayPortionOfBitmap(BUFFER *pBuffer, HBITMAP hBitmap, int x, int y, RECT rPortion);

// This display a portion of a bitmap with a transparency to (0, 0, 0) (pure black)
void DisplayTransparentBitmap(BUFFER *pBuffer, HBITMAP hBitmap, int x, int y, RECT rPortion);

// This stretches the image to the window rectangle to fill up the background
void DrawBackgroundImage(BUFFER *pBuffer, HBITMAP hBitmap);

// Clears the backbuffer to what ever color is passed in
void ClearScreen(HDC hdc, RECT scrnRect, int color);

// Swaps the back buffer to the screen
void SwapBackBuffer(BUFFER *pBuffer, BOOL bClearBackBuffer);

// This resizes the buffers to the current window size
void ResizeBuffers(BUFFER *pBuffer);

// This frees the back buffer information
void FreeBuffers(BUFFER *pBuffer);

#endif

