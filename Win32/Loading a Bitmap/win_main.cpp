// Done by TheTutor

/*

  Okay all we're going to do is load a bitmap to the screen.  That is it.  
  Now it is important to know the method we're using to load the bitmap to the screen is
  just ONE way we could go about doing it.

  You should use this tutorial to play with loading bitmaps so you get familiar with loading 'em
  Try changing some values -- See what crashes, see what doesn't

  I don't know about you, but it looks like it's a good time to start getting loaded :)

*/

#include <windows.h>

// WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprev, PSTR cmdline, int ishow)
{
  	HWND hwnd;
    MSG msg;
    WNDCLASSEX wndclassex = {0};	// Zero out WNDCLASSEX
	
	char class_name[] = "GameTutorials_LoadingBMPs";

	// Fill in fields we care about -- If your unsure of what all these "fields" are
	// MSDN is an excellent reference to guide to everything Win32
	wndclassex.cbSize = sizeof(WNDCLASSEX);
    wndclassex.style = CS_HREDRAW | CS_VREDRAW;
    wndclassex.lpfnWndProc = WinProc;
    wndclassex.hInstance = hinstance;
    wndclassex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wndclassex.lpszClassName = class_name;
    wndclassex.hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(IDC_ARROW), IMAGE_CURSOR,
											0, 0, LR_SHARED); // Load the default arrow cursor
											
    RegisterClassEx(&wndclassex);

	// Creating our window -- 
    hwnd = CreateWindowEx(NULL, // The extended Window style (we'll have no extended style)
						  class_name, // The name of our registered window class 
						  "www.GameTutorials.com -- Loading a .bmp", // Window title
						  WS_OVERLAPPEDWINDOW, // Attributes of our window
						  CW_USEDEFAULT, // Upper left X-coordinate of our window
										// (OS assigns default value)
						  CW_USEDEFAULT, // Upper left Y-coordinate of our window
										// (OS assigns default value)
						  450, // Width of our window 
						  325, // Height of our window
						  NULL, // Handle to parent window (No parent window)
						  NULL,	// Handle to menu (No menu)
						  hinstance, // The instance of this application, an argument of WinMain()
						  NULL); // Pointer for "extra info" (we have none)

		// Error Check
		if(!hwnd)
			return EXIT_FAILURE;
		
	// Show and update window
    ShowWindow(hwnd, ishow);
    UpdateWindow(hwnd);


/*************** Loading a bitmap AND displaying the bitmap *********************************/
	
	// Okay here's the explanation for each argument:
	
	// hinstance is the HINSTANCE given to us by WinMain(),
	
	// "MyBitmap.bmp" is the name of the bitmap we want to load, 
	
	// IMAGE_BITMAP is a flag to say hey we're loading a .bmp type file
	
	// The two zeros correspond to the width and height of the image we're loading.  We can make these
	// zero because since we're loading from a file it will take the width and height that 
	// the file says the image we're loading is.  Can you say wordy?
	
	// All right the last parameter says that we're loading this image from a file 

	HBITMAP hbitmap = (HBITMAP)LoadImage(hinstance,"MyBitmap.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
	
	HDC hdc = GetDC(hwnd); // This gets the device context associated with the window we made
	HDC image_dc = CreateCompatibleDC(hdc);	// The gets a device context that is of the same size and
											// type of device context we pass in.  If your unsure of
											// what a device context is, at the bottom of this
											// file I give a formal definition

	// Must save off old object that was "selected"
	HBITMAP old_hbitmap = (HBITMAP)SelectObject(image_dc,hbitmap);

	
	// This is the sweet function that draws stuff for us 
	// hdc is where we want to draw stuff to (device context of our window)
	// 50, 50 is the upper left corner of where we are going to draw
	// 320 is the width of bitmap in pixels
	// 192 is the height of our bitmap in pixels
	// image_dc is the stuff we want to draw to our window
	// 0,0 is the upper left corner of where we want to start coping the image_dc from
	// SRCCOPY stands for source copy -- Meaning take everything at the source (image_dc) and
	// copy it to the destination (hdc)
	BitBlt(hdc,50,50,320,192,image_dc,0,0,SRCCOPY);

/*************** Done loading a bitmap AND displaying the bitmap ****************************/

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
			/* YOU WOULD DO COMPUTATIONALLY EXPENSIVE THINGS HERE
			   (updating player movement, checking for collision, etc) */

		}

	} // end of while(1)

	// FREE UP MEMORY
	SelectObject(image_dc,old_hbitmap);	// Every time you select an object into a device context
										// you must select back in the "old object" that was there
										// to completely free up your GDI resources

	DeleteObject(hbitmap); // Delete the HBITMAP we created -- Notice we didn't create
						   // "old_hbitmap" so we don't have to free it

	ReleaseDC(hwnd, hdc); // When you call GetDC() ALWAYS call ReleaseDC() on the HDC you get
						  // back to free up memory

	DeleteDC(image_dc); // When you call CreateCompatibleDC() ALWAYS call DeleteDC() on the 
						// HDC you get back when you no longer need to use it.  This again
						// frees up memory and since we are good programmers we NEVER want
						// to not free up all the memory we allocate :]

	// Frees the memory associated with the WNDCLASSEX we registered
	UnregisterClass(class_name,hinstance);
		return (int)msg.wParam;
}


// Does this WinProc look simple enough???  We're not doing anything special so we
// don't have to check for to many messages
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    PAINTSTRUCT ps;
			
	switch(message)
    {		
		case WM_PAINT:
			BeginPaint(hwnd, &ps);
			EndPaint(hwnd, &ps);
				return 0;

		// This message gets sent when the window is terminated (closed, exited, you get
		// the point)
		case WM_DESTROY:
	
			PostQuitMessage(0); // Send WM_QUIT message thus we'll break the while loop in
				return 0;	   // WinMain()
    
	} // end of switch(message)

    return(DefWindowProc(hwnd, message, wparam, lparam));
}


/* Bitmap Patch 1.001

	A Device Context is --

	A structure that defines a set of graphic objects and their attributes, and the graphic
	modes that affect output.  In addition, the device context (DC) refers to a physical output
	device -- its name, device driver, and other attributes.  GDI function call parameters
	contain a handle to a DC to include the attributes of the specified device.  There are four
	types of DC: display (supports drawing operations on a video display terminal), 
	printer (supports drawing operations on a printer or plotter), 
	memory (supports drawing operations on a bitmap), 
	and information (supports retrieval of device data.)

		Well that's what MSDN says -- If that is way to much techno-babble just think of a 
		device context as a place in the computer where stuff gets stored to make drawing
		to the screen possible :)


	Also, you'll notice if you resize the screen our image goes away!  This is because we are
	only painting the image to the screen ONCE --

	By making "hdc" and "image_dc" global AND copying the function BitBlt() between
	the BeginPaint() and EndPaint() in WM_PAINT the picture will always show (but it will
	flicker	when you redraw it)

	Finally, notice we don't check to see if MyBitmap.bmp actually gets loaded --
	When you do this yourself it would be a VERY GOOD IDEA to see that indeed the bitmap got
	loaded if it didn't, you can decide what to do then (I usually spit out an error message
	and terminate the program)
*/

/*----------------------------*\
|  TheTutor                    |
|  thetutor@gametutorials.com  |
|  © 2000-2005 GameTutorials   |
\*----------------------------*/
