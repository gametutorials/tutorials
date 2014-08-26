//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	digiben@gametutorials.com		 //
//																		 //
//		$Program:		Text											 //
//																		 //
//		$Description:	Draw's "Hello World!" centered on the screen	 //
//																		 //
//***********************************************************************//


// When working with windows we want to print text sometimes, but black text
// can be boring after a while.  This tutorials shows you how to add color
// to your text.  We have a nice function called SetTextColor() that takes
// a RGB value (red green blue) for the desired color.  Go ahead and check
// out the tutorial below.


#include <windows.h>										// We need to include windows.h
#include <stdio.h>											// Include stdio.h for the basics

// Here is our WndProc for handling messages we receive from the operating system														
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Our WinMain function		
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{														
    HWND hwnd; // The handle to our window
    MSG msg; // The structure to hold the message from the OS
    WNDCLASSEX wndclass = {0}; // The window class
    
	// Init all the important fields in the WNDCLASSEX
	wndclass.cbSize = sizeof(wndclass); // Size of WNDCLASSEX		
	wndclass.style = CS_HREDRAW | CS_VREDRAW; // Style of WNDCLASSEX		
	wndclass.lpfnWndProc = WndProc; // Pointer to our window procedure
	wndclass.hInstance = hInstance; // Instance of our window						
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); // Background color of the window
	wndclass.hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(IDC_ARROW), IMAGE_CURSOR,
										  0, 0, LR_SHARED); // Set the cursor to be the default arrow cursor
	wndclass.lpszClassName = "GameTutorials"; // Name of our class

	// We need to register our windows class with the Windows OS.  This tells Windows that we have
	// a type of window we'd like to create and here are the basic parameters that define that window.
	RegisterClassEx(&wndclass);	
	
	// CreateWindow() returns a handle to the window, which we store in our HWND "hwnd".
    hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, // Extended window style that gives the border a sunken edge
						  "GameTutorials", // The window's class name
						  "www.GameTutorials.com - Color Text", // Title bar text
						  WS_OVERLAPPEDWINDOW, // Window style
						  CW_USEDEFAULT, // The top left corner X value of the window in screen coordinates.  Windows will pick a default X value.
						  CW_USEDEFAULT, // The top left corner Y value of the window in screen coordinates.  Windows will pick a default Y value.
						  640, // Desired width of the window
						  480, // Desired height of the window
						  NULL,	// This is the parent window handle.  Since we don't have a parent window, we set this to NULL
						  NULL,	// This is the window menu handle, but since we don't have a menu, we set this to NULL.
						  hInstance, // This is the programs instance handle.  We just pass it our hInstance from our WinMain().
						  NULL); // If we wanted to pass in a variable or structure to the "WndProc", we would do it here.

	ShowWindow(hwnd, iCmdShow); // Show the window
    UpdateWindow(hwnd); // Update (paint) the window
	
	while(1) // Loop forever
	{	
		// Get any messages the OS has for us
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// If the message says to quit, quit!
			if(msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg); // Translate from virtual keys to character messages						
			DispatchMessage(&msg); // Send the message to the WndProc
		}							
	}

	// Unregister (free up) our WNDCLASSEX
	UnregisterClass("GameTutorials",hInstance);			
    
	return msg.wParam; // Return the exit code for the program										
}

// The window procedure					
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{	
	// Static variables are variables that aren't erased after the function quits.  
	// WndProc gets called millions of times.  We want these variables vales to persist
	// for each call to WndProc, thus we make 'em static.
	static RECT clientRect = {0}; // This will hold the client dimensions of window
	
	TEXTMETRIC textInfo; // This a struct that contains the basic physical properties of a font							
	PAINTSTRUCT paintStruct; // A PAINTSTRUCT structure is something we need to paint (draw) when handling the WM_PAINT message.
	HDC hdc=NULL; // An HDC is a handle to a device context.  We use this handle as a way to communicate to the graphics card so we can draw things to the window.  
	int x, y; // These will be used to position our text.
	char szHello[]="Hello World!"; // The string to print, pretty original huh? :)

	// This checks what the message is.  Below we handle the messages that we care of about.
	// Remember there are HUNDREDS of messages we could handle, but we'll only deal with the 
	// following four.
    switch(iMsg)											
    {														
		// This message is sent when the window is created.
		case WM_CREATE:										
			// Upon creation we need to get the rectangular dimensions of the client area
			// of our window. The client area is the area of the window minus the title bar and border
			// surrounding the window.
			GetClientRect(hwnd, &clientRect);
			break;											

		// This message is sent to the WndProc after the windows size has changed
		case WM_SIZE:
			
			// Since the window has been resized, we need to get our client area again.										
			GetClientRect(hwnd, &clientRect);
			break;				

		// This message is sent to the WndProc when the window needs to be repainted.  This might be if
		// we moved the window, resized it, or maximized it, or if another window was covering ours.
		case WM_PAINT:
			// This is how we get an HDC to paint with.  BeginPaint() passes back an hdc and fills 
			// in the paintStruct structure.  The paintStruct holds optional information on how to paint
			// but all we really care about is getting the HDC of the window so we can draw
			// to the window.									
			hdc = BeginPaint(hwnd, &paintStruct);			
			
			// Here we pass in the hdc of our window and our textInfo into GetTextMetrics().  
			// This fills in textInfo with all the font info for the window such as height, 
			// width, spacing, etc, etc.  At any one time, an HDC can have one font 
			// associated with it.  By default Windows assigns a font to a window upon creation.
			// This call to GetTextMetrics will obtain the information for the default font
			// associated with our window that we created.
			GetTextMetrics(hdc, &textInfo);				
			
			// Now we want to change the color of our default font
			// We can do this by the SetTextColor function.  Notice the macro RGB.  If you
			// are not familiar with color formats, be sure and check out the 
			// quick lesson on RGB at the bottom of this file.												
			SetTextColor(hdc, RGB(255, 0, 0));				
															
															
			// Alright, here's where some math comes in :)  It's just simple algebra so don't panic.
			// We want "Hello World!" to be centered in the screen.  So first we want to find the 
			// width and height of the window.  Remember we stored the client rect of the window
			// in our WM_CREATE message.  In general a RECT is a struct with four members, left
			// right, top, and bottom.  With (left,top) defining the upper left corner of the 
			// RECT and (right,bottom) defining the lower right corner of the RECT.  By definition,
			// a client rect's upper-left corner is (0,0).  This means we know the width and height
			// of the window is clientRect.right and clientRect.bottom respectively.
			
			// Our goal is to have our text to be centered in the screen. 
			// So, first we calculate an (x,y) that are dead center in our window.
			x = clientRect.right / 2;
			y = clientRect.bottom / 2;
															
			// Now we need to subtract half of the length of the string from
			// our x position so that the string of text can be centered in the 
			// window.  To do that, we calculate the number of characters in the string
			// by strlen(szHello).  Next we multiply by the average character width
			// for our font.  This will give us the total length in pixels of our text.
			// Dividing that by two and subtracting it from our x will give us the starting x
			// position to draw our text.											
			x = x - ((strlen(szHello) * textInfo.tmAveCharWidth) / 2);											
			
			// Okay x is ready to go, now we need to do a bit more math to y.  We want the 
			// text centered in the screen, so we'll subtract off half of the text's height
			// so it's centered nicely in the window.
			y = y - (textInfo.tmHeight / 2);											
						
			// Now we need to print the text, which TextOut will do nicely 
			// for us.  The first parameter, hdc, is the device context for 
			// drawing to.  The next two parameters, (x,y) is the starting upper-left
			// corner to begin drawing the text to.  The next parameter, szHello, is the
			// the text string to print.  The last parameter, is the number of 
			// characters to print.  As mentioned above, strlen() returns the number of 
			// characters in a string.    
			TextOut(hdc, x, y, szHello, strlen(szHello));			
			
			// And that about wraps up our our end paint.  This should be 
			// the last function you call after BeginPaint.  As the name suggests,
			// this tells Windows we're done drawing %)
			EndPaint(hwnd, &paintStruct);					
		    break;											

		// This message is sent when it's time to destroy the window
		case WM_DESTROY:									
			PostQuitMessage(0); // Quit the tutorial																	
			break;											
	}													

	// The DefWindowProc function calls the default window procedure to provide default processing
	// for any window messages that an application does not process.  This function ensures that every
	// message is processed. DefWindowProc is called with the same parameters received by the WndProc. 
	return DefWindowProc (hwnd, iMsg, wParam, lParam);		

} // End of the WndProc

// You are probably thinking, "Man, all this for a stupid window and a line of text?"  yes, yes..
// A lot of people don't LIKE windows programming because it can be very large, but then again a
// window is a very complicated thing.  What you will end up doing, once you get familiar with all
// this stuff, is putting most of this into function, so you don't have to continually copy 'n paste it.
// Like, for instance you would create your own CreateWindow() function that you would pass in a width,
// height, maybe a title, and some other parameters.  This function would then do ALL of that stuff for
// you, all in one line.  It is really nice once you get it set up.  You could conceivable do the 
// program we just finished, in a few lines, like this:

// #include <windows.h>
//
// CreateMyWindow(100, 100, 400, 400, "My Window!");
//
// while(Messages())
//		DisplayText("Hello World!", TEXT_CENTER);

// There you have it.  If you set up some functions, you could do all of this in 3 lines of code.
// See where I am going with this?  As you get better with the Win32 API you'll be able to 
// put together your own base window code to wrap up the Win32 functions.  Matter-of-factly,
// that exactly what Microsoft has done with their MFC and C# windowing languages.  But 
// if you want to run as fast as possible, you're going to want to deal directly with the 
// Win32 API.

// *Quick Lesson on RGB*:

// RGB stands for Red Green and Blue.  These are the primary colors of life :)
// With these colors, you can make any color.  In the RGB scale, 255 is the max.
// As you see in our program, we used RGB(255, 0, 0); which made our text RED.
// Here is how the macro works:  RGB( Red Intensity, Green Intensity, Blue Intensity);
// If RED is 255 (the max intensity) and GREEN and BLUE are zero, then RED is the final color.
// Here is GREEN:   RGB(0, 255, 0);   Here is BLUE:  RGB(0, 0, 255). Here is RED:  RGB(255, 0, 0);
// Can you guess what RGB(0, 0, 0) is?  What is the absence of color?  Well, what is the absence of light?
// Dark, right... so then the color would be BLACK: RGB(0, 0, 0);  Then what is RGB(255, 255, 255)?
// What color is the highest intensity of light?  It's WHITE: RGB(255, 255, 255);.  Now if you remember
// Messing around with paints when you were in elementary school, if you MIX colors they produce other colors.
// That is the same with the macro RGB().  Here are some mixed colors:
// PINK: RGB(255, 0, 255)	YELLOW: RGB(255, 255, 0)	CYAN (sky blue): RGB(0, 255, 255)
// Now if we want a lower intensity of a color, just decrement the value, say RGB(0, 125, 0) which is a darker green.
// Anyway, I think you get the point.  Just test color combinations to get the desired color, or go buy some paints :)
//
// © 2000-2006 GameTutorials
