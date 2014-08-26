//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	digiben@gametutorials.com		 //
//																		 //
//		$Program:		Display Bitmaps (DIB)							 //
//																		 //
//		$Description:	Displays a bitmap to the screen at an x, y		 //
//																		 //
//***********************************************************************//

#include "DisplayBitmap.h"									// Include our own header file which holds all the include and structure/function prototype information
																														
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)			// Anyway, so here is our "main()" for windows.  Must Have this for a windows app.
    {														
    HWND        hwnd;										// Create the handle to the window.  
    MSG         msg;										// Create the variable to hold the window messages
    WNDCLASSEX  wndclass;									// Create the structure that holds the attributes of the window

    wndclass.cbSize        = sizeof (wndclass);				// Set the size of the class (required)
    wndclass.style         = CS_HREDRAW | CS_VREDRAW;		// Set the style of the window to redraw horizontally and vertically.
    wndclass.lpfnWndProc   = WndProc;						// Assign the callback function to our window.
    wndclass.cbClsExtra    = 0;								// We don't need any extra bytes
    wndclass.cbWndExtra    = 0;								// We don't need any extras
    wndclass.hInstance     = hInstance;						// Assign the hInstance passed from the OS to the window

    wndclass.hIcon         = LoadIcon (NULL, IDI_WINLOGO);	// Set the windows logo Icon
    wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW);	// Set the default arrow to the window
															// Set the background to white
    wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH); 

    wndclass.lpszMenuName  = NULL;							// Set the menu to NULL
    wndclass.lpszClassName = "Window Class";				// Set the name of our windows class
    wndclass.hIconSm       = LoadIcon (NULL, IDI_WINLOGO);	// Assign the small icon to the windows logo

	RegisterClassEx (&wndclass);							// Register the window class with the operating system
															
															// Now, we actually create the window
    hwnd = CreateWindow ("Window Class",					// window class name 
						 "Bitmap Background",	  			// window's Title    
						 WS_OVERLAPPEDWINDOW,				// window style		 
						 CW_USEDEFAULT,						// initial x position
						 CW_USEDEFAULT,						// initial y position
						 CW_USEDEFAULT,						// initial x size	 
						 CW_USEDEFAULT,					    // initial y size	 
						 NULL,								// This is the parent window handle.  
						 NULL,								// This is the window menu handle
						 hInstance,						    // This is the programs instance handle.
						 NULL);								// We don't want to pass any extra data in, so NULL

    ShowWindow (hwnd, iCmdShow);							// This shows our window. 
    UpdateWindow (hwnd);									// This pretty much paints our window to the screen.
															
	while (GetMessage (&msg, NULL, 0, 0))					// Here is our main loop. 
    {														
		TranslateMessage (&msg);							// This Translates messages so windows understands them.
		DispatchMessage (&msg);								// This sends the messages to the WndProc().
    }
    
	UnregisterClass("Window Class",hInstance);				// Unregister the window class with the operating system 

	return msg.wParam ;										// Quit the program
}

															// Here is the WndProc that handles all the messages															
LRESULT CALLBACK WndProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;												// Create a handle to the device context (We need this for every graphic operation)
	static BMP Bmp;											// Create our structure that will hold the bitmap info.
															// We make it static because we don't want it to change.
															// A static variable is initialized, but is not freed (erased) when the function ends.
															// We could just make it global by putting outside of any function, but globals are dangerous.
															// If we didn't have this static it would read in the bitmap, and then when this function finished,
															// It would erase everything and then it wouldn't have any info on the bitmap.  Try deleting the static to see what I mean.
	PAINTSTRUCT ps;
	RECT windowRect;
	static int x=0, y=0;

    switch (iMsg)											// Check which message was passed in
    {

		case WM_CREATE:										// Here we load our bitmap (This happens only ONCE).  "Bitmap.bmp" is our bitmap name.
			if(!LoadABitmap(&Bmp, "Bitmap.bmp"))			// We pass in the address to the BMP structure we created.  This will fill in the data.
			{												// Display a error message if we can't find the bitmap.
					MessageBox(hwnd, "Can't find Bitmap.bmp! (Make sure it's in the same directory as the .cpp file)", "Error!", MB_OK);
					exit(0);								// Quit the program if the bitmap is not found!
			}												// The bitmap must be in the same directory as your .cpp file, or .exe if you don't have the code.
			break;											// LoadABitmap() returns a FALSE if we can't find the bitmap.

		case WM_MOUSEMOVE:
			x = LOWORD(lParam);								// Get the x position from the LOW bits of the lParam  
			y = HIWORD(lParam);								// Get the y position from the HIGH bits of the lParam  
																	
															// Once again, the LOWORD and HIWORD is a way to extract half of the total bits.
															// Reviewing, bits are 1's and 0's.  So, 1101 is 4 bits.  11000110 8 bits (also 1 byte).
															// The HIWORD and LOWORD macro's just access half of those bits.  For instance:
															// If we have, "1101"   , the HI would be "11" and the LO would be "01".  It just cuts it in half.
															// A DWORD is 32-bits, so the low 16 bits hold the x position, and the top 16 bits hold the y position.

			GetClientRect(hwnd, &windowRect);				// Get the rectangle of the window
			InvalidateRect(hwnd, &windowRect, TRUE);		// This function clears the window's rectangle.  It essentially forces a WM_PAINT message.
			break;

		case WM_PAINT:										// Here is where everything is painted to the screen
			hdc = BeginPaint(hwnd, &ps);					// Get a DC
			DisplayBitmap(hdc, &Bmp, x, y);					// Display the bitmap under the cursor
															// See if you can try and use some math to center the bitmap on the cursor.
			EndPaint(hwnd, &ps);							// Free the paint structure
			break;

		case WM_DESTROY:									// This message is sent when the user closes the window.
			
			PostQuitMessage(0);								// Post the QUIT message to the window (0 = WM_QUIT)															
			break;				
	}													

	return DefWindowProc (hwnd, iMsg, wParam, lParam);		// Process the default messages
															
}

bool LoadABitmap(BMP *Bmp, char *szFile)
{
	memset(Bmp,0, sizeof(BMP));								// This sets the structure to 0.  memset means "memory set()"
															// The sizeof() function gets the size in bytes of the structure BMP

															// The rest of this stuff is reading in the bitmap.  Each bitmap has
															// A header before the image in the file that holds the information about the type of bitmap.
															// I won't go into to much detail about it, because you can get the information off the internet.
															// Essentially, a bitmap is stored with a header first, then the bitmap information (size, pixels info, etc..)
															// Then the image bits.  The image bits are the actual pixels.

															// This opens the file and assigns a file pointer to our structure.  Just like fopen(), but this is better.
	Bmp->hFilePointer = CreateFile(szFile, GENERIC_READ, FILE_SHARE_READ, NULL, 
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN, NULL);
															// This checks if the file pointer is valid, if we didn't find the file, return FALSE , or 0.
	if(Bmp->hFilePointer == INVALID_HANDLE_VALUE) return(0);
															// This function returns a handle to the file.  We can choose to view ALL of the file, or parts of it.
															// Later, in animation we will utilize the option to only view parts of the image with this function.
	Bmp->hFileMapping = CreateFileMapping(Bmp->hFilePointer, NULL, PAGE_READONLY, 0, 0, NULL);
															// Here we get a handle to the beginning of the file, so we know where to start reading in the header information.
	Bmp->BeginOfFile  = (PUCHAR)MapViewOfFile(Bmp->hFileMapping, FILE_MAP_READ, 0, 0, 0);
															// Now we cast the handle to the beginning of the file as a bitmap header structure.  Now we have the file header stored.
	Bmp->bmFileHeader = (BITMAPFILEHEADER*)Bmp->BeginOfFile;
															// Now we add the size of the header from the beginning to move past that part in the file.
	Bmp->BeginOfFile  = sizeof(BITMAPFILEHEADER) + Bmp->BeginOfFile;
															
	Bmp->bmFileInfo   = (BITMAPINFO*)Bmp->BeginOfFile;		// Now we are at the part of the file that holds the actual bitmap information.
															// This stores the beginning of the image bits.  Some funky math on my part for more precision.
	Bmp->BeginOfImage = (PUCHAR) Bmp->BeginOfFile + Bmp->bmFileHeader->bfOffBits-sizeof(PUCHAR)*3-2;

	return(1);												// Return a success!!
}															// End of LoadABitmap()


void DisplayBitmap(HDC hdc, BMP *bitmap, int x, int y)		// This displays the bitmap!  Passed in our hdc, the bitmap structure, and the position to display it (x, y)
{
	HDC hdc2;
															// Create the DIB Section
	bitmap->hBitmap = CreateDIBSection(NULL, bitmap->bmFileInfo, DIB_RGB_COLORS,NULL, NULL,0);
															// Set the bits into the hdc
	SetDIBits(hdc, bitmap->hBitmap, 0, bitmap->bmFileInfo->bmiHeader.biHeight,
			  bitmap->BeginOfImage, bitmap->bmFileInfo, DIB_RGB_COLORS);
															// Get a patterm handle	
	hdc2 = CreateCompatibleDC(hdc);							// Create a pseudo backbuffer
	SelectObject(hdc2, bitmap->hBitmap);					// Put the bitmap into the backbuffer
															// Blit the backbuffer to the screen
	BitBlt(hdc, x, y, x + bitmap->bmFileInfo->bmiHeader.biWidth, 
		   y + bitmap->bmFileInfo->bmiHeader.biHeight,  hdc2, 0, 0, SRCCOPY);

	DeleteDC(hdc);											// Delete the first hdc (Frees the memory)
	DeleteDC(hdc2);											// Delete the second hdc (Frees the memory)
	DeleteObject(bitmap->hBitmap);							// Delete the handle to the bitmap (Frees the memory)
	
}
////////////////////////////////////////////////////////////
//
// *Quick Notes*
//
// Here is the first way we load in a bitmap.  We actually do it manually.
// In the next tutorial we will use a function called LoadImage().  This 
// tutorial shows you how to create a DIB section.
// 
// A Device dependent bitmap is dependent on your video card, but it's faster.
//
// A device independent bitmap (DIB) is slower, but can work on most any video card.  
// This is not dependent on a video card, but you can't really get to the image bits.
// 
// Eventually, we end up with a medium, and it's called a DIB section.  Now we
// can get to the bits AND it's device independent, along with being fast.
// 
// Don't worry about the loading in of the file right now.  Just know that is the
// way we manually load in a bitmap.
// 
// You will notice that the picture flickers when you drag it across the screen.
// To solve this problem, we'd have to use a techinque called double buffering.
// 
// © 2000-2005 GameTutorials
