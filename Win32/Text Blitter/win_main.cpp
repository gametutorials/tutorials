//   Done by TheTutor

/*

  This is a "text-blitter" tutorial although what it really is demonstrating is how 
  to "parse sections of a .bmp out" -- Meaning we're only loading one .bmp, but this .bmp
  is comprised of many (26 to be exact) smaller .bmps that we're going to want to be able
  to access as if they were separate .bmps -- Got it?

  Now before we go any further, I want to point out this is NOT a good text blitter -- No
  I'm not writing bad code, it's just (for example) our Alphabet.bmp doesn't have 
  any punctuation OR even a "space character" -- So for blitting an English sentence it's
  simply not going to cut it -- But the fundamentals will be sound :)

*/

#include "win_main.h"

#define class_name "GameTutorials_TextBlitter"

// WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

// Frees up any memeory associated with an SText struct
void FreeText(SText &text);

// WinMain
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprev, PSTR cmdline, int ishow)
{
	SText textBlitter = {0}; // This is the object that holds the data for "text blitting"
	CWinObj win_obj; // Our window object that holds all the data about our window
  
	HWND hwnd;
    MSG msg;
    WNDCLASSEX wndclassex = {0};	// Zero out WNDCLASSEX
	
	// Fill in fields we care about -- If your unsure of what all these "fields" are
	// Check out an early tutorial or MSDN
	wndclassex.cbSize = sizeof(WNDCLASSEX);
    wndclassex.style = CS_HREDRAW | CS_VREDRAW;
    wndclassex.lpfnWndProc = WinProc;
    wndclassex.hInstance = hinstance;
	wndclassex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wndclassex.lpszClassName = class_name;
    wndclassex.hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(IDC_ARROW), IMAGE_CURSOR,
											0, 0, LR_SHARED);

    RegisterClassEx(&wndclassex);

	// Creating our window -- 
    hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, /// Window will have a "sunken-edge"
						  class_name,		
						  "www.GameTutorials.com -- A \"text-blitter\"",		
						  WS_SYSMENU | WS_CAPTION, // Window with title bar that's not resizable	
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
		
	// Init CWinObj
	if(win_obj.init(hwnd) == false)
		return EXIT_FAILURE; // Something bad happened

	// ** Fill the TEXT_BLITTER struct **

	// Load the bitmap
	textBlitter.hbitmap = (HBITMAP)LoadImage(hinstance,"Alphabet.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE);

		// Better error check...
		if(!(textBlitter.hbitmap))
			return EXIT_FAILURE;
		
	// Create a compatible HDC
	textBlitter.hdc = CreateCompatibleDC(win_obj.getHDC());

		// Better error check...
		if(!(textBlitter.hdc))
			return EXIT_FAILURE;
		
	// Select the hbitmap into the textBlitter
	textBlitter.old_bitmap = (HBITMAP)SelectObject(textBlitter.hdc,textBlitter.hbitmap);

	// ** Done filling TEXT_BLITTER struct **

	// Show and update window
    ShowWindow(hwnd, ishow);
    UpdateWindow(hwnd);
 
	PrintText(win_obj,textBlitter,"GameTutorials",0,50);
	PrintText(win_obj,textBlitter,"TheTutor",0,350);
	
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


	// Frees the memory associated with the WNDCLASSEX we registered
	UnregisterClass(class_name,hinstance);
		return msg.wParam;
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

		// This message gets sent when the window is terminated (closed, exited, you get the point)
		case WM_DESTROY:
	
			PostQuitMessage(0);
				return 0;
    
	} // end of switch(message)

    return DefWindowProc(hwnd, message, wparam, lparam);
}

// This will print "buff" to the screen starting at (x, y)
void PrintText(CWinObj &win_obj, SText &tb, char buff[MAX_LENGTH], int x, int y)
{
	// tempX and tempY will hold the upper left coord of where we're going to blit
	// to (ie win_obj.win_dc)
	int tempX = x;
	int tempY = y;

	// We'll keep looping until we've printed out the all of buff (either MAX_LENGTH characters
	// or until we hit a NULL character)
	for(int i = 0; (i < MAX_LENGTH) && (buff[i] != NULL); i++)
	{
		// Okay what we're doing here is taken the current letter in buff and making it
		// capital (this way we won't have to make two checks for 'a' and 'A')
		// The we're subtraction ALPHA_CONST which will give us an
		// integer (if buff[i] is a letter of the alphabet) between 0 and 25 --
		int index = toupper(buff[i]) - ALPHA_CONST;

		/* Here is how we have the letters laid out in our bitmap with 'A' starting at 0

		   0	1	2	3	4	5	6	7	8	9	10	11	12
		   13	14	15	16	17	18	19	20	21	22	23	24	25
		*/

		// Make sure our index is legal
		if((index >= 0) && (index <= 25))
		{
			int xOffset, yOffset;

			// Now look at the "diagram" of how our Alphabet.bmp is laid out (a few lines above)
			// Any "letter rect" on the top line is going to have an upper left y-ccord of 0
			// So for indexes 0 - 12 we know yOffset starts at 0 -- Each letter is exactly
			// LTR_WID (64 pixels) wide so the starting x-coord is going to be "index * LTR_WID"
			if(index < 13)
			{
				xOffset = index * LTR_WID;
				yOffset = 0;
			}
			else
			{
				/* 
				   If the index is 13 or greater we've moved 64 pixels down (the height of the
				   bitmap) for our y-offset (upper left y-coord of rect for blitting)
				   Our x-coord is going to work the same way (index * LTR_WID) it's just we have
				   to subtract 13 to get the index in the same range (i.e. 0 - 12)
				*/
				xOffset = (index - 13) * LTR_WID;
				yOffset = LTR_HGT;
			}

			// Now remember when we're blitting we only blitting one letter at a time
			// our letters are (64x64) so by parameter:
			// win_obj.getHDC() -- Where we want to blit to
			// tempX -- The upper left x-coord of where we want to blit to
			// tempY -- The upper left y-coord of where we want to blit to
			// LTR_WID -- The width (in pixels) of the RECT we want to blit to
			// LTR_HGT -- The height (in pixels) of the RECT we want to blit to
			// tb.hdc -- The source we want to blit from (where we getting the image to blit)
			// xOffset -- The upper left x-coord in tb.hdc to begin blitting
			// yOffset -- The upper left y-ccord in tb.hdc to begin blitting
			// SRCCOPY -- "source copy" take everything from the source (tb.hdc) and copy it
			//			  verbatim to the dest (win_obj.win_dc)
			BitBlt(win_obj.getHDC(),tempX,tempY,LTR_WID,LTR_HGT,tb.hdc,xOffset,yOffset,SRCCOPY);
		
		} // end of if((index >= 0) && (index <= 25))


		// Every time we blit a letter we need to move to the right LTR_WID (64 pixels) 
		// for the start of the next blit
		tempX += LTR_WID;

		// If our starting position is to close to the right of our window (if we can't FULLY
		// blit the next letter with out all of it or some of it going off the window) move down
		// to the next line
		if(tempX > (win_obj.getClientWid() - LTR_WID))
		{
			tempX = x; // Start back at "the left"
			tempY += LTR_HGT; // Effectively move to the next line
		}
	
	} // end of for(int i = 0; (i < MAX_LENGTH) && (buff[i] != NULL); i++)

}

// Free the memory in a SText struct
void FreeText(SText &text)
{
	// If old_bmp AND hdc has been assigned some memory
	if(text.old_bitmap && text.hdc)
	{
		SelectObject(text.hdc,text.old_bitmap); // Select back old bitmap

		DeleteObject(text.hbitmap); // Delete hbitmap
		DeleteDC(text.hdc); // Delete the HDC
	}
	
	// Zero out the variables
	text.hbitmap = text.old_bitmap = NULL;
	text.hdc = NULL;
}


