#include "main.h"

//////////////////////////////////////////////////////////////////////////
/////
///// FUNCTION:    CreateDoubleBuffering()
///// DESCRIPTION: This sets up the double buffering by filling in the 
/////			   buffer structure that is passed in.
///// INPUT:       hwnd    - The window handle
///// OUTPUT:      pBuffer - The back buffer structure to fill in
///// RETURN:      nothing
/////
//////////////////////////////////////////////////////////////////////////

void CreateDoubleBuffering(BUFFER *pBuffer, HWND hwnd)
{
	// Assign the window handle to our structure
	pBuffer->hwnd = hwnd;									
	
	// Store the client rectangle in our structure
	GetClientRect(hwnd, &(pBuffer->scrnRect));

	// Get a handle to the device context and store it as the front buffer
	pBuffer->hdcFront = GetDC(pBuffer->hwnd);

	// Create a compatible device context for the backbuffer
	pBuffer->hdcBack = CreateCompatibleDC(pBuffer->hdcFront);

	// Create another hdc to store the bitmap in before the backbuffer
	pBuffer->hdcBitmap = CreateCompatibleDC(pBuffer->hdcFront);

	// Create a dummy bitmap handle to store in the front buffer
	pBuffer->hCompBitmap = CreateCompatibleBitmap(pBuffer->hdcFront, pBuffer->scrnRect.right, pBuffer->scrnRect.bottom);

	// Select the dummy bitmap handle into the back buffer to make it like the front hdc
	pBuffer->hOldBitmap = (HBITMAP)SelectObject(pBuffer->hdcBack, pBuffer->hCompBitmap);	
}

//////////////////////////////////////////////////////////////////////////
/////
///// FUNCTION:    LoadABitmap()
///// DESCRIPTION: This loads a bitmap and returns a handle to that bitmap.
///// INPUT:       szFileName - The name of the file to load (Ie. "Bitmap.bmp")
///// OUTPUT:      nothing
///// RETURN:      HBITMAP - The handle that references the bitmap loaded
/////
//////////////////////////////////////////////////////////////////////////

HBITMAP LoadABitmap(LPSTR szFileName)
{
	// Load the bitmap and return the handle to the bitmap we just loaded
	return (HBITMAP)LoadImage(NULL, szFileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
}

//////////////////////////////////////////////////////////////////////////
/////
///// FUNCTION:    DisplayBitmap()
///// DESCRIPTION: This puts the bitmap in the backbuffer
///// INPUT:       hBitmap - The handle to the bitmap to display
/////			   x, y    - The X and Y coordinates to display the bitmap
///// OUTPUT:      pBuffer - The back buffer that holds the buffer information
///// RETURN:      nothing
/////
//////////////////////////////////////////////////////////////////////////

void DisplayBitmap(BUFFER *pBuffer, HBITMAP hBitmap, int x, int y)
{
	// Select the bitmap handle into the extra hdc that holds the bitmap
	HBITMAP hOldBitmap = (HBITMAP) SelectObject(pBuffer->hdcBitmap, hBitmap);

	// Blit the bitmap hdc into the backbuffer
	BitBlt(pBuffer->hdcBack, x, y, pBuffer->scrnRect.right, 
		   pBuffer->scrnRect.bottom,  pBuffer->hdcBitmap, 0, 0, SRCCOPY);

	// Select the old handle to the bitmap back where it was to clean up
	SelectObject(pBuffer->hdcBitmap, hOldBitmap);
}

//////////////////////////////////////////////////////////////////////////
/////
///// FUNCTION:    DisplayPortionOfBitmap()
///// DESCRIPTION: This takes a portion of a bitmap and puts it in the backbuffer
///// INPUT:       hBitmap - The handle to the bitmap to display
/////			   x, y    - The X and Y coordinates to display the bitmap
/////			   rPortion - The portion of the bitmap to be displayed
///// OUTPUT:      pBuffer - The back buffer that holds the buffer information
///// RETURN:      nothing
/////
//////////////////////////////////////////////////////////////////////////

void DisplayPortionOfBitmap(BUFFER *pBuffer, HBITMAP hBitmap, int x, int y, RECT rPortion)
{
	// Assign the width and height to shorten variables. 
	int width = rPortion.right - rPortion.left, height = rPortion.bottom - rPortion.top;

	// Select the bitmap handle into the extra hdc that holds the bitmap
	HBITMAP hOldBitmap = (HBITMAP) SelectObject(pBuffer->hdcBitmap, hBitmap);

	// Blit the bitmap hdc into the backbuffer using the transparent color 0 (black)
	BOOL result = BitBlt(pBuffer->hdcBack, x, y, width, height, 
				   pBuffer->hdcBitmap, rPortion.left, rPortion.top, SRCCOPY);

	// Select the old handle to the bitmap back where it was to clean up
	SelectObject(pBuffer->hdcBitmap, hOldBitmap);
}


void TransparentBlit(HDC hdc, int destX, int destY, int destWidth, int destHeight, 
					 HDC hdc2, int srcX, int srcY, UINT tranparency)
{
	unsigned char* pImageBits;
	unsigned char* pBackBits;
	BITMAPINFO bmBitmapInfo = {0};
	HBITMAP hBitmap, hBitmap2, hOldBitmap, hOldBitmap2;
	HDC compHDC;
	HDC compHDC2;
	
	// Create a valid address for our image bits
	pImageBits = (unsigned char *)malloc(sizeof(unsigned char));
	memset(pImageBits, 0, sizeof(unsigned char));

	// Fill in our BitmapInfo structure (we want a 24 bit image)
	bmBitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmBitmapInfo.bmiHeader.biCompression = BI_RGB;
	bmBitmapInfo.bmiHeader.biHeight = destHeight;
	bmBitmapInfo.bmiHeader.biWidth = destWidth;	
	bmBitmapInfo.bmiHeader.biBitCount = 24;
	bmBitmapInfo.bmiHeader.biClrUsed = 0;
	bmBitmapInfo.bmiHeader.biPlanes = 1;

	// Create 2 DIB Sections.  One for the Front Buffer and one for the BackBuffer
	hBitmap  = CreateDIBSection(hdc,  &bmBitmapInfo, DIB_RGB_COLORS,(void **)&pImageBits, 0,0);	
	hBitmap2  = CreateDIBSection(hdc,  &bmBitmapInfo, DIB_RGB_COLORS,(void **)&pBackBits, 0,0);

	// Create a compatible DC for the front buffer and Select our Dib Section into it
	compHDC = CreateCompatibleDC(hdc);
	hOldBitmap = (HBITMAP)SelectObject(compHDC, hBitmap);
	
	// Create a compatible DC for the back buffer and Select our Dib Section into it
	compHDC2 = CreateCompatibleDC(hdc2);
	hOldBitmap2 = (HBITMAP)SelectObject(compHDC2, hBitmap2);

	// Blit the front buffer to our compatible DC that will hold the destination bits
	BitBlt(compHDC, 0, 0, destWidth, destHeight, hdc, destX, destY, SRCCOPY);

	// Blit the back buffer to our compatible DC that will hold the source bits
	BitBlt(compHDC2, 0, 0, destWidth, destHeight, hdc2, srcX, srcY, SRCCOPY);

	// Loop through the 24 bit image (Times 3 for R G and B)
	for(int i = 0; i < destHeight * destWidth * 3; i += 3)
	{
		// Check if the current pixel being examined isn't the transparent color
		// Remember, the image bits are stored (Blue, Green, Red), not (Red, Green, Blue)
		// We use the system macros to abstract the R G B data
		if((pBackBits[i]   != GetBValue(tranparency)) || 
		   (pBackBits[i+1] != GetGValue(tranparency)) || 
		   (pBackBits[i+2] != GetRValue(tranparency)))
		{
			// Assign the desired pixel to the foreground
			pImageBits[i]     = pBackBits[i];
			pImageBits[i + 1] = pBackBits[i + 1];
			pImageBits[i + 2] = pBackBits[i + 2];
		}
	}  

	// Blit the transparent image to the front buffer (Voila!)
	BitBlt(hdc, destX, destY, destWidth, destHeight, compHDC, 0, 0, SRCCOPY);

	// Cleanup the monochrome bitmaps
	SelectObject(compHDC, hOldBitmap);
	SelectObject(compHDC2, hOldBitmap2);	

	// Free GDI Resources
	DeleteObject(hBitmap);
	DeleteObject(hBitmap2);
	DeleteDC(compHDC);
	DeleteDC(compHDC2);
}



//////////////////////////////////////////////////////////////////////////
/////
///// FUNCTION:    DisplayTransparentBitmap()
///// DESCRIPTION: This takes a portion of a bitmap and puts it in the backbuffer
/////			   BUT, not displaying any color with rgb values of (0, 0, 0).
/////			   The problem with this function is that is leaks memory in Win98
///// INPUT:       hBitmap - The handle to the bitmap to display
/////			   x, y    - The X and Y coordinates to display the bitmap
/////			   rPortion - The portion of the bitmap to be displayed
///// OUTPUT:      pBuffer - The back buffer that holds the buffer information
///// RETURN:      nothing
/////
//////////////////////////////////////////////////////////////////////////

void DisplayTransparentBitmap(BUFFER *pBuffer, HBITMAP hBitmap, int x, int y, RECT rPortion)
{
	// Assign the width and height to shorten variables. 
	int width = rPortion.right - rPortion.left, height = rPortion.bottom - rPortion.top;

	// Select the bitmap handle into the extra hdc that holds the bitmap
	HBITMAP hOldBitmap = (HBITMAP) SelectObject(pBuffer->hdcBitmap, hBitmap);

	// Blit the bitmap hdc into the backbuffer using the transparent color 0 (black)
	TransparentBlit(pBuffer->hdcBack, x, y, width, height, 
				    pBuffer->hdcBitmap, rPortion.left, rPortion.top, TRANSPARENT_COLOR);

	// Select the old handle to the bitmap back where it was to clean up
	SelectObject(pBuffer->hdcBitmap, hOldBitmap);
}

//////////////////////////////////////////////////////////////////////////
/////
///// FUNCTION:    DrawBackgroundImage()
///// DESCRIPTION: This draws and if desired, stretches an image to the backbuffer
///// INPUT:       hBitmap - The handle to the bitmap to display
/////			   rPortion - The portion of the bitmap to be displayed
///// OUTPUT:      pBuffer - The back buffer that holds the buffer information
///// RETURN:      nothing
/////
//////////////////////////////////////////////////////////////////////////

void DrawBackgroundImage(BUFFER *pBuffer, HBITMAP hBitmap)
{
	// This will hold our information on the bitmap
	BITMAP bitmapInfo;

	// Select the bitmap handle into the extra hdc that holds the bitmap
	HBITMAP hOldBitmap = (HBITMAP) SelectObject(pBuffer->hdcBitmap, hBitmap);

	// Get the width and height of the background bitmap
	GetObject(hBitmap, sizeof(BITMAP), &bitmapInfo);

	// Blit the bitmap hdc into the backbuffer using the transparent color 0 (black)
	BOOL result = StretchBlt(pBuffer->hdcBack, 0, 0, pBuffer->scrnRect.right, pBuffer->scrnRect.bottom, 
				   pBuffer->hdcBitmap, 0, 0, bitmapInfo.bmWidth, bitmapInfo.bmHeight, SRCCOPY);

	// Select the old handle to the bitmap back where it was to clean up
	SelectObject(pBuffer->hdcBitmap, hOldBitmap);
}

//////////////////////////////////////////////////////////////////////////
/////
///// FUNCTION:    ClearScreen()
///// DESCRIPTION: This fills the rectangle passed in with a specified color
///// INPUT:       hdc      - The handle to the device context to fill
/////              scrnRect - The screens rectangle to fill in
/////			   color    - The color to fill the rectangle in (Ie, WHITE_BRUSH)
///// OUTPUT:      nothing
///// RETURN:      nothing
/////
//////////////////////////////////////////////////////////////////////////

void ClearScreen(HDC hdc, RECT scrnRect, int color)
{
	// Fill the backbuffer with white to erase the previous position of the bitmap
	FillRect(hdc, &scrnRect, (HBRUSH)GetStockObject(color));
}

//////////////////////////////////////////////////////////////////////////
/////
///// FUNCTION:    SwapBackBuffer()
///// DESCRIPTION: This flips the backbuffer to the front of the screen,
/////			   which displays the bitmaps to the screen.
///// INPUT:       bClearBackBuffer - A flag to clear the backbuffer or not
///// OUTPUT:      pBuffer - The back buffer info to swap
///// RETURN:      nothing
/////
//////////////////////////////////////////////////////////////////////////

void SwapBackBuffer(BUFFER *pBuffer, BOOL bClearBackBuffer)
{
	// Blit the whole backbuffer to the screen (The front buffer)
	BitBlt(pBuffer->hdcFront, pBuffer->scrnRect.left, pBuffer->scrnRect.top, 
		   pBuffer->scrnRect.right, pBuffer->scrnRect.bottom, pBuffer->hdcBack, 0, 0, SRCCOPY);

	// If we want to clear the screen afterwards, set the background to white
	if(bClearBackBuffer) ClearScreen(pBuffer->hdcBack, pBuffer->scrnRect, WHITE_BRUSH);
}

//////////////////////////////////////////////////////////////////////////
/////
///// FUNCTION:    ResizeBuffers()
///// DESCRIPTION: Changes the size of the front and back buffers according 
/////			   to the new client size of the window.
///// INPUT:       nothing
///// OUTPUT:      pBuffer - The back buffer info to resize
///// RETURN:      nothing
/////
//////////////////////////////////////////////////////////////////////////

void ResizeBuffers(BUFFER *pBuffer)
{
	// Get the new client rectangle of the window
	GetClientRect(pBuffer->hwnd, &(pBuffer->scrnRect));
			
	// Select the old bitmap back into the back buffer
	SelectObject(pBuffer->hdcBack, pBuffer->hOldBitmap);

	// Delete the old compatible bitmap
	DeleteObject(pBuffer->hCompBitmap);

	// Create a new compatible bitmap with the new size of the window
	pBuffer->hCompBitmap = CreateCompatibleBitmap(pBuffer->hdcFront, pBuffer->scrnRect.right, pBuffer->scrnRect.bottom);

	// Select the new compatible bitmap into the backbuffer and store the old one again
	pBuffer->hOldBitmap = (HBITMAP) SelectObject(pBuffer->hdcBack, pBuffer->hCompBitmap);	
}

//////////////////////////////////////////////////////////////////////////
/////
///// FUNCTION:    FreeBuffers()
///// DESCRIPTION: Frees the buffer for the window
///// INPUT:       nothing
///// OUTPUT:      pBuffer - The back buffer info to free
///// RETURN:      nothing
/////
//////////////////////////////////////////////////////////////////////////

void FreeBuffers(BUFFER *pBuffer)
{
	// Set the back buffer to it's original bitmap (This is necessary for clean-up)
	SelectObject(pBuffer->hdcBack, pBuffer->hOldBitmap);

	// Free the hbitmaps
	DeleteObject(pBuffer->hOldBitmap);
	DeleteObject(pBuffer->hCompBitmap);

	// Free all the HDC's
	ReleaseDC(pBuffer->hwnd, pBuffer->hdcBack);
	ReleaseDC(pBuffer->hwnd, pBuffer->hdcBitmap);
	ReleaseDC(pBuffer->hwnd, pBuffer->hdcFront);
}


////////////////////////////////////////////////////////////
//
// *Quick Notes*
//
// This file stays the same since the last tutorial, but we
// use the double buffering structures and functions.
// We will most likely be including this will most of our next 
// tutorials
// 
//
