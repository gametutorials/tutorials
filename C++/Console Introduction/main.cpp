// Done by TheTutor

// This tutorial is a introduction to console (DOS) programming on Win32.
// What we are going to do is use several console functions/data types to print a block 
// of text to the screen.  For every key press, a "block" of that character will be 
// blasted to the center of the console window.  Using console functions allows us to
// print characters (with color) anywhere on the console screen, among other things.  
// Let us go back and remember our friend, DOS...

#include <windows.h> // windows.h contains our DOS friendly functions that we will use
#include <iostream>
using namespace std;

int main()
{
	// So for starters, what is a HANDLE? 
	// Well if you look it up, you might find an answer like, "Handle - A handle to an object"
	// That's not really a ton of information.  A HANDLE is a pointer.  It is basically 
	// a way for us to talk to a specific "object" or "device".  In our case, we'll use our 
	// handles to talk to an input object (the keyboard) and output object (the monitor).
	HANDLE input_handle;
	HANDLE output_handle;

	// An INPUT_RECORD is a structure that is used to record input events that come from the
	// console input buffer.  This will get filled in with all the info we'll need to know 
	// about a key press.
	INPUT_RECORD input;

	// The COORD can be thought of as a 2D Cartesian point.  It defines the coordinates
	// of a character cell in a console screen buffer (ie the console window).
	COORD pos;

	// A DWORD stands for "double word".  This is a throw back to the old 16-bit Win32 systems
	// Back in those days, an "int" or "WORD" was 16-bits.  So a DWORD was double that or 
	// 32-bits.  With the arrival of the 32-bit OS from Win95-XP, a DWORD is essentially
	// the same thing as "int".
	DWORD info; // Some of the functions we'll use require passing in a DWORD to get back some
				// information.  We will use this variable for passing into those functions

	// This is one HUGE struct name eh?  This bad boy holds information about 
	// our console screen buffer (our console window) -- Here's a break down of each
	// member inside of the structure:
	// dwSize -- Size, in character columns and rows, of the screen buffer. 
	// dwCursorPosition -- Column and row positions of the cursor in the screen buffer. 
	// wAttributes -- Attributes of the characters written to a screen buffer using "console"
	//				  read/write functions
	// srWindow -- A SMALL_RECT struct that contains the screen buffer coordinates of the 
	//			   upper-left and lower-right corners of the console window.
	// dwMaximumWindowSize -- Maximum size of the console window, considering the current screen
	//						  buffer size, font, and screen size. 
	CONSOLE_SCREEN_BUFFER_INFO screen_info;

	// The first thing we need to do is fill our input and output handles.
	// GetStdHandle() will return the standard handle for input, output, or error device.
	// The three flags it can be passed are:
	// STD_INPUT_HANDLE -- Returns handle to the standard input device
	// STD_OUTPUT_HANDLE -- Returns handle to the standard output device
	// STD_ERROR_HANDLE -- Returns handle to standard error device
	input_handle = GetStdHandle(STD_INPUT_HANDLE);   // Get our input handle (ie handle to keyboard)
	output_handle = GetStdHandle(STD_OUTPUT_HANDLE); // Get our output handle (i.e. handle to 
													 // console buffer a.k.a console window that 
												     // is displayed on the monitor)  

	// Next on the list is to fill our "screen_info" with information about our console
	// window.  Since we did not set-up our console window, we'll get the default information.
	// **NOTE** This can be set by the user, so depending on a persons settings it can be 
	//			different.
	// By parameter:
	// output_handle -- A HANDLE to our console screen buffer
	// &screen_info -- A pointer to a CONSOLE_SCREEN_BUFFER_INFO to be filled
	GetConsoleScreenBufferInfo(output_handle, &screen_info);

	// One last thing to do before we get going,
	// the SetConsoleMode() function lets us set the mode of our console window (specify how
	// it should act).  For instance, if we wanted to enable mouse input in our 
	// console window, we'd use this function.  By passing in the ENABLE_PROCESSED_INPUT
	// flag we're saying, "Hey let CRTL+C quit the app and handle backspaces, carriage returns,
	// and similar system type key strokes for us".
	SetConsoleMode(input_handle, ENABLE_PROCESSED_INPUT);

	// We've filled in some of our data structures now it's time to put 'em to use!

	// Until somebody hits CTRL+C we'll keep looping
	while(1)
	{
		// Now, inside the loop we set our cursor position to start drawing from.
		// Every time the user presses a key, we're going to display that key (for instance 'a')
		// ten times centered in the console window.  Using the "srWindow" of our "screen_info",
		// we'll do a little math to make sure that this string of 10 characters is always centered.
		pos.X = screen_info.srWindow.Right / 2 - 5; // Center X - (1/2 string length) 
		pos.Y = screen_info.srWindow.Bottom / 2;    // Center Y

		// This function does just as it is named.  It will set the position of the
		// cursor in our console window to the COORD passed in.
		SetConsoleCursorPosition(output_handle, pos);

		// Here we read the input.
		// By parameter:
		// input_handle -- The HANDLE to the console input buffer (keyboard for us).
		// &input -- A pointer to a INPUT_RECORD that will be filled with the input data.
		// 1 -- Number of INPUT_RECORD's we want to read.
		// &info -- A pointer to a DWORD that will be filled with the number of INPUT_RECORD's
		//			actually read.
		ReadConsoleInput(input_handle, &input, 1, &info);

		// Check to see if a key was pushed down
		if(input.Event.KeyEvent.bKeyDown)
		{	
			// Retrieve the ASCII char that was pressed
			char theChar = (char)input.Event.KeyEvent.uChar.AsciiChar;

			// Print out it's ASCII character 10 times
			for(int i = 0; i < 10; ++i)
				cout << theChar;
		}
	}

	return 0; // Success sure is sweet!
}

// Hot Diggity DOS
/*
	Well that sums it up for an introduction to console (DOS) programming using the Win32 API.
	We've covered some of the major functions, but there's still more we didn't even touch.  As
	an extra credit assignment look up the following API's and see if you can put 'em to good
	use:

		WriteConsoleInput()
		ReadConsole()
		WriteConsole()
		FlushConsoleInputBuffer()

	If you have any questions about this tutorial or programming/game programming in general, be
	sure to post 'em to www.GameTutorials.com.
*/

/*----------------------------*\
|  TheTutor                    |
|  thetutor@gametutorials.com  |
|  © 2000-2005 GameTutorials   |
\*----------------------------*/
