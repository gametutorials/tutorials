// Done by TheTutor

/*	
	This tutorial is all about what a device context is. In a Win32 program, these are usually
	declared as HDC (handle to a device context).  So for starters, what in the heck 
	is a device context?  Well MSDN defines it as, "Structure that defines a set of graphic
	objects and their attributes, and the graphic modes that affect output. In addition, the
	device context (DC) refers to a physical output device, its name, device driver, and other
	attributes."
	
	That's a pretty good definition.  An HDC (if you see HDC just think of device context) is
	essentially a pointer to a struct that contains all the necessary information of the
	device context.  This layer of abstraction is really handy.  This way, we can draw something
	to the screen without knowing anything about the video card or monitor, because Windows
	takes care of all that stuff for us.  So to summarize, a device context is a means for 
	us to communicate between hardware devices, without knowing anything about the underlying
	hardware devices.

	There are four types of device contexts, they are:

	Display -- These support drawing operations on a video display terminal
	Printer -- These support writing/drawing operations on a printer
	Memory -- These support drawing operations on a bitmap
	Information -- These support retrieval of device data

	In general, the main two we are going to concern ourselves with are display and memory
	device contexts.  In this tutorial, we are only going to talk about the display HDC.

	So now that you have a rudimentary understanding of what an HDC is, lets talk about what
	this very simple program does.  All we are going to do is create a window, gets it's HDC
	and using the HDC, fill the window to black.  Pretty simple right?!?

*/

#include <windows.h>

////////////////
// Constants //
//////////////

const int kWinWid = 320; // Width of window
const int kWinHgt = 240; // Height of window
const char kClassName[] = "GameTutorials_HDC"; // WNDCLASSEX's name 

// Standard callback function
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprev, PSTR cmdline, int ishow)
{
	HWND hwnd; // This will hold the handle to our window
    MSG msg; // This will hold any messages (such as a mouse click) that
			 // our window might receive

    WNDCLASSEX wndclassex = {0}; // This is our "window class" -- The "EX" stands
								// for "extended style" which gives us more options
							   // when creating our window (although we're going to
							  // completely ignore 'em)

	// Fill the fields we care about
	wndclassex.cbSize = sizeof(WNDCLASSEX); // Always must be set
    wndclassex.style = CS_HREDRAW | CS_VREDRAW; // Window classes style
    wndclassex.lpfnWndProc = WinProc; // Pointer to where the WinProc() is defined
    wndclassex.hInstance = hinstance; // The handle to the instance of our window
	wndclassex.lpszClassName = kClassName; // The name of our window class
	wndclassex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); // This sets
																   // the background color
																  // of the window to white
	wndclassex.hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(IDC_ARROW), IMAGE_CURSOR,
											0, 0, LR_SHARED); // This sets the cursor to
															 // be the default arrow cursor    

    RegisterClassEx(&wndclassex); // Register the window class so calls to CreateWindowEx()
								 // will know what window class to use when creating the window
	
    hwnd = CreateWindowEx(NULL, // Extra window attributes, NULL means none
						  kClassName, // Name we gave our WNDCLASSEX			
						  "www.GameTutorials.com -- HDC's", // Text for the title bar
						  WS_OVERLAPPEDWINDOW, // Style of window (see MSDN for full description)
						  CW_USEDEFAULT, // Upper left xPos of window (Windows picks default)
						  CW_USEDEFAULT, // Upper left yPos of window (Windows picks default)
						  kWinWid, // Width of window in pixels
						  kWinHgt, // Height of window in pixels
						  NULL, // Handle to parent window (we have none, this is the parent)
						  NULL, // Handle to a menu (we have none)
						  hinstance, // Handle to instance of this window (passed in by WinMain())
						  NULL); // Extra info to pass to the WinProc (we have none)

		// Error check
		if(!hwnd)
			return EXIT_FAILURE; // Something really bad happened!

	// Okay here is where we are getting the windows HDC.  Now what this gets us
	// is a handle to the DISPLAY device context.  So once we have this we can use it
	// to draw to our window.  For any type of graphical stuff the computer needs to know 
	// where to draw.  The HDC of the window tells the computer exactly that :)
	HDC hdc = GetDC(hwnd);

		// Now afterwards we are going to check to make sure the call worked
		// if "hdc" equals NULL, then the call has failed and we need to bail out
		// of the program
		if(!hdc)
			return EXIT_FAILURE; // Couldn't get window's hdc -- This be bad :(

    ShowWindow(hwnd, ishow); // Show the window (make it visible)
    UpdateWindow(hwnd); // Updates the window (paint the window)

	// Okay now we will make the window black.  There's actually a decent amount 
	// of stuff going on to do that, so lets break it down piece by piece.

	// First we need to get a RECT that describes the CLIENT (that is the inside of the 
	// window NOT counting the edges and title bar) area of the window.
	RECT rect;

	GetClientRect(hwnd,&rect); // Luckily, this handy-dandy function will do just that
							  // Pass in the handle of the window you want the RECT for
							 // and a pointer to a RECT and it will fill it with the
							// window's CLIENT dimensions

	// Now that we got the RECT we call fill that area to black -- Here's how this function 
	// breaks down:
	// hdc -- The DEVICE CONTEXT we want to draw to (our window)
	// &rect -- Address of the area in the hdc we want to draw to (the client area of the window)
	// (HBRUSH)GetStockObject(BLACK_BRUSH) -- Okay this functions technically returns an HGDIOBJ
	//										  which is a handle to a GDI (graphic display interface).
	//										  These can represent any drawing object, such as a brush
	//										  pen, font, etc to be used by Windows.  For now, just know
	//										  this is how you can specify a color to fill things in with.
	//										  If you feel so inclined, try changing BLACK_BRUSH to 
	//										  DKGRAY_BRUSH.  See if you can guess the results %)
	FillRect(hdc,&rect,(HBRUSH)GetStockObject(BLACK_BRUSH));
	
	// We only draw once so we are already done with the HDC!
	
	// It is very important that you ALWAYS release the memory associated with
	// an HDC.  Supposedly Win2K and up will do this for you, but even so 
	// that would only happen at the end of the program, if you were doing this
	// in a loop you'd chew up memory pretty fast.  To release the memory from a call to
	// GetDC() just do the following.
	// hwnd -- Handle to the window the HDC is associated with
	// hdc -- Handle to the device context for freeing
	ReleaseDC(hwnd,hdc);
   
    while(1)
	{
		// Get message(s) (for instance a key press) if there is any
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
				break;
				
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// Do all the hardcore computational stuff here :)
		}
	}

	// It is also important that you remove the memory associated with a "window class"
	// when you are done with it.  When we call RegisterClassEx() some memory gets set aside
	// for the "class description" (our WNDCLASSEX) of this window we are going to create.
	// If we don't free this memory, it will stay around.  So we call this function below to
	// free up the memory we allocated when calling RegisterClassEx().  Most of the time you'll
	// want to call this at the very end of your Win32 program.
	// 
	//	**NOTE** You ONLY need to call UnregisterClass() if you register the window class with
	//	a call to RegisterClassEx(), the older register function, ReisterClass(), (notice no "Ex")
	//	does not require this call
	UnregisterClass(kClassName,hinstance);
		return msg.wParam; // And we're out
}

// The WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
   	// Depending on the message -- we'll do different stuff
    switch(message)
    {
		// As you can see we've kept the WinProc() VERY SIMPLE :)

        case WM_DESTROY:
            
			PostQuitMessage(0);
				return 0;

    } // end of switch(message)

    return DefWindowProc(hwnd, message, wparam, lparam);
}

/*  It's absolutely imperative that you grasp what a HDC is.  You will see these again and again
	and	again and again in Win32 programming.  If you're still a little unsure, think of an
	HDC like this:

	An HDC is a "gateway" to the pixels that make up your window.  With a display device context
	(that is what GetDC() returns you) you can draw to the window.  Win32 provides many
	ways for drawing to the window, FillRect() is just one of many.  In other Win32 tutorials
	you will see the HDC being used repeatedly for drawing to the window.  So an HDC is	a means to
	draw to a window.
	
	Lastly, if you resize the window in any way, the black color we filled it to will go away.
	This is because resizing automatically causes the window to be redrawn.  Since we only fill
	the color once, the black color gets erased.

	If you're still unsure about device contexts or anything else in Windows, be sure to post your 
	quandaries to www.GameTutorials.com
*/

/*----------------------------*\
|  TheTutor                    |
|  thetutor@gametutorials.com  |
|  © 2000-2005 GameTutorials   |
\*----------------------------*/
