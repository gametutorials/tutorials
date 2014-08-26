// Done by TheTutor

/*

  This tutorial is all about double buffering.  So what's that you ask?  Well to really 
  explain double buffering it's important that first you know what "normal" single buffering
  is all about.  So a quick lesson in what's going on when you make a window.

	When you call CreateWindowEx() what you get is a HWND (a handle to a window).
	You can think of this HWND as a pointer (cause it is) that points to ALL the information
	one could ever want about the window.

	Using the HWND you can get an HDC (handle to a device context) for the window.  You can
	think of HDC as a pointer (cause it is) that points to ALL the GRAPHICAL information
	about a window.  When you create a window you get a block of memory that you can think
	of as a "bitmap" (HBITMAP) where ALL the drawing/blitting/graphical stuff will take 
	place.  The HDC acts as a way for you to modify the bitmap in memory that is created 
	when we make a window.  This bitmap is referred to as the "single buffer" or "front buffer"
	and it is literally all the pixels that you see on your monitor.  Every time you
	draw to the front buffer, the window (and hence the monitor) will instantly be updated.

  So.... this brings us to double buffering.  Double buffering is the technique in where 
  you copy the front buffer of your window so now you have TWO buffers, (the new buffer is
  commonly referred to as a "back buffer") and do all your drawing to the copied buffer (the back
  buffer) FIRST.  Once all drawing is complete, you draw the finished back buffer to the
  front buffer.

  What does this do for us?  Well it avoids tearing and other graphical glitches.  When we 
  draw to the front buffer (or really any buffer for that matter) eventually down in the
  bowls of the CPU/GPU what happens is a pixel by pixel copy of our "source image" to our 
  "destination image" (our monitor).  Now if your only doing a small amount of rendering, then
  drawing directly to the front buffer is no problem and your eye is too slow to visually
  catch the changes.  But when you're dealing with games (of even a small magnitude) there
  is a butt-load of drawing that takes place.  If you try to just pump everything to the front
  buffer, your eye is fast enough to pick up the "artifacts" (graphical glitches) that will
  occur from doing so.  Thus we draw everything first to our back buffer, then draw our 
  completed, full rendered scene to the front buffer so no visual anomalies occur.

  Hopefully that enlightens you on what double buffering is all about.  But do not fret if
  you are still confused.  A few lines down you'll see a line of code that looks like this:

	#define DOUBLE_BUFFER 1

  If you change the '1' to a '0' double buffering will be turned off and you can see as 
  plain as day why you need it :)


  Now before we get started, lets talk about what we are going to do.  It's pretty 
  straight forward.  First we'll make our window.  Second, we'll create our double buffer.
  Third we're gonna load a bitmap.  Then we'll allow users to move the bitmap around with
  the mouse.  Each time it's moved we'll draw it to our back buffer and then update our
  front buffer.  You should see a smooth, seamless bitmap being moved around inside of the 
  window.

  And with that, it's time to get coding

*/

#include <windows.h>

#define WIN_WID 640
#define WIN_HGT 480

#define IMG_WID 128 // Width of our image
#define IMG_HGT 128 // Height of our image

#define class_name "GameTutorials_DoubleBuffer"

#define DOUBLE_BUFFER 1 // Change this to zero to view the app without double buffering

// This is the struct that will hold all of our double buffering info
struct SDBuffer
{
	HWND win_hwnd; // A copy of the window handle

	HDC win_dc; // This is the window's HDC
	HDC back_dc; // This is the HDC to the back buffer we're gonna make

	RECT rect; // This will hold the client RECT of our window

	HBITMAP back_bmp; // This is the HBITMAP that we'll fill with an exact copy of 
					 // our window's HBITMAP (pixels)

	HBITMAP old_bmp; // We need this guy around so we can totally free up memory when it's
				    // all said and done
};

// Globals ***

int gXPos = 0; // This will hold the upper left hand x-coord of the mouse
int gYPos = 0; // This will hold the upper left hand y-ccord of the mouse

// *** End of Globals

// This will set up our double buffering.   We return true on success, false on failure
bool InitDoubleBuffer(SDBuffer &doubleBuff);

// This funciton will free up all the memory associated with our double buffer
void FreeDoubleBuffer(SDBuffer &doubleBuff);

// Locks the frame rate at the passed in frame rate (60 FPS by default)
bool LockFrameRate(int frame_rate = 60);

// Standard callback function
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprev, PSTR cmdline, int ishow)
{  
	HWND hwnd;
    MSG msg;
    WNDCLASSEX wndclassex = {0};

	SDBuffer doubleBuff = {0}; // This is our "double buffer" struct

	// Fill the fields we care about
	wndclassex.cbSize = sizeof(WNDCLASSEX);
    wndclassex.style = CS_HREDRAW | CS_VREDRAW;
    wndclassex.lpfnWndProc = WinProc;
    wndclassex.hInstance = hinstance;
    wndclassex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclassex.lpszClassName = class_name;
    wndclassex.hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(IDC_ARROW), IMAGE_CURSOR,
											0, 0, LR_SHARED); // Load the default arrow cursor

    RegisterClassEx(&wndclassex); // Register the window

    hwnd = CreateWindowEx(NULL, // No extra window attributes
						  class_name,
						  "www.GameTutorials.com -- Double Buffering",
						  WS_OVERLAPPEDWINDOW,
						  CW_USEDEFAULT, // Window will receive a default x pos on screen
						  CW_USEDEFAULT, // Window will receive a default y pos on screen
						  WIN_WID,
						  WIN_HGT,
						  NULL,
						  NULL,
						  hinstance,
						  NULL);

		// Error check
		if(!hwnd)
			return EXIT_FAILURE; // Something really bad happened!
		
	doubleBuff.win_hwnd = hwnd; // Set the HWND of our double buffer struct

	// Attempt to initialize our double buffering
	if(!InitDoubleBuffer(doubleBuff))
		return EXIT_FAILURE; // Couldn't set up double buffering
	
	// Here we'll load up our bitmap
	HBITMAP img_bmp = (HBITMAP)LoadImage(hinstance,"AnImage.bmp",IMAGE_BITMAP,
										 0,0,LR_LOADFROMFILE);

		// Error Check
		if(!img_bmp)
			return EXIT_FAILURE; // Couldn't load our image
		
	// Create a compatible HDC so that we can draw our "img_bmp"
	HDC img_dc = CreateCompatibleDC(doubleBuff.win_dc);
		
		if(!img_dc)
			return EXIT_FAILURE;
		
	// Select our "img_bmp" into the "img_dc"
	HBITMAP old_bmp = (HBITMAP)SelectObject(img_dc,img_bmp);

    ShowWindow(hwnd, ishow);
    UpdateWindow(hwnd);
  
    while(1)
	{
		// Check message(s) if there are any
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
				break;
				
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else if(LockFrameRate())
		{
			#if DOUBLE_BUFFER // If we are using double buffering

				// First we fill our back buffer to solid white (the same color as the
				// background color of our window)
				FillRect(doubleBuff.back_dc,&doubleBuff.rect,(HBRUSH)GetStockObject(WHITE_BRUSH));

				// Next we'll draw the bitmap to our back buffer
				BitBlt(doubleBuff.back_dc,gXPos,gYPos,gXPos + IMG_WID,
					   gYPos + IMG_HGT,img_dc,0,0,SRCCOPY);

				// Then we draw the back buffer to the front buffer (our window)
				BitBlt(doubleBuff.win_dc,0,0,doubleBuff.rect.right,
					   doubleBuff.rect.bottom,doubleBuff.back_dc,0,0,SRCCOPY);

			#else // No double buffering in use

				// We fill our window with solid white so we can clear away the "old"
				// position of the image
				FillRect(doubleBuff.win_dc,&doubleBuff.rect,(HBRUSH)GetStockObject(WHITE_BRUSH));

				// Blit the image to the window
				BitBlt(doubleBuff.win_dc,gXPos,gYPos,gXPos + IMG_WID,
					   gYPos + IMG_HGT,img_dc,0,0,SRCCOPY);

				// **NOTE** Do not be mislead by the use of the "doubleBuff" variable.
				//			We are ONLY using this to access the window's HDC.  Absolutely
				//			no double buffering goes on in the between the #else and the 
				//			#endif.  Be sure to look at how worse it looks without double
				//			buffering.

			#endif
		}

	}

	// Free up our image memory
	SelectObject(img_dc,old_bmp);
	DeleteDC(img_dc);

	// Free up all the memory associated with our back buffer
	FreeDoubleBuffer(doubleBuff);

	UnregisterClass(class_name,hinstance); // Free up WNDCLASSEX
		return msg.wParam;
}

// The WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	PAINTSTRUCT ps;
 
	// Depending on the message -- we'll do different stuff
    switch(message)
    {       		
		case WM_PAINT:
            
			BeginPaint(hwnd,&ps);
			EndPaint(hwnd,&ps);
				return 0;

		// Message we get when the mouse moves
		case WM_MOUSEMOVE:

			gXPos = LOWORD(lparam);  // X position of cursor 
			gYPos = HIWORD(lparam);  // Y position of cursor
				return 0;

        case WM_DESTROY:
            
			PostQuitMessage(0);
             return 0;

    } // end of switch(message)

    return DefWindowProc(hwnd, message, wparam, lparam);
}

// Here will create/initialize our double buffer
// Return true on success, false on failure
bool InitDoubleBuffer(SDBuffer &doubleBuff)
{
		// Error Check
		if(!doubleBuff.win_hwnd)
			return false; // Not a valid window HWND

	// First and foremost we are going to get and store off the CLIENT RECT of our
	// window.  The client area (client rect) is the portion of a window where the
	// application displays output, such as text, graphics, etc, etc
	GetClientRect(doubleBuff.win_hwnd,&doubleBuff.rect); // Get the client RECT

	// The next thing we are going to do is store off the window's 
	// HDC.  Remember you can think of the HDC as a way to "talk to" all of the 
	// pixels that make up the window.
	doubleBuff.win_dc = GetDC(doubleBuff.win_hwnd); // Get the window's HDC

		// Error Check
		if(doubleBuff.win_dc == NULL)
			return false;

	// Okay we have the window's HDC.  What we want to do is create a replicable 
	// in memory of the window's pixels that we are able to draw to.  So that
	// means we'll need a HDC that can interface (is compatible to) the window's
	// HDC.  Thus we create a compatible HDC
	doubleBuff.back_dc = CreateCompatibleDC(doubleBuff.win_dc); 

		// Error Check
		if(doubleBuff.back_dc == NULL)
			return false;

	// So now we have a means to talk to our back buffer, but we DO NOT have 
	// the buffer (the set aside memory that will hold the pixel information).
	// No worries, we'll just create a compatible bitmap the SAME size as 
	// our window's client area
	doubleBuff.back_bmp = CreateCompatibleBitmap(doubleBuff.win_dc,
												 doubleBuff.rect.right,
												 doubleBuff.rect.bottom);

		// Error Check
		if(doubleBuff.back_bmp == NULL)
			return false;

	// If we get here that means we were successful in creating a "buffer" that 
	// mimics the windows front buffer.  But before we can draw to our newly 
	// created buffer, we have to tell the "back buffer's HDC" that it is 
	// associated with (or in charge of) the "back buffer's HBITMAP -- Thus we
	// select the created back buffer into our back buffer's device context
	doubleBuff.old_bmp = (HBITMAP)SelectObject(doubleBuff.back_dc,doubleBuff.back_bmp); 

	// **NOTE** remember we save off the return from SelectObject() so we can FULLY 
	// free up memory before we close the application

	return true; // We be buff
}

// This function will free up all the memory we allocated
void FreeDoubleBuffer(SDBuffer &doubleBuff)
{
		// Error Check
		if(!doubleBuff.win_hwnd)
			return;

	if(doubleBuff.old_bmp) // If we have a double buffer
	{
		// Select back the original "bitmap"
		SelectObject(doubleBuff.back_dc,doubleBuff.old_bmp);

		// Free up memory
		DeleteObject(doubleBuff.back_bmp);
		DeleteDC(doubleBuff.back_dc);
	}

	if(doubleBuff.win_dc) // If we have the window's device context
	{
		// Relese the device context
		ReleaseDC(doubleBuff.win_hwnd,doubleBuff.win_dc);
	}

	// Set everything to zero
	memset(&doubleBuff,0,sizeof(SDBuffer));
}

// Locks the frame rate at "frame_rate"
// Returns true when it's okay to draw, false otherwise
bool LockFrameRate(int frame_rate)
{
	static float lastTime = 0.0f;
	
	// Get current time in seconds (milliseconds * .001 = seconds)
	float currentTime = GetTickCount() * 0.001f; 

	// Get the elapsed time by subtracting the current time from the last time
	// If the desired frame rate amount of seconds has passed -- return true (ie Blit())
	if((currentTime - lastTime) > (1.0f / frame_rate))
	{
		// Reset the last time
		lastTime = currentTime;	
			return true;
	}

	return false;
}

// Double buffer your pleasure...

/*
	Hopefully you can see that without a doubt, something double buffered looks better 
	than something singled buffer.  However, double buffering IS NOT the end all be all
	solution to all the possible graphic mishaps out there.  It's just one tool in the tool
	belt of graphic programmers that should be used to make games/applications look prettier.

	Also note that if you move the mouse wildly around you may ask yourself, "Why does 
	this look choppy, I thought we were double buffering?", Well unfourtantely the answer to
	that question is, "It should look choppy %)" -- Since we are not doing any syncing of
	the mouse movement and the drawing to the screen it's possible to produce some 
	"choppy bitmap movement".  But that is a whole other topic...

	As always, questions, comments or quandaries should be posted directly to
	www.GameTutorials.com
*/

/*----------------------------*\
|  TheTutor                    |
|  thetutor@gametutorials.com  |
|  © 2000-2005 GameTutorials   |
\*----------------------------*/
