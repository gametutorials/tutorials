// Done by TheTutor

/*
	Loading files is always a tad tricky -- One of the harder parts is getting a description
	of how the file is laid out.  If you don't know how the file is laid out, it's going to
	be pretty hard to load -- MSDN is where I looked up how a .bmp is laid out

	8-bit bitmaps can support 256 colors.  8-bit bitmaps pixel data, are actually indexes
	into an array of colors (RGBQUAD) associated with the bitmap.  8-bit bitmaps also 
	have the ability to be compressed.  This tutorial will teach you how to load 
	uncompressed 8-bit bitmaps.
	
	Now, lets get loading :)	

*/

// These will be the upper-left coordinates of where the 8-bit bitmap will be blit to
#define XPOS 100		
#define YPOS 100

#include "bitmap_class.h"

// Window Globals
HDC gHDC; // Window's HDC
CBitmap gBitmap; // Used for holding the 8-bit .bmp

// WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprev, PSTR cmdline, int ishow)
{
    HWND hwnd;
    MSG msg;
    WNDCLASSEX wndclassex = {0};
	
	char class_name[] = "GameTutorials_8bitBMP";
	
	// Init the fields we care about
	wndclassex.cbSize = sizeof(WNDCLASSEX);
    wndclassex.style = CS_HREDRAW | CS_VREDRAW;
    wndclassex.lpfnWndProc = WinProc;
    wndclassex.hInstance = hinstance;
    wndclassex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wndclassex.lpszClassName = class_name;
    wndclassex.hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(IDC_ARROW), IMAGE_CURSOR,
											0, 0, LR_SHARED); // Load the default arrow cursor

	RegisterClassEx(&wndclassex);

	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
						  class_name,
						  "www.GameTutorials.com -- Loading 8-bit bitmaps",
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
		
	gHDC = GetDC(hwnd);

	// Capture window's HDC
	if(!gHDC)
	{
		MessageBox(hwnd,"This is not good!  Bailing ship!","**ERROR**",MB_OK | MB_ICONERROR);
			return EXIT_FAILURE;
	}

	if(gBitmap.loadBMP("GameTutorials.bmp",gHDC) == false)
	{
		MessageBox(hwnd,"Couldn't load bitmap -- Who's the funny guy???","Error",MB_OK);
			return EXIT_FAILURE;
	}
	
	// Show and update window
    ShowWindow(hwnd, ishow);
    UpdateWindow(hwnd);
	
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
			// Heavy computational stuff goes here
		}

	} // end of while(1)

	ReleaseDC(hwnd,gHDC); // Release the window's HDC
	UnregisterClass(class_name,hinstance); // Free mem associated with WNDCLASSEX
		return msg.wParam;
}

// The WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    PAINTSTRUCT ps;
		
	switch(message)
    {
		case WM_PAINT:
			
			BeginPaint(hwnd, &ps);
			
			// Blit the pretty picture
			BitBlt(gHDC,XPOS,YPOS,gBitmap.getWidth(),gBitmap.getHeight(),
				   gBitmap.getHDC(),0,0,SRCCOPY);

			EndPaint(hwnd, &ps);
				return 0;

		case WM_DESTROY:

			PostQuitMessage(0);
				return 0;
    
	} // end of switch(message)

    return DefWindowProc(hwnd, message, wparam, lparam);
}

// 8-Bit Goodness --

/*

	Now you know some things about loading 8-bit .bmps.  Why use an 8-bit when  
	any monitor on the planet that is worth a pile of poop supports 32-bit color?  The answer
	is simple, space.  There may come a day when space is no longer a concern, but that day
	is far ahead in the future.  If you happen to be writing a program that needs to be
	space conscious, be sure to use 8-bit images to your advantage.

	Questions, comments, concerns?  Send 'em our way at www.GameTutorials.com

*/

/*----------------------------*\
|  TheTutor                    |
|  thetutor@gametutorials.com  |
|  © 2000-2005 GameTutorials   |
\*----------------------------*/
