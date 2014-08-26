#ifndef _BITMAP_H
#define _BITMAP_H

#include <iostream>											// Include the standard C++ file
#include <windows.h>										// We need to include windows.h
using namespace std;										// Include the standard namespace															

#define HEIGHT 600											// Create a define for our window height
#define WIDTH  800											// Create a define for our window width

struct BUFFER
{
	HWND hwnd;
	RECT scrnRect;
	HANDLE hCompBitmap;
	HANDLE hOldBitmap;
	HANDLE hOldBitmap2;
	HDC hdcFront;
	HDC hdcBack;
	HDC hdcBitmap;	
};

// The function prototype for our WndProc (The function that handles all the windows messages)																																																																									
LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);		

// Loads the bitmaps and creates the double buffer
void Init(HWND hwnd);

// Create the double buffering for all of our bitmaps
void CreateDoubleBuffering(BUFFER *pBuffer, HWND hwnd);

// This loads a bitmap (ie. "Bitmap.bmp") and returns a handle to that bitmap
HBITMAP LoadAnImage(LPSTR szFileName);

// This displays the bitmap to the screen at a X and a Y location
void DisplayBitmap(BUFFER *pBuffer, HBITMAP hBitmap, int x, int y);

// Clears the backbuffer to what ever color is passed in
void ClearScreen(HDC hdc, RECT scrnRect, int color);

// Swaps the back buffer to the screen
void SwapBackBuffer(BUFFER *pBuffer, BOOL bClearBackBuffer);

#endif
