// Done by TheTutor

/*
	Background Scrolling --
		
	This program is going to do two things:

		1) We are going to "tile" our background image so that it fills the window
		2) We are going to scroll our background -- It will go from top to bottom

	Now with not to much more work, you could put a spacship on the screen that is
	controlled by the mouse -- Add asteroids to avoid and you got yourself an Atari game!

	But I'll leave that as a challenge for you %)

	Within we're just going to cover the concept of scrolling a background image

*/

#include "winobj.h"
#include <time.h>

#define BKGRD_WID 300 // Width of the background image in pixels
#define BKGRD_HGT 150 // Height of the background image in pixels

// Width and height of our window
#define WIN_WID 640
#define WIN_HGT 480

#define FRAME_RATE 30 // 30 sounds good to me %)

// Window Globals ***

HBITMAP gBackground; // Handle to the background image

int gYScroll = 0; // This is how much of the bitmap is currently "scrolled"
			    // When we start out, none (zero) of the background has been scrolled
					
// *** End of Globals

// Returns true when drawing should occur -- false otherwise
bool LockFrameRate(int desired_frame_rate);

// This is the function that will scroll our background
void ScrollBackground(CWinObj &win_obj);

// WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprev, PSTR cmdline, int ishow)
{
	HWND hwnd;
    MSG msg;
    WNDCLASSEX wndclassex = {0};
	char class_name[] = "GameTutorials";

	CWinObj win_obj; // Main window object
										
	// Init the fields we care about			
    wndclassex.cbSize = sizeof(WNDCLASSEX);
    wndclassex.style = CS_HREDRAW | CS_VREDRAW;
    wndclassex.lpfnWndProc = WinProc;
    wndclassex.hInstance = hinstance;
	wndclassex.lpszClassName = class_name;
	wndclassex.hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(IDC_ARROW), IMAGE_CURSOR,
											0, 0, LR_SHARED);

    RegisterClassEx(&wndclassex);

	// Create the window
    hwnd = CreateWindowEx(WS_EX_APPWINDOW,
						  class_name,
						  "www.GameTutorials.com -- Background scrolling",
						  WS_OVERLAPPED | WS_SYSMENU, // Window won't be resizable
						  CW_USEDEFAULT, 
						  CW_USEDEFAULT, 
						  WIN_WID,
						  WIN_HGT,
						  NULL,
						  NULL,
						  hinstance,
						  NULL);

		// Error Check
		if(!hwnd)
			return EXIT_FAILURE;
		
	// Init our CWinObj
	if(!win_obj.init(hwnd))
		return EXIT_FAILURE;
	
	// Load our background image -- Notice when loading from a file you DO NOT have to specify
	// a width and height of your image
	gBackground = (HBITMAP)LoadImage(hinstance,"background.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE);

		// Error Check
		if(gBackground == NULL)
			return EXIT_FAILURE;
		
	// Show and update window
    ShowWindow(hwnd, ishow);
    UpdateWindow(hwnd);
	
	while(1)
	{
		// Checking for window messages -- If we get one we'll deal with it
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
				break;
				
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else if(LockFrameRate(FRAME_RATE))	// Otherwise we'll redraw the screen if we have to
		{	
			ScrollBackground(win_obj);

			// Blit our back_dc (our buffer we do all drawing to) to our win_dc
			// (basically the screen)
			BitBlt(win_obj.getHDC(),0,0,win_obj.getClientWid(),win_obj.getClientHgt(),
				   win_obj.getBackHDC(),0,0,SRCCOPY);
		}

	} // end of while(1)

	UnregisterClass(class_name,hinstance); // Unregister the WNDCLASSEX
		return msg.wParam;
}

// WinProc (nice and simple eh?)
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch(message)
    {
		case WM_DESTROY:			
	
			PostQuitMessage(0);
				return 0;
    
	} // end of switch(message)

    return(DefWindowProc(hwnd, message, wparam, lparam));
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

// Scrolls the background
void ScrollBackground(CWinObj &win_obj)
{
	int x_pos = 0;
	int y_pos = gYScroll;

	HDC temp_dc = CreateCompatibleDC(win_obj.getHDC());

		// Error Check
		if(temp_dc == NULL)
			return;

	// Select the background image into our temp_dc
	HBITMAP save_bmp = (HBITMAP)SelectObject(temp_dc,gBackground);

	// We are going to draw the background tile going left to right, from top to bottom
	// Once our y_pos (for the upper left corner) is greater than the height of the window
	// we're done drawing for this frame
	while(y_pos < win_obj.getClientHgt())
	{
		// First thing we do is draw going left to right -- 
		// We ALWAYS start at the left side (that means x_pos == 0)
		// Then we add the WIDTH of the background image to the x_pos so we can
		// next time we blit, we'll draw right next to the last one (effectively tiling the
		// image so it will look like continous image)
		//
		// It's a little wordy so a quick look at the "illustration" at the bottom
		// of this tutorial should help :)
		while(x_pos < win_obj.getClientWid())
		{
			// Blit the background tile at the current location
			BitBlt(win_obj.getBackHDC(),x_pos,y_pos,BKGRD_WID,BKGRD_HGT,
				   temp_dc,0,0,SRCCOPY);

			x_pos += BKGRD_WID; // Move the X-pos of the upper left corner (of where to blit to)
								// to the right

		}

		x_pos = 0; // Zero out x_pos (we'll go from left to right again)

		// Move down to the next "row" for blitting the background tile
		y_pos += BKGRD_HGT;		
	}

	// Lastly -- We need to fill the top row with whatever percentage of the 
	// bitmap has been slid down
	
		x_pos = 0; // Zero out x_pos

		while(x_pos < win_obj.getClientWid())
		{
			// Blit the background tile at the current location
			BitBlt(win_obj.getBackHDC(),x_pos,0,BKGRD_WID,gYScroll,
				   temp_dc,0,(BKGRD_HGT - gYScroll),SRCCOPY);

			// Does the equation in the BitBlt() make sense???  Well I'll explain it --
			// The first time we draw we'll start drawing at the top of the window (line 0)
			// The next time thru we start drawing at (line 1) of the window meaning that
			// first line of pixels needs to be redrawn -- Not only does it need to be 
			// redrawn but in needs to be redrawn with the BOTTOM line of pixels of our
			// background image so that the tiling stays looking correct.  So to reinforce
			// this concept, here's the parameters of BitBlt() broken down:

			/*

			win_obj.back_dc -- Same as always
			x_pos -- Same as always (still are going left to right)
			0  -- This will always be zero cause we know we are filling from the top of the screen
			      Remember we are scrolling down, so the "hole" will always be at the top
			BKGRD_WID -- Same as always
			gYScroll -- We only need to blit the height of the bitmap we've scrolled down
			win_obj.bmp_dc -- Same as always
			0 -- Same as always (starting at left most position in bitmap)
			(BKGRD_HGT - gYScroll) -- Okay this may look tricky but it's not, if we moved down
									 one line of pixels (meaning gYScroll would equal 1) then the
									 line of pixels we want to draw at the top of window is the
									 LAST line of pixels in our bitmap (this way the seamless
									 textures still appear seamless) -- Since our BKGRD_HGT
									 is 150 the last row would be 149 which is (150 - 1) which
									 is what BKGRD_HGT - gYScroll is equivalent to :)
			SRCCOPY -- Same as always (When we blit, do a source copy)
			
			*/

			x_pos += BKGRD_WID; // Move the X-pos of the upper left corner (of where to blit)
								// to the right

		}


	// Here every time we blit everything we want to increase the amount we've "scrolled"
	// When we've scrolled the entire height of the bitmap, we reset to zero
	if(++gYScroll == BKGRD_HGT)
		gYScroll = 0; // Set gYScroll to zero

	// Free up the memory
	SelectObject(temp_dc,save_bmp);
	DeleteDC(temp_dc);

} // end of ScrollBackground(CWinObj &win_obj)


// Okay the tiling this is what happens lets suppose we have a window which is
// 10x5 and a bitmap that's 4x2

/*
	  Window		Bitmap

	0000000000		 $$$$
	0000000000		 ****
	0000000000
	0000000000
	0000000000

	----------------- Begin Blitting -------------------------------------------------
	----------------- Remember this is only ONE call to ScrollBackground()

	gYScroll = 0

	First Blit		Second Blit		Third Blit	// It's okay that we went over
												// Windows takes care of that for
	$$$$000000		$$$$$$$$00		$$$$$$$$$$  // us -- But since we're did go 
	****000000		********00		**********  // over we start back at the left
	0000000000		0000000000		0000000000
	0000000000		0000000000		0000000000
	0000000000		0000000000		0000000000

	Fourth Blit		Fifth Blit		Sixth Blit
											
	$$$$$$$$$$		$$$$$$$$$$		$$$$$$$$$$
	**********		**********		**********
	$$$$000000		$$$$$$$$00		$$$$$$$$$$
	****000000		********00		**********
	0000000000		0000000000		0000000000

	Seventh Blit	Eighth Blit		Nineth Blit
										
	$$$$$$$$$$		$$$$$$$$$$		$$$$$$$$$$
	**********		**********		**********
	$$$$$$$$$$		$$$$$$$$$$		$$$$$$$$$$
	**********		**********		**********
	$$$$000000		$$$$$$$$00		$$$$$$$$$$
	
	// Here it doesn't matter that we went over
	// the bottom -- Again, Windows takes care of that
	// for us, but since we did we increase our yScoll to 1
	// and we would return from the call to ScrollBackground()

	----------------- Begin Blitting -------------------------------------------------
	----------------- Remember this is only ONE call to ScrollBackground()

	// This would be the NEXT call to ScrollBackground(), the next frame

	gYScroll = 1

	First Blit		Second Blit		Third Blit	// Notice how the top line of
												// the window IS NOT initally drawn
	0000000000		0000000000		0000000000  // Because gYScroll == 1, we start 
	$$$$000000		$$$$$$$$00		$$$$$$$$$$  // drawing at the 1st line in the window
	****000000		********00		**********  // 
	0000000000		0000000000		0000000000
	0000000000		0000000000		0000000000

	Fourth Blit		Fifth Blit		Sixth Blit
											
	0000000000		0000000000		0000000000
	$$$$$$$$$$		$$$$$$$$$$		$$$$$$$$$$
	**********		**********		**********
	$$$$000000		$$$$$$$$$$		$$$$$$$$$$
	****000000		**********		**********

	Seventh Blit	Eighth Blit		Nineth Blit	// Now here we go back and fill the
												// the hole at the top -- The equation
	****000000		********00		**********	// use is Bitmap Height - gYScroll
	$$$$$$$$$$		$$$$$$$$$$		$$$$$$$$$$  // for how many lines should be drawn
	**********		**********		**********	// Well 2 (Bitmap Height) - 1 (gYScroll)
	$$$$$$$$$$		$$$$$$$$$$		$$$$$$$$$$	// equals 1 which is EXACTLY how many
	**********		**********		**********	// rows of pixels needs to be drawn
												// AND since we always begin blitting from
												// the bottom of our background image and work 
												// our way up in our second while loop, the
												// line of pixels we want will always match up :)
	
	// If you still have quesitons be sure to post 'em at at www.GameTutorials.com
	// We're open 24 hours a day 365 days a year %)
*/
	

/*----------------------------*\
|  TheTutor                    |
|  thetutor@gametutorials.com  |
|  © 2000-2005 GameTutorials   |
\*----------------------------*/
