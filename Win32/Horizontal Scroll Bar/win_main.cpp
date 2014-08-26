// Done by TheTutor

/*
	Sometimes everything just doesn't fit in one window.  Hence the need to scroll.  This tutorial
	shows how to have and use a horizontal scroll bar with a window.
	
	No need for a large introduction, lets get to the code!
*/

#include <windows.h>

////////////////
// Constants //
//////////////

// Width and height of the window
const int kWinWid = 400;
const int kWinHgt = 200;

// The starting (x,y) of where to draw our sentence
const int kStartX = 2;
const int kStartY = 25;

const char kClassName[] = "GT_HorizontalScrollBars"; // Our WNDCLASSEX's name

// This is the sentence that will be displayed to the window.  As you can
// see it's pretty long to make sure we will have to scroll to read the whole thing.
const char kSentence[] = "This is a sentence that you have to horizontally scroll to completely read.  If you can read this entire sentence without horizontally scrolling, you have amazing powers and should consider yourself gifted!";

// Standard call back function
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprev, PSTR cmdline, int ishow)
{
    HWND hwnd;
    MSG msg;
    WNDCLASSEX wndclassex = {0};

	// Fill the WNDCLASSEX fields we care about
	wndclassex.cbSize = sizeof(WNDCLASSEX);
	wndclassex.style = CS_HREDRAW | CS_VREDRAW; // Redraw the window if it's position or size changes
	wndclassex.lpfnWndProc = WinProc;
	wndclassex.hInstance = hinstance;
	wndclassex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); // Make the window background white
	wndclassex.lpszClassName = kClassName;
	wndclassex.hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(IDC_ARROW), IMAGE_CURSOR,	
											0, 0, LR_SHARED); // Load the default arrow cursor

    RegisterClassEx(&wndclassex); // Register the WNDCLASSEX with the OS
    
	// Here we set the style of the window
	// Notice how we use the style WS_HSCROLL, this creates the window with a 
	// horizontal scroll bar
	DWORD style = WS_SYSMENU | WS_HSCROLL;

	// Create the window
    hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, // Our Window has a border with a sunken edge
						  kClassName,
						  "www.GameTutorials.com -- Horizontal Scroll Bar",
						  style, // Style of window
						  CW_USEDEFAULT, // Windows picks upper-left X-pos of window 
						  CW_USEDEFAULT, // Windows picks upper-left Y-pos of window
						  kWinWid, 
						  kWinHgt,
						  NULL,
						  NULL,
						  hinstance,
						  NULL);

		// Error Check
		if(hwnd == NULL)
			return EXIT_FAILURE; // Couldn't create the window

	// Show and update the window
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
		else
		{
			// This is where the core of our application would go if we were writing one...
		}

	} // end of while(1)

	UnregisterClass(kClassName,hinstance); // Free up memory
	    return msg.wParam;
}

LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    HDC hdc; // Handle to our device context
    PAINTSTRUCT ps; // Paint structure for the WM_PAINT message
    
    // This represents the starting X position of our sentence.  Every time we scroll to
    // the right we'll increment this value and then use it as the starting position in
    // our sentence character array when we draw it to the screen.
	static int xPos; 
    static SCROLLINFO scrollInfo; // This is the Windows struct that will hold the information
								 // about our horizontal scroll bar

	// Do different this depending on the message
    switch(message)
    {
		case WM_CREATE:
		
			xPos = 0; // When we create the window, set our "xPos" to zero
			          // We haven't scrolled at all so it should be zero

			// Set the fields in our SCROLLINFO
				
			// This will ALWAYS needs to be set to the size/ of SCROLLINFO
			scrollInfo.cbSize = sizeof(SCROLLINFO);

			// This is the lowest x-pos you can scroll to (farthest to the left you can scroll to)
			scrollInfo.nMin = 0;
			
			// We want to be at least able to scroll as far as 
			// our sentence is long, so we set the highest x-pos (farthest we can scroll to the right)
			// to be the length of our sentence
			scrollInfo.nMax = strlen(kSentence); 
			
			// This represents the size of the page that is being scrolled on.  So in a MS Word
			// application, depending on the layout, this might number might change.  What it is
			// used to determine is the size of your scroll thumb (that's the little sliding
			// piece in the scroll bar).  The larger the number, the bigger the thumb.
			scrollInfo.nPage = 1; 

			// A SCROLLINFO has multiple member variables that can be set.  When we pass the 
			// structure to Windows to internally set these variables, Windows doesn't know which one
			// we truly want to use.  This is where the "fMask" variable comes in.  We set it with a bunch
			// of flags that represent the variables in the struct, that we actually want to set.
			// The SIF_PAGE flag says "We've set the nPage variable, please use it".
			// The SIF_RANGE flag says "We've set the nMin and nMax variables, please use them." 
			scrollInfo.fMask = SIF_PAGE | SIF_RANGE; 
			
			// Now that we've set our SCROLLINFO, we need to let the window know about it.
			// The following function does that for it, it's parameters are:
			// hwnd -- The handle to our window
			// SB_HORZ -- A flag specify what type of scroll bar we're dealing with, SB_HORZ means
			//			  horizontal scroll bar
			// &scrollInfo -- The address of the SCROLLINFO struct we filled
			// false -- A boolean value saying whether we want to redraw the scroll bar or not.
			SetScrollInfo(hwnd,SB_HORZ,&scrollInfo,false);
				return 0;
		
		// This message is sent to a window when a scroll event occurs in a window's standard
		// horizontal scroll bar
		case WM_HSCROLL:
			
			// If we get a horizontal scrolling message, we need to check what the message is
			// all about.  The LOWORD of the wparam tells us what the message is
			
			switch(LOWORD(wparam)) // Switch on the message
			{
				case SB_LINELEFT: // The left arrow of the scroll bar has been clicked
				
					// If the xPos is already at the minimum X position then we can't move
					// left anymore so simply return
					if(xPos == scrollInfo.nMin)
						return 0;
					else
						xPos = xPos - 1; // Otherwise, we want to scroll left by one unit 
						
					break;

				case SB_LINERIGHT: // The right arrow of the scroll bar has been clicked
				
					// If the xPos is already at the maximum X position then we can't move
					// right anymore so simply return
					if(xPos == scrollInfo.nMax)
						return 0;
					else
						xPos = xPos + 1; // Otherwise we want to scroll right by one unit
						
					break;
				
				// The user has dragged the thumb (that's the box inbetween the left and right arrows on 
				// the scroll bar) by clicking it, dragging it, and then releasing the mouse button
				case SB_THUMBPOSITION: 
					
					// We need to know the new position of the scroll bar.  It is given to us 
					// by the HIWORD of the WPARAM
					xPos = HIWORD(wparam); 
						break;

				default:

					// **NOTE**	There are other messages that could be sent but we're not 
					//			going to do anything if we receive 'em.  As always, for the 
					//			full unedited scoop, check out MSDN.					
					break;

			} // end of switch(LOWORD(wparam))
			
			// Alright if we get here, we know that we've updated our scroll bar's position
			// so we need to fill the SCROLLINFO with that information and then tell the 
			// window about it
			
			scrollInfo.nPos = xPos; // Set the new position		
			scrollInfo.fMask = SIF_POS; // Set the flag saying "We're use the nPos value in SCROLLINFO"
			
			// Set the new position.
			// **NOTE** We pass true because we want the scroll bar redrawn to update it's
			//			thumb position
			SetScrollInfo(hwnd, SB_HORZ, &scrollInfo, true);
			
			// Lastly, we need to redraw the window to display the correct portion of the
			// sentence.  What InvalidateRect does for us is specify a "dirty" region of 
			// the window that needs to be repainted.  When the window doesn't have any other messages
			// in it's message queue, it will generate a WM_PAINT message, and repaint all the 
			// areas in it's dirty region.
			// By parameter:
			// hwnd -- Handle to the window who is to be repainted
			// NULL -- Rectangle area of the window to repaint.  By passing NULL we are requesting
			//		   that the entire client area of the window be repainted
			// TRUE -- This is a boolean value that says whether the background should be redrawn
			//		   or not.  For us, our background is solid white, and we want it redrawn so
			//		   the sentence text doesn't draw over itself.  Typically, this value will be set
			//		   to true.
			InvalidateRect(hwnd,NULL,TRUE);
				return 0;

		// This message is sent when the window needs to be repainted
		case WM_PAINT:

			// Begin painting to the window and get the HDC associated
			// with the window
			hdc = BeginPaint(hwnd, &ps); 
			
			/*
				Now we need to be a tad tricky.  We want to begin drawing our sentence
				at (kStartX, kStartY).  However, whenever we scroll to the right, we need
				to begin writing out the sentence farther to the right.
				
				kSentence is the pointer to the string.  It points to the first letter in
				the character string.  So if we add 1 to kSentence, it will point to the 
				second letter in the string.  If we add 2 to kSentence, it will point to the
				third letter, and so on and so forth.  Hence we pass (kSentence + xPos) as the
				starting character in the sentence when we draw it to the screen. 
				
				Lastly, we specify the number of characters to draw as strlen(kSentence) - xPos.
				This is because we want to draw the entire sentence minus the amount we've scrolled
				to the right.  
			*/	
			
			// TextOut draws text to the screen.
			// By parameter:
			// hdc -- Handle to the device context to draw to
			// kStartX -- Starting upper-left X position to draw to
			// kStartY -- Starting upper-left Y position to draw to
			// (kSentence + xPos) -- Pointer to the string to be drawn
			// strlen(kSentence) - xPos -- Length of string to be drawn
			TextOut(hdc, kStartX, kStartY, (kSentence + xPos), strlen(kSentence) - xPos);
			
			// We've finished painting to the window
			EndPaint(hwnd, &ps);
				return 0;

		// When the program is destroyed 
		case WM_DESTROY:
			
			PostQuitMessage(0);
				return 0;
    
	} // end of switch(message)

    return DefWindowProc(hwnd, message, wparam, lparam);
}

// Don't scroll past this!

/*
	 Here is another function you might useful in your scrolling needs:
	 
		GetScrollBarInfo() Retrieves information about a scroll bar
		GetScrollInfo() Retrieves a scroll bar's parameters
		EnableScrollBar() Enables/Disables a scroll bar
		
	If you have programming questions, be sure and post 'em at www.GameTutorials.com.
*/

/*-------------------------*\
|  Programmed by:  TheTutor |
|  ©2005 GameTutorials, LLC |
\*-------------------------*/
