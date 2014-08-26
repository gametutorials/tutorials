// Done by TheTutor

/*
	Sometimes everything just doesn't fit in one window.  Hence the need to scroll.  This tutorial
	shows how to use a vertical scroll bar with a window.
	
	No need for a large introduction, lets get to the code!
*/

// In Visual Studio 2005 Microsoft added safer versions (denoted by a "_s" suffix) to many 
// common C/C++ run time functions.  Currently the ISO Standards Committee for C and C++ 
// is determining if they are going to add them to the official language set.  Until then, 
// this define will remove warnings about using the "unsafe" versions of these functions.
#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <windows.h>

////////////////
// Constants //
//////////////

// Width and height of the window
const int kWinWid = 400;
const int kWinHgt = 200;

const char kClassName[] = "GT_VerticalScrollBars"; // Our WNDCLASSEX's name

//////////////
// Globals //
////////////

char *gText = NULL;

// Loads the text file specified by "fileName" into "gText"
// Returns true on success, false otherwise
bool LoadTextFile(const char *fileName);

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
	// Notice how we use the style WS_VSCROLL, this creates the window with a 
	// vertical scroll bar
	DWORD style = WS_SYSMENU | WS_VSCROLL;

	// Create the window
    hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, // Our Window has a border with a sunken edge 
						  kClassName,
						  "www.GameTutorials.com -- Vertical Scroll Bar",
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
			
	// Load a the text file for displaying
	if(!LoadTextFile("VerticalText.txt"))
		return EXIT_FAILURE; // No text file, no tutorial

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
    
    static int scrollAmt; // This holds the amount we've scrolled down
    static SCROLLINFO scrollInfo; // This is the Windows struct that will hold the information
								 // about our vertical scroll bar

	// Do different this depending on the message
    switch(message)
    {
		case WM_CREATE:
		
			scrollAmt = 0; // When we create the window, set our "scrollAmt" to zero,
						  // since we haven't scrolled yet

			// Now we set the fields of the SCROLLINFO
				
			// This will ALWAYS needs to be set to the size of SCROLLINFO
			scrollInfo.cbSize = sizeof(SCROLLINFO);

			// This is the lowest amount (y-pos) that we can scroll to
			scrollInfo.nMin = 0;
			
			// This is the maximum amount (y-pos) that we can scroll to
			// So why did we choose 256?  Well we don't know how large our text file is
			// going to be and how far we will need to be able scroll down.  To do this in
			// a more robust way, we should first load our text file, see how much we need
			// to scroll down and then set the maximum amount we can scroll.  But since we 
			// know the file being loaded ahead of time, we take a small shortcut and hard code
			// a value that will allow us to the see the entire file  
			scrollInfo.nMax = 256; 
			
			// This represents the size of the page that is being scrolled on.  So in a MS Word
			// application, depending on the layout, this might number might change.  What it is
			// used to determine is the size of your scroll thumb (that's the little sliding
			// piece in the scroll bar).  The larger the number, the bigger the thumb.
			scrollInfo.nPage = 1; 

			// A SCROLLINFO has multiple member variables that can be set.  When we pass the 
			// structure to Windows to internally set these variables, Windows doesn't know which ones
			// we truly want to use.  This is where the "fMask" variable comes in.  We set it with a bunch
			// of flags that represent the variables in the struct, that we actually want to set.
			// The SIF_PAGE flag says "We've set the nPage variable, please use it".
			// The SIF_RANGE flag says "We've set the nMin and nMax variables, please use them." 
			scrollInfo.fMask = SIF_PAGE | SIF_RANGE; 
			
			// Now that we've set our SCROLLINFO, we need to let the window know about it.
			// The following function does that for it, it's parameters are:
			// hwnd -- The handle to our window
			// SB_VERT -- A flag specify what type of scroll bar we're dealing with, SB_VERT means
			//			  vertical scroll bar
			// &scrollInfo -- The address of the SCROLLINFO struct we filled
			// false -- A boolean value saying whether we want to redraw the scroll bar or not.
			SetScrollInfo(hwnd,SB_VERT,&scrollInfo,false);
				return 0;
		
		// This message is sent to a window when a scroll event occurs in a window's standard
		// vertical scroll bar
		case WM_VSCROLL:
			
			// If we get a vertical scrolling message, we need to check what the message is
			// all about.  The LOWORD of the wparam tells us what the message is
			
			switch(LOWORD(wparam)) // Switch on the message
			{
				case SB_LINEUP: // The up arrow of the scroll bar has been clicked
				
					// If the scrollAmt is already at the minimum Y position then we can't move
					// up anymore so simply return
					if(scrollAmt == scrollInfo.nMin)
						return 0;
					else
						scrollAmt = scrollAmt - 1; // Otherwise, we want to scroll up by one unit 
						
					break;

				case SB_LINEDOWN: // The down arrow of the scroll bar has been clicked
				
					// If the scrollAmt is already at the maximum Y position then we can't move
					// down anymore so simply return
					if(scrollAmt == scrollInfo.nMax)
						return 0;
					else
						scrollAmt = scrollAmt + 1; // Otherwise we want to scroll down by one unit
						
					break;
				
				// The user has dragged the thumb (that's the box inbetween the left and right arrows on 
				// the scroll bar) by clicking it, dragging it, and then releasing the mouse button
				case SB_THUMBPOSITION: 
					
					// We need to know the new position of the scroll bar.  It is given to us 
					// by the HIWORD of the WPARAM
					scrollAmt = HIWORD(wparam); 
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
			
			scrollInfo.nPos = scrollAmt; // Set the new position		
			scrollInfo.fMask = SIF_POS; // Set the flag saying "We're use the nPos value in SCROLLINFO"
			
			// Set the new position.
			// **NOTE** We pass true because we want the scroll bar redrawn to update it's
			//			thumb position
			SetScrollInfo(hwnd, SB_VERT, &scrollInfo, true);
			
			// Lastly, we need to redraw the window to display the correct portion of the
			// text.  What InvalidateRect does for us is specify a "dirty" region of 
			// the window that needs to be repainted.  When the window doesn't have any other messages
			// in it's message queue, it will generate a WM_PAINT message, and repaint all the 
			// areas in it's dirty region.
			// By parameter:
			// hwnd -- Handle to the window who is to be repainted
			// NULL -- Rectangle area of the window to repaint.  By passing NULL we are requesting
			//		   that the entire client area of the window be repainted
			// TRUE -- This is a boolean value that says whether the background should be redrawn
			//		   or not.  For us, our background is solid white, and we want it redrawn so
			//		   the text doesn't draw over itself.  Typically, this value will be set
			//		   to true.
			InvalidateRect(hwnd,NULL,TRUE);
				return 0;

		// This message is sent when the window needs to be repainted
		case WM_PAINT:
		
			// Begin painting to the window and get the HDC associated
			// with the window
			hdc = BeginPaint(hwnd, &ps);
			
			// The paint struct contains the area of the window for redrawing.  We
			// want to subtract off the amount we've scrolled down, so that we show the
			// correct portion of text based on where we have vertically scrolled.
			ps.rcPaint.top -= scrollAmt;
						
			if(gText)
			{
				// DrawText(), as the name implies, this function draws text to the screen
				// By parameter:
				// hdc -- The handle to the device context to draw to
				// gText -- Pointer to the string for drawing
				// strlen(gText) -- Length of the string to draw
				// &(ps.rcPaint) -- Pointer to the bounding rectangle of where to draw the text
				// DT_WORDBREAK -- This parameter is for optional flags to DrawText().  This
				//				   specific flag says that if the text extends past the right 
				//				   side of it's bounding rectangle, do a word wrap and continue
				//				   the text on the next line.
				DrawText(hdc, gText, strlen(gText), &(ps.rcPaint), DT_WORDBREAK);
			}
				
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

// Loads the text file specified by "fileName" into "gText"
// Returns true on success, false otherwise
bool LoadTextFile(const char *fileName)
{
	FILE *file = NULL;
	
	file = fopen(fileName, "rt");
	
		// Error check
		if(!file)
			return false;
	
	int count = 0;		
	int temp = 0; // Temp variable to use for counting the number of characters in the file
	
	while(temp != EOF)
	{
		count++;
		temp = fgetc(file);
	}
	
	// Now that we know how many characters are in the file, lets create a buffer big 
	// enough to read them all in
	gText = new char[count];
	
		// Make sure we could allocate memory
		if(!gText)
			return false;
			
	// Rewind the file pointer to the beginning of the file
	rewind(file);
	
	// Read in the text		
	if(!fread(gText, sizeof(char), count, file))
		return false;
		
	// Make sure the string is terminated with a NULL character
	// **NOTE** When we counted the characters in the file, we counted the
	//			end of file character, so we have a extra character in our buffer
	//			that we can set to NULL
	gText[count - 1] = NULL;
	
	fclose(file); // Close the file
		return true;
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
