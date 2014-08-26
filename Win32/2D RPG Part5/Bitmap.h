#ifndef _BITMAP_H				// If we haven't included this file
#define _BITMAP_H				// Set a flag saying we included it     

#include "main.h"				// Include our main header file
								
								// This is the RGB color NOT to display from a bitmap (BLACK)
#define TRANSPARENT_COLOR RGB(255, 255, 255)


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// This is our back buffering class
class CBuffer					
{
public:

	// Constructor (default initializing function) and deconstructor (deinit function)
	CBuffer();
	~CBuffer();

	// Create the double buffering for all of our bitmaps
	void CreateDoubleBuffering(HWND hWnd);

	// This loads a bitmap (ie. "Bitmap.bmp") and returns a handle to that bitmap
	HBITMAP LoadABitmap(LPSTR szFileName);

	// This displays the bitmap to the screen at a X and a Y location
	void DisplayBitmap(HBITMAP hBitmap, int x, int y);

	// This display a portion of a bitmap with a transparency to (0, 0, 0) (pure black)
	void DisplayTransparentBitmap(HBITMAP hBitmap, int x, int y);
	
	// This displays a bitmap with an opaque effect
	void DisplayOpaqueBitmap(HBITMAP hBitmap, int x, int y, int alpha);

	// Displays a portion of a bitmap to the backbuffer (Used from one bitmap animation)
	void DisplayBitmap(HBITMAP hBitmap, int x, int y, RECT rPortion);

	// This display a portion of a bitmap with a transparency to (0, 0, 0) (pure black)
	void DisplayTransparentBitmap(HBITMAP hBitmap, int x, int y, RECT rPortion);
	
	// This display a portion of a bitmap with a every other pixel opacity
	void DisplayOpaqueBitmap(HBITMAP hBitmap, int x, int y, int alpha, RECT rPortion);
	
	// This does the dirty pixel work to make an image see through (Opaque)
	void OpaqueBlit(HDC hdc, int destX, int destY, int destWidth, int destHeight, 
					HDC hdc2, int srcX, int srcY, int alpha);

	// This stretches the image to the window rectangle to fill up the background
	void DrawBackgroundImage(HBITMAP hBitmap);

	// Clears the backbuffer to what ever color is passed in
	void ClearScreen(int color);

	void SetPen(int type, int thickness, COLORREF color);

	// Swaps the back buffer to the screen
	void SwapBackBuffer(BOOL bClearBackBuffer);

	// This resizes the buffers to the current window size
	void ResizeBuffers();

	// This frees the back buffer information
	void FreeBuffers();

	// This returns the back buffer hdc
	HDC GetHDC()	{ return hdcBack; };

private:

	HWND hWnd;					// This holds the current window's handle
	RECT scrnRect;				// This holds the client rectangle of the window
	HBITMAP hCompBitmap;		// This holds the compatible bitmap for the backbuffer
	HBITMAP hOldCompBitmap;		// This is used for storage to free when the program quits
	HBITMAP hOldDispBitmap;		// This is used for storage to free when the program quits
	HDC hdcFront;				// This is the front buffer (The part we see)
	HDC hdcBack;				// This is the back buffer (the part we draw to, then flip)
	HDC hdcBitmap;				// This is a temp buffer to swap the bitmap back and forth from
	HPEN oldPen;				// This holds the original pen stored by windows
	HBRUSH oldBrush;			// This holds the original brush stored by windows
};

#endif

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


////////////////////////////////////////////////////////////////////
//
// *Quick Notes*
//  
// We changed this file from previous tutorials to a nice encapsulated class.
// We also added a function regarding setting pens (for our exits).
// 
// 
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//
// © 2000-2005 GameTutorials      
