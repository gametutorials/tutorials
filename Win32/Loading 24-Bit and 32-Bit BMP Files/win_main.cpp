// Done by TheTutor

/*
	Okay this tutorial is going to load a 24 or 32-bit .bmp file "manually" -- We're going to
	load it into our class CBitmap (high color bitmap) -- The class will also contain a few 
	other methods but it's basic functionality is to load 24 or 32-bit .bmp files :)

	A few lines down you see a #define for IMAGE_NAME -- If you change the "24" in IMAGE_NAME
	to "32" it will load the 32-bit image instead of the 24-bit image
*/

// These will be the upper-left coordinates of where the bitmap will be blit to
// Totally just picked some numbers out of the air :)
#define XPOS 200
#define YPOS 125

#include "bitmap_class.h"

#define CLASS_NAME "GT_BitmapLoader"
#define IMAGE_NAME "24Bit_Image.bmp" // Name of file to load

// WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprev, PSTR cmdline, int ishow)
{
	HDC win_hdc = NULL; // Used for holding a handle to the window's device context
	CBitmap bitmap; // Used for holding .bmp

    HWND hwnd;
    MSG msg;
    WNDCLASSEX wndclassex = {0};
	
	// Init the fields we care about
	wndclassex.cbSize = sizeof(WNDCLASSEX);
    wndclassex.style = CS_HREDRAW | CS_VREDRAW;
    wndclassex.lpfnWndProc = WinProc;
    wndclassex.hInstance = hinstance;
    wndclassex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wndclassex.lpszClassName = CLASS_NAME;
    wndclassex.hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(IDC_ARROW), IMAGE_CURSOR,
											0, 0, LR_SHARED);

	RegisterClassEx(&wndclassex); // Register the extended window class

	// Create the window
	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
						  CLASS_NAME,
						  "www.GameTutorials.com -- Loading 24-bit and 32-bit bitmaps",
						  WS_OVERLAPPEDWINDOW,
						  CW_USEDEFAULT,
						  CW_USEDEFAULT,
						  640,
						  480,
						  NULL,
						  NULL,
						  hinstance,
						  NULL);

		// Error Check
		if(!hwnd)
			return EXIT_FAILURE;
		
	// Capture window hwnd and HDC
	win_hdc = GetDC(hwnd);

		// Error Check
		if(!win_hdc)
			return EXIT_FAILURE;
		
	// Try and load the image
	if(bitmap.loadBMP(IMAGE_NAME,win_hdc) == false)
	{
		MessageBox(hwnd,"Couldn't load bitmap -- Who's the funny guy???","Error",MB_OK);
			return EXIT_FAILURE;	
	}

	// Show and update window
    ShowWindow(hwnd, ishow);
    UpdateWindow(hwnd);

	// Blit the pretty picture
	// **NOTE** any more drawing to window will draw over the pretty picture :)
	BitBlt(win_hdc,XPOS,YPOS,bitmap.getWidth(),bitmap.getHeight(),
		   bitmap.getHDC(),0,0,SRCCOPY);
	
	while(1)
	{
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
				break;
				
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// Do heavy computational stuff here
		}

	} // end of while(1)

	UnregisterClass(CLASS_NAME,hinstance); // Free mem associated with WNDCLASSEX
		return msg.wParam;
}

// The WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch(message)
    {
		case WM_DESTROY:

			PostQuitMessage(0);
				return 0;
    
	} // end of switch(message)

    return DefWindowProc(hwnd, message, wparam, lparam);
}

/*	24-Bit Color Me Badd

	24-bit and 32-bit bitmaps are great cause they give you TONS of colors.  But it's 
	important to keep one fact in mind, 24-bit and 32-bit bitmaps have the EXACT same
	number of colors between the two.  The only difference is that 32-bit bitmaps have
	an alpha value as well.  Moral of the story:  Use 32-bit bitmaps when you need alpha %)

	Still feeling a little unsure?  Too many colors hurt your eyes and brain?  Just remember
	you can always post your questions at www.GameTutorials.com 
*/

/*----------------------------*\
|  TheTutor                    |
|  thetutor@gametutorials.com  |
|  © 2000-2005 GameTutorials   |
\*----------------------------*/

	
